#!/usr/bin/env bash
# Sourceable renderers and lifecycle helpers for ember's user units.
#
# Two features ship as systemd user units because both need to run as the user
# and outlive a single shell:
#
#   ui-watch    draws the store-page badge for as long as Steam runs. It blocks,
#               polls the DevTools endpoint, and exits when the client is gone.
#   cloud-push  one-shot `ember cloud push`, driven by a timer.
#
# Units are sentinel-marked so removal never touches a foreign file of the same
# name, and every manager call is best-effort: a machine without a reachable
# systemd --user still gets a working install, it just does not get the units.

: "${ECU_SYSTEMCTL:=systemctl}"

ECU_UI_SERVICE=ember-ui-watch.service
ECU_CLOUD_SERVICE=ember-cloud-push.service
ECU_CLOUD_TIMER=ember-cloud-push.timer
ECU_UNIT_SENTINEL='# X-Ember-Unit=true'

# XDG_CONFIG_HOME, absolute or nothing. Kept local so the library has no
# dependency on another sourced file.
ecu_config_home() {
	case "${XDG_CONFIG_HOME:-}" in
		/*) printf '%s\n' "$XDG_CONFIG_HOME" ;;
		*) printf '%s\n' "${HOME:-/tmp}/.config" ;;
	esac
}

: "${ECU_UNIT_DIR:=$(ecu_config_home)/systemd/user}"

ecu_log() {
	printf 'ember-units: %s\n' "$*" >&2
}

# Where the CLI actually lives. The distro installer drops it beside the libs
# under ~/.local/share/ember, and a Nix-managed install puts it on PATH, so
# resolve once and bake the absolute path into the unit: systemd user units get
# a minimal PATH that may not include either.
ecu_cli_path() {
	local candidate
	for candidate in \
		"${EMBERDIR:-$HOME/.local/share/ember}/ember" \
		"$HOME/.local/share/ember/ember" \
		"$(command -v ember 2>/dev/null || true)"
	do
		if [ -n "$candidate" ] && [ -x "$candidate" ]; then
			printf '%s\n' "$candidate"
			return 0
		fi
	done
	return 1
}

# The Steam root ember reads for the DevTools marker. Kept in one place so the
# unit condition and the module agree.
ecu_steam_root() {
	local candidate
	for candidate in \
		"${HOME}/.local/share/Steam" \
		"${HOME}/.steam/steam" \
		"${HOME}/.steam/debian-installation"
	do
		if [ -d "$candidate" ]; then
			printf '%s\n' "$candidate"
			return 0
		fi
	done
	return 1
}

ecu_ui_service_content() {
	local cli
	cli="$(ecu_cli_path)" || {
		ecu_log "cannot locate the ember CLI"
		return 1
	}
	cat <<EOF
$ECU_UNIT_SENTINEL
[Unit]
Description=ember store-page badge watcher
Documentation=https://github.com/axioncs/ember
# The watcher exits on its own once the Steam client is gone; restarting it
# keeps the badge available across client restarts without a timer.
After=graphical-session.target

[Service]
Type=simple
ExecStart=$cli ui watch --quiet
Restart=on-failure
RestartSec=5s
# The loop polls the DevTools endpoint every couple of seconds. Nice keeps it
# off the desktop's critical path; it is a cosmetic layer, not a service.
Nice=10
IOSchedulingClass=idle

[Install]
WantedBy=default.target
EOF
}

ecu_cloud_service_content() {
	local cli
	cli="$(ecu_cli_path)" || {
		ecu_log "cannot locate the ember CLI"
		return 1
	}
	cat <<EOF
$ECU_UNIT_SENTINEL
[Unit]
Description=Push ember cloud saves to the configured rclone remote
Documentation=https://github.com/axioncs/ember
# A failed push is reported by the timer's own status; do not restart it in a
# loop, because the failure is usually a missing rclone remote rather than a
# transient network error.

[Service]
Type=oneshot
ExecStart=$cli cloud push
# rclone talks to a remote provider; give a slow link room, but never let a
# hung transfer wedge the timer.
TimeoutStartSec=600
Nice=10
IOSchedulingClass=idle
EOF
}

ecu_cloud_timer_content() {
	local interval="${EMBER_CLOUD_INTERVAL:-15min}"
	cat <<EOF
$ECU_UNIT_SENTINEL
[Unit]
Description=Periodic ember cloud save push

[Timer]
OnBootSec=5min
OnUnitActiveSec=$interval
# A run missed while the machine was off fires on resume rather than being
# skipped, so saves are not silently left behind.
Persistent=true
Unit=$ECU_CLOUD_SERVICE

[Install]
WantedBy=timers.target
EOF
}

# Return 0 after replacement, 1 when unchanged, or 2 on a write error.
ecu_write_changed() {
	local name="$1" content="$2" destination temporary
	destination="$ECU_UNIT_DIR/$name"
	if [ -e "$destination" ] || [ -L "$destination" ]; then
		[ -f "$destination" ] || return 2
		grep -qxF "$ECU_UNIT_SENTINEL" "$destination" 2>/dev/null || return 2
	fi
	temporary="$(mktemp "$ECU_UNIT_DIR/.${name}.tmp.XXXXXX")" || return 2
	if ! printf '%s\n' "$content" > "$temporary"; then
		rm -f -- "$temporary"
		return 2
	fi
	chmod 0644 "$temporary" 2>/dev/null || true
	if [ -f "$destination" ] && cmp -s -- "$temporary" "$destination"; then
		rm -f -- "$temporary"
		return 1
	fi
	if mv -fT -- "$temporary" "$destination"; then
		return 0
	fi
	rm -f -- "$temporary"
	return 2
}

ecu_systemctl() {
	local description="$1"
	shift
	ecu_log "systemctl --user $description"
	if ! "$ECU_SYSTEMCTL" --user "$@"; then
		ecu_log "$description failed"
		return 1
	fi
	return 0
}

# Enable the badge watcher now. Best-effort: an unreachable systemd --user must
# not fail the install.
ecu_enable_ui_watch() {
	ecu_systemctl daemon-reload daemon-reload || true
	ecu_systemctl "enable --now $ECU_UI_SERVICE" \
		enable --now "$ECU_UI_SERVICE" || true
	return 0
}

# Cloud sync is opt-in: a timer that runs `rclone sync` against an unconfigured
# remote produces a journal entry every interval, so install the units but
# leave them off until the user enables them.
ecu_install_cloud_units() {
	ecu_systemctl daemon-reload daemon-reload || true
	return 0
}

ecu_enable_cloud_push() {
	ecu_systemctl daemon-reload daemon-reload || true
	ecu_systemctl "enable --now $ECU_CLOUD_TIMER" \
		enable --now "$ECU_CLOUD_TIMER"
}

ecu_disable_cloud_push() {
	ecu_systemctl "disable --now $ECU_CLOUD_TIMER" \
		disable --now "$ECU_CLOUD_TIMER" || true
	return 0
}

# Return 0 when unit bytes changed (and the manager was notified), 1 when all
# files already match, or 2 when rendering/writing failed.
ecu_install_units() {
	local status changed=0 errors=0 unit destination content
	local ui_content cloud_content timer_content

	ui_content="$(ecu_ui_service_content)" || {
		ecu_log "cannot render $ECU_UI_SERVICE"
		return 2
	}
	cloud_content="$(ecu_cloud_service_content)" || {
		ecu_log "cannot render $ECU_CLOUD_SERVICE"
		return 2
	}
	timer_content="$(ecu_cloud_timer_content)" || {
		ecu_log "cannot render $ECU_CLOUD_TIMER"
		return 2
	}

	# Fail closed before publishing any sibling when an existing same-name unit
	# is not owned by this project.
	for unit in "$ECU_UI_SERVICE" "$ECU_CLOUD_SERVICE" "$ECU_CLOUD_TIMER"; do
		destination="$ECU_UNIT_DIR/$unit"
		if [ -e "$destination" ] || [ -L "$destination" ]; then
			if [ ! -f "$destination" ] \
			   || ! grep -qxF "$ECU_UNIT_SENTINEL" "$destination" 2>/dev/null; then
				ecu_log "preserving foreign unit: $destination"
				return 2
			fi
		fi
	done

	if ! mkdir -p -- "$ECU_UNIT_DIR"; then
		ecu_log "cannot create unit directory: $ECU_UNIT_DIR"
		return 2
	fi

	for unit in "$ECU_UI_SERVICE" "$ECU_CLOUD_SERVICE" "$ECU_CLOUD_TIMER"; do
		case "$unit" in
			"$ECU_UI_SERVICE")    content="$ui_content" ;;
			"$ECU_CLOUD_SERVICE") content="$cloud_content" ;;
			*)                    content="$timer_content" ;;
		esac
		if ecu_write_changed "$unit" "$content"; then
			status=0
		else
			status=$?
		fi
		case "$status" in
			0) changed=1 ;;
			1) : ;;
			*) errors=1 ;;
		esac
	done

	[ "$errors" -eq 0 ] || return 2
	[ "$changed" -eq 1 ] && return 0
	return 1
}

ecu_remove_units() {
	local unit path changed=0 errors=0
	local -a owned=()
	for unit in "$ECU_UI_SERVICE" "$ECU_CLOUD_SERVICE" "$ECU_CLOUD_TIMER"; do
		path="$ECU_UNIT_DIR/$unit"
		[ -f "$path" ] || continue
		grep -qxF "$ECU_UNIT_SENTINEL" "$path" 2>/dev/null || continue
		owned+=("$path")
	done
	[ "${#owned[@]}" -gt 0 ] || return 1

	ecu_systemctl "disable --now $ECU_CLOUD_TIMER" \
		disable --now "$ECU_CLOUD_TIMER" "$ECU_UI_SERVICE" || true
	for path in "${owned[@]}"; do
		if rm -f -- "$path"; then
			changed=1
		else
			ecu_log "cannot remove unit: $path"
			errors=1
		fi
	done
	if [ "$changed" -eq 1 ]; then
		ecu_systemctl daemon-reload daemon-reload || true
		ecu_systemctl reset-failed reset-failed || true
	fi
	[ "$errors" -eq 0 ] || return 2
	return 0
}