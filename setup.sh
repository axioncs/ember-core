#!/bin/bash
# ember installer / uninstaller for the tarball release.
#
# Launch model: the distributor's `steam` binary and its "Steam" desktop entry
# are never touched. ember builds a launcher at $EMBERDIR/path/steam (the only
# thing that sets LD_AUDIT) and exposes it as `steam-ember` on PATH, plus a
# separate "Wired Steam" desktop entry. Start Wired Steam for ember; start
# Steam for the vanilla client.
#
# Upgrade and uninstall undo the desktop-entry and /usr/bin/steam patches that
# older releases applied, restoring them from their tagged backups.

EMBERDIR="$HOME/.local/share/ember"
EMBERLIB="$EMBERDIR/ember-core.so"
OS_RELEASE_FILE="${OS_RELEASE_FILE:-/etc/os-release}"

# User-local applications dir (XDG override always wins over system-wide).
USER_APPS="${XDG_DATA_HOME:-$HOME/.local/share}/applications"
# The "Wired Steam" entry. Its own file name, so the distributor's
# steam.desktop keeps pointing at the vanilla client and steam:// links keep
# resolving to it.
WIRED_ENTRY="$USER_APPS/wired-steam.desktop"
SYS_DESKTOP="/usr/share/applications/steam.desktop"
LEGACY_USER_DESKTOP="$USER_APPS/steam.desktop"
LEGACY_USER_AUTOSTART="${XDG_CONFIG_HOME:-$HOME/.config}/autostart/steam.desktop"

# Where `steam-ember` lands. XDG_BIN_HOME wins when the user set one.
BIN_DIR="${XDG_BIN_HOME:-$HOME/.local/bin}"
STEAM_EMBER="$BIN_DIR/steam-ember"

# Markers older releases put in desktop entries they had patched. Only used to
# find and restore those entries; nothing writes them any more.
EMBER_TAG="X-Ember-Patched=true"
EMBER_ALT_TAG="X-Ember-Alternative=true"

DC_BACKUP_ROOT="$EMBERDIR/backup"
LEGACY_LAUNCHER_BACKUP="$EMBERDIR/system-launcher-backup"
SETUP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ -f "$SETUP_DIR/tools/ember-units.lib.sh" ]; then
	# shellcheck source=/dev/null
	. "$SETUP_DIR/tools/ember-units.lib.sh"
fi

# A source build records the commit it came from, so `ember update` can tell a
# source install from a release install. Without this it compares the latest
# tag to the latest tag and reports "up to date" forever.
ember_version_stamp() {
	local tag="${EMBER_VERSION:-unknown}"
	if [ -n "${EMBER_SOURCE_REV:-}" ]; then
		printf '%s+source-%s\n' "$tag" "$(printf '%s' "$EMBER_SOURCE_REV" | cut -c1-7)"
	else
		printf '%s\n' "$tag"
	fi
}

# ============================================================================
# Pretty output (colors + box-drawing). Falls back to plain text when stdout
# is not a TTY or the terminal does not advertise colour support.
#
# Palette: "moonlit night" — cool blues for structure, silver-white for the
# moon glyph, standard semantic colours for status. Uses 256-colour escapes
# when the terminal supports them, otherwise degrades to 8-colour ANSI.
# ============================================================================

if [ -t 1 ] && [ -z "${NO_COLOR:-}" ] && [ "${TERM:-dumb}" != "dumb" ]; then
	# Detect 256-colour support. tput is the reliable check; default to true
	# if tput is unavailable but COLORTERM looks modern.
	if command -v tput >/dev/null 2>&1 && [ "$(tput colors 2>/dev/null || echo 0)" -ge 256 ]; then
		HAS_256=1
	elif [ "${COLORTERM:-}" = "truecolor" ] || [ "${COLORTERM:-}" = "24bit" ]; then
		HAS_256=1
	else
		HAS_256=0
	fi

	BOLD=$'\033[1m'
	DIM=$'\033[2m'
	NC=$'\033[0m'

	if [ "$HAS_256" = 1 ]; then
		ACCENT=$'\033[38;5;153m'
		DIM2=$'\033[38;5;75m'
		BRIGHT=$'\033[38;5;231m'
		MUTED=$'\033[38;5;110m'
		GREEN=$'\033[38;5;114m'
		YELLOW=$'\033[38;5;221m'
		RED=$'\033[38;5;203m'
	else
		ACCENT=$'\033[1;34m'
		DIM2=$'\033[0;36m'
		BRIGHT=$'\033[1;37m'
		MUTED=$'\033[0;34m'
		GREEN=$'\033[0;32m'
		YELLOW=$'\033[0;33m'
		RED=$'\033[0;31m'
	fi
else
	BOLD=""; DIM=""; NC=""
	ACCENT=""; DIM2=""; BRIGHT=""; MUTED=""
	GREEN=""; YELLOW=""; RED=""
fi

print_banner() {
	echo ""
	echo -e "${ACCENT}${BOLD}"
	echo "┌─────────────────────────────────────────────────────────┐"
	printf "│             ${BRIGHT}${BOLD}◯${NC}${ACCENT}${BOLD}  ember installer                   │\n"
	echo "└─────────────────────────────────────────────────────────┘"
	echo -e "${NC}"
}

print_section() {
	echo ""
	echo -e "${DIM2}─────────────────────────────────────────────────────────${NC}"
	echo -e "${DIM2}${BOLD}❯ $1${NC}"
	echo -e "${DIM2}─────────────────────────────────────────────────────────${NC}"
}

log_info()    { echo -e "${DIM2}→${NC} $1"; }
log_success() { echo -e "${GREEN}✓${NC} $1"; }
log_warn()    { echo -e "${YELLOW}⚠${NC} $1"; }
log_error()   { echo -e "${RED}✗${NC} $1"; }
log_step()    { echo -e "${ACCENT}•${NC} $1"; }

print_install_complete() {
	echo ""
	echo -e "${GREEN}${BOLD}"
	echo "┌─────────────────────────────────────────────────────────┐"
	echo "│        ✓ ember Installation Completed!           │"
	echo "└─────────────────────────────────────────────────────────┘"
	echo -e "${NC}"
	echo ""
	echo -e "   Start ${BOLD}Wired Steam${NC} (or run ${GREEN}steam-ember${NC}) to play with ember."
	echo -e "   Plain ${BOLD}Steam${NC} and ${BOLD}steam${NC} stay vanilla."
	echo ""
}

print_uninstall_complete() {
	echo ""
	echo -e "${GREEN}${BOLD}"
	echo "┌─────────────────────────────────────────────────────────┐"
	echo "│               ✓ Uninstall Complete!                     │"
	echo "└─────────────────────────────────────────────────────────┘"
	echo -e "${NC}"
	echo ""
	echo -e "   Restart your terminal and Steam for changes to take effect."
	echo ""
}

# ============================================================================
# Detection helpers
# ============================================================================

# True on immutable / atomic systems (Bazzite, SteamOS, Fedora Atomic/ublue, …)
# where /usr is read-only: an uninstall cannot rewrite a system-wide file there,
# so say so instead of asking for sudo that cannot help.
is_immutable_distro() {
	case "${EMBER_IMMUTABLE:-}" in
		1|yes|true) return 0 ;;
		0|no|false) return 1 ;;
	esac

	local id="" like="" variant=""
	if [ -r "$OS_RELEASE_FILE" ]; then
		id="$(
			# shellcheck disable=SC1090
			. "$OS_RELEASE_FILE" >/dev/null 2>&1
			printf '%s' "${ID:-}"
		)"
		like="$(
			# shellcheck disable=SC1090
			. "$OS_RELEASE_FILE" >/dev/null 2>&1
			printf '%s' "${ID_LIKE:-}"
		)"
		variant="$(
			# shellcheck disable=SC1090
			. "$OS_RELEASE_FILE" >/dev/null 2>&1
			printf '%s' "${VARIANT_ID:-}"
		)"
	fi
	id="${id,,}"; like="${like,,}"; variant="${variant,,}"
	case " $id $like " in
		*" bazzite "*|*" steamos "*|*" steamdeck "*|*" holoiso "*|\
		*" silverblue "*|*" kinoite "*|*" sericea "*|*" onyx "*|\
		*" bluefin "*|*" aurora "*|*" ucore "*) return 0 ;;
	esac
	case "$variant" in silverblue|kinoite|sericea|onyx|*atomic*) return 0 ;; esac
	command -v rpm-ostree >/dev/null 2>&1 && return 0
	command -v steamos-readonly >/dev/null 2>&1 && return 0
	if command -v findmnt >/dev/null 2>&1; then
		case ",$(findmnt -no OPTIONS / 2>/dev/null)," in *,ro,*) return 0 ;; esac
	fi
	return 1
}

# Find the real Steam binary. Distros vary:
#   /usr/games/steam        Debian, Ubuntu, Mint (steam-installer)
#   /usr/bin/steam          Arch, Fedora, openSUSE, Manjaro, Pop!_OS
#   /usr/local/bin/steam    manual installs
detect_steam_binary() {
	local c
	for c in /usr/games/steam /usr/bin/steam /usr/local/bin/steam; do
		if [ -x "$c" ]; then
			echo "$c"
			return 0
		fi
	done
	command -v steam 2>/dev/null
}

# True once Steam's client data root exists (~/.steam/steam -> .../steam.sh).
# Before that, the launcher is Debian's "Install Steam" stub.
steam_data_bootstrapped() {
	[ -L "$HOME/.steam/steam" ] || return 1
	local root
	root="$(readlink -e "$HOME/.steam/steam" 2>/dev/null || true)"
	[ -n "$root" ] && [ -x "$root/steam.sh" ]
}

# True when a .desktop file (or launcher script) carries a marker an older ember
# put there. Nothing writes these any more; this only drives the upgrade and
# uninstall restore paths.
has_ember_marker() {
	[ -f "$1" ] || return 1
	grep -qF "$EMBER_TAG" "$1" 2>/dev/null && return 0
	grep -qF "$EMBER_ALT_TAG" "$1" 2>/dev/null && return 0
	grep -qF "ember" "$1" 2>/dev/null
}


# ============================================================================
# Steam process management
# ============================================================================

# Stop any running Steam so the wrapper / desktop entry takes effect on next
# launch. Tries graceful shutdown first, falls back to SIGTERM then SIGKILL.
kill_steam() {
	if ! pgrep -x steam >/dev/null 2>&1 \
	   && ! pgrep -f '/steam$|/steam ' >/dev/null 2>&1 \
	   && ! pgrep -f 'steamwebhelper' >/dev/null 2>&1; then
		log_success "No running Steam process detected"
		return 0
	fi

	log_info "Stopping running Steam processes"

	if command -v steam >/dev/null 2>&1; then
		# Sandboxed launchers replicate the caller's cwd inside their sandbox.
		# The installer may still be inside a temporary extraction directory,
		# which does not exist in that sandbox. Keep the shutdown request local
		# to this subshell so setup.sh itself retains its current directory.
		(
			cd "$HOME" 2>/dev/null || cd / 2>/dev/null || exit 0
			steam -shutdown >/dev/null 2>&1
		) || true
	fi

	for _ in 1 2 3 4 5; do
		if ! pgrep -x steam >/dev/null 2>&1 \
		   && ! pgrep -f 'steamwebhelper' >/dev/null 2>&1; then
			log_success "Steam stopped"
			return 0
		fi
		sleep 1
	done

	pkill -TERM -x steam 2>/dev/null || true
	pkill -TERM -f 'steamwebhelper' 2>/dev/null || true
	pkill -TERM -f '/steam$|/steam ' 2>/dev/null || true
	sleep 2

	if pgrep -x steam >/dev/null 2>&1 \
	   || pgrep -f 'steamwebhelper' >/dev/null 2>&1 \
	   || pgrep -f '/steam$|/steam ' >/dev/null 2>&1; then
		log_warn "Steam still running — sending SIGKILL"
		pkill -KILL -x steam 2>/dev/null || true
		pkill -KILL -f 'steamwebhelper' 2>/dev/null || true
		pkill -KILL -f '/steam$|/steam ' 2>/dev/null || true
		sleep 1
	fi

	log_success "Steam stopped"
}

# ============================================================================
# Install steps
# ============================================================================

install_ember()
{
	LIB="./bin/ember-core.so"

	if [ ! -f "$LIB" ]; then
		log_error "$LIB not found"
		echo ""
		echo "   If you're a developer, build it first:"
		echo -e "     ${GREEN}./build-docker.sh${NC}  ${MUTED}# For releases (requires Podman/Docker)${NC}"
		echo -e "     ${GREEN}make${NC}               ${MUTED}# For local testing${NC}"
		echo ""
		exit 1
	fi

	log_info "Installing ember libraries"
	mkdir -p "$EMBERDIR" || exit 1
	# Record which build this is. A tarball install carries its tag; a checkout
	# carries tag+source-<sha>, so `ember update` never reports a source build
	# as current with the release it was built beside.
	if [ -z "${EMBER_SOURCE_REV:-}" ] && [ -d "$SETUP_DIR/.git" ]; then
		EMBER_SOURCE_REV="$(git -C "$SETUP_DIR" rev-parse HEAD 2>/dev/null || true)"
	fi
	ember_version_stamp > "$EMBERDIR/VERSION"
	cp -v ./bin/* "$EMBERDIR/" | sed "s|^|   ${MUTED}${NC}|"
	# User units for the badge watcher and the cloud-push timer. Shipped beside
	# the libs so the uninstaller and a later reinstall can find them.
	install -m 0644 ./tools/ember-units.lib.sh "$EMBERDIR/ember-units.lib.sh" 2>/dev/null || \
		cp ./tools/ember-units.lib.sh "$EMBERDIR/ember-units.lib.sh"
	# Payload auto-updater. Runs detached from the wrapper; it only swaps the
	# libs above when a newer release exists, so keep it next to them.
	install -m 0755 ./tools/ember-autoupdate.sh "$EMBERDIR/ember-autoupdate.sh" 2>/dev/null || \
		{ cp ./tools/ember-autoupdate.sh "$EMBERDIR/ember-autoupdate.sh"; chmod +x "$EMBERDIR/ember-autoupdate.sh"; }
	# Which release this payload came from. The updater only touches an install
	# it can identify (a hand-built payload has no version file and is left
	# alone), so a release zip has to carry its own tag.
	if [ -r ./res/release-version.txt ]; then
		tr -d '[:space:]' < ./res/release-version.txt > "$EMBERDIR/version"
	fi
	log_success "Libraries installed at $EMBERDIR"
	echo ""
}

create_steam_wrapper()
{
	log_info "Creating Steam wrapper with ember injection"

	mkdir -p "$EMBERDIR/path"

	# The wrapper resolves the real Steam binary at runtime so the install is
	# portable across distros (and survives moves between, e.g., a Debian-style
	# /usr/games/steam and an Arch-style /usr/bin/steam).
	cat > "$EMBERDIR/path/steam" << 'EOF'
#!/bin/sh
# ember wrapper. Injects ember via LD_AUDIT (rtld-audit) so the audit
# namespace can't interpose on Steam's own copies of protobuf / yaml-cpp /
# libstdc++. ember-library-inject.so redirects libcurl to the system copy and must
# come first. CloudRedirect (cloud saves) is injected separately via LD_PRELOAD
# (see its block below).
EMBERDIR="$HOME/.local/share/ember"

# Desktop entries from older installs may invoke this wrapper with our audit
# DSOs already present in LD_AUDIT.  Remove only our known objects immediately,
# before helper commands (readlink, stat, awk, …) are spawned.  An inherited
# third-party auditor is preserved and is reattached after the guard has made
# its decision.
ember_strip_own_auditors() {
	_ember_old_ifs="$IFS"
	IFS=:
	_ember_clean=""
	for _ember_audit in ${LD_AUDIT:-}; do
		case "$_ember_audit" in
			''|*ember-core.so|*ember-library-inject.so|*libember-core.so|*libember-ember-library-inject.so)
				continue ;;
			*)
				_ember_clean="${_ember_clean:+$_ember_clean:}$_ember_audit" ;;
		esac
	done
	IFS="$_ember_old_ifs"
	printf '%s' "$_ember_clean"
}
EMBER_INHERITED_AUDIT="$(ember_strip_own_auditors)"
if [ -n "$EMBER_INHERITED_AUDIT" ]; then
	export LD_AUDIT="$EMBER_INHERITED_AUDIT"
else
	unset LD_AUDIT
fi

# Resolve the real Steam binary, skipping our own wrapper.
SELF="$(readlink -f "$0" 2>/dev/null || echo "$0")"
STEAM_BIN=""
EMBER_EXPLICIT_STEAM_BIN=0
# Override hook: an explicit, executable path wins. Lets power users pin a
# specific Steam binary and lets the wrapper guard's unit test inject a fake
# Steam; a no-op when unset.
if [ -n "${EMBER_STEAM_BIN:-}" ] && [ -x "${EMBER_STEAM_BIN:-}" ] && \
   [ "$(readlink -f "$EMBER_STEAM_BIN" 2>/dev/null || echo "$EMBER_STEAM_BIN")" != "$SELF" ]; then
	STEAM_BIN="$EMBER_STEAM_BIN"
	EMBER_EXPLICIT_STEAM_BIN=1
fi
if [ -z "$STEAM_BIN" ]; then
	for c in /usr/games/steam /usr/bin/steam /usr/local/bin/steam; do
		if [ -x "$c" ] \
			&& [ "$(readlink -f "$c" 2>/dev/null || echo "$c")" != "$SELF" ]; then
			STEAM_BIN="$c"
			break
		fi
	done
fi
if [ -z "$STEAM_BIN" ]; then
	# Fall back to PATH lookup, but skip ourselves.
	IFS=:
	for d in $PATH; do
		c="$d/steam"
		if [ -x "$c" ] \
			&& [ "$(readlink -f "$c" 2>/dev/null || echo "$c")" != "$SELF" ]; then
			STEAM_BIN="$c"
			break
		fi
	done
	unset IFS
fi
# No system launcher found.  Fall back to the steam.sh script inside the
# Steam data directory (it is what the system launcher would exec anyway
# once the bootstrap is set up).
if [ -z "$STEAM_BIN" ]; then
	for c in "${HOME}/.local/share/Steam/steam.sh" \
	         "${HOME}/.steam/steam/steam.sh" \
	         "${HOME}/.steam/debian-installation/steam.sh"; do
		if [ -x "$c" ]; then
			STEAM_BIN="$c"
			break
		fi
	done
fi
if [ -z "$STEAM_BIN" ]; then
	echo "ember: could not find the real Steam binary" >&2
	exit 127
fi
# Canonicalize relative paths before we touch cwd below. Absolute paths must
# remain verbatim because the bootstrap guard below matches those prefixes
# literally.
case "$STEAM_BIN" in
	/*) ;;
	*) STEAM_BIN="$(readlink -f "$STEAM_BIN" 2>/dev/null || echo "$STEAM_BIN")" ;;
esac

# Valve's distro launcher (bin_steam.sh, which /usr/bin/steam symlinks to on
# Fedora/Nobara) takes its whole identity from its own argv[0]:
#
#     STEAMPACKAGE="${0##*/}" ... *) log "Unknown Steam package"; exit 1
#
# Exec'ing it under any other name aborts the launch before Steam opens its
# logger: Steam silently never starts and no log is written anywhere. Never exec
# a launcher under a name it can reject - route through a `steam`-named alias for
# the same file. EMBER_POLICY_BIN keeps the resolved path so the bootstrap guard
# below still classifies the launcher by what was actually resolved.
EMBER_POLICY_BIN="$STEAM_BIN"
ember_launcher_name_safe() {
	case "${1##*/}" in
		steam|steambeta|bin_steam.sh|steam.sh) return 0 ;;
	esac
	return 1
}
ember_same_file() {
	_ember_a="$(readlink -f "$1" 2>/dev/null || true)"
	_ember_b="$(readlink -f "$2" 2>/dev/null || true)"
	[ -n "$_ember_a" ] && [ "$_ember_a" = "$_ember_b" ]
}
ember_normalize_launcher() {
	ember_launcher_name_safe "$STEAM_BIN" && return 0
	# A private alias we own, created on demand. Valve's distro launcher derives
	# its identity from argv[0] and aborts on anything but steam/bin_steam.sh, so
	# never exec a launcher under a name it rejects.
	_ember_alias_dir="$EMBERDIR/launcher-alias"
	_ember_alias="$_ember_alias_dir/steam"
	if mkdir -p "$_ember_alias_dir" 2>/dev/null; then
		if ! ember_same_file "$_ember_alias" "$STEAM_BIN"; then
			[ -L "$_ember_alias" ] || rm -f "$_ember_alias" 2>/dev/null || true
			ln -sfn "$STEAM_BIN" "$_ember_alias" 2>/dev/null || true
		fi
		if [ -x "$_ember_alias" ] && ember_same_file "$_ember_alias" "$STEAM_BIN"; then
			STEAM_BIN="$_ember_alias"
			return 0
		fi
	fi
	# Nothing aliasable: the data-dir steam.sh ignores its own name.
	for _ember_c in "${HOME}/.local/share/Steam/steam.sh" \
	               "${HOME}/.steam/steam/steam.sh" \
	               "${HOME}/.steam/debian-installation/steam.sh"; do
		if [ -x "$_ember_c" ]; then
			STEAM_BIN="$_ember_c"
			return 0
		fi
	done
	return 1
}
ember_normalize_launcher || true

# Some Steam launchers (Nix's FHS/bwrap wrapper) replicate our cwd inside
# their sandbox via --chdir. If we were started from a short-lived temp dir
# (an installer extracting to $(mktemp -d) before running us), the sandbox's
# isolated /tmp won't have it and bwrap fails to chdir. $HOME is always
# there, so pin cwd to it now that STEAM_BIN is resolved.
cd "$HOME" 2>/dev/null || cd / 2>/dev/null || true

ember_exec_vanilla() {
	# Safe mode must not depend on what a desktop entry or parent shell put in
	# the loader environment.  Unset the complete loader surface, not only our
	# own names, so a broken third-party preload cannot keep the recovery loop.
	exec env -u LD_AUDIT -u LD_PRELOAD -u LD_LIBRARY_PATH "$STEAM_BIN" "$@"
}

# Debian's Install Steam package stub is a valid executable before the client
# root exists, but it is not a Steam client and must never receive injection.
# An explicitly pinned binary is the caller's own choice and stays eligible.
ember_bootstrap_guard() {
	if [ "${EMBER_EXPLICIT_STEAM_BIN:-0}" = 1 ]; then
		return 0
	fi
	case "$EMBER_POLICY_BIN" in
		/usr/games/steam|/usr/bin/steam|/usr/local/bin/steam) ;;
		*) return 0 ;;
	esac
	[ -L "$HOME/.steam/steam" ] || return 1
	_ember_bootstrap_root="$(readlink -e "$HOME/.steam/steam" 2>/dev/null || true)"
	[ -n "$_ember_bootstrap_root" ] || return 1
	[ -f "$_ember_bootstrap_root/steam.sh" ] && \
		[ -x "$_ember_bootstrap_root/steam.sh" ]
}
if ! ember_bootstrap_guard; then
	ember_exec_vanilla "$@"
fi

# ---------------------------------------------------------------------------
# Crash-loop fail-safe (Game Mode boot protection).
#
# In a gamescope "Game Mode" session the supervisor relaunches Steam every time
# it exits. If our injected stack (ember via LD_AUDIT / CloudRedirect via
# LD_PRELOAD / the sidecar) goes incompatible with a freshly-updated Steam
# client and stalls the engine, the device loops on the splash forever and the
# user can never reach Desktop Mode to update. This guard counts boots that
# CRASHED AT STARTUP (Steam wrote an assert/crash minidump within the boot's
# first few minutes) and, past a threshold, LATCHES into a safe mode that starts
# Steam completely vanilla (no LD_AUDIT, no LD_PRELOAD, no sidecar) so the
# session comes up. The latch is scoped to THAT SESSION: a relaunch loop lives
# entirely inside one boot, so a boot is all the time pausing injection needs to
# cover, and every new session re-arms it. (A latch that outlived the boot left
# the user on a silently unhooked Steam - no plugin UI, no explanation - that
# only a payload reinstall could undo.) We deliberately key off the crash dump
# and NOT merely a short-lived session: switching Game Mode <-> Desktop, a client
# self-update restart, or a quick manual quit all end Steam fast but are NOT
# failures, and a clean kill never writes a dump. As a fast path, a startup crash whose
# steamclient.so differs from the last cleanly-booted one latches on the FIRST
# crash (a fresh client is the near-certain cause), so the user is not made to
# loop MAX_FAILS times for a known-cause break; a crash on an UNCHANGED client
# keeps the conservative MAX_FAILS threshold. Session/distro-agnostic: ChimeraOS/Bazzite
# (sessions.d STEAMCMD), SteamOS (steam-launcher PATH drop-in) and Desktop Mode
# all funnel through this one wrapper. Every step is best-effort and can never
# itself block the launch.
GUARD_DIR="${XDG_STATE_HOME:-$HOME/.local/state}/ember"
mkdir -p "$GUARD_DIR" 2>/dev/null || true
GUARD_LAST="$GUARD_DIR/last_launch"          # mtime = start of the most recent boot
GUARD_GFX="$GUARD_DIR/last_launch_display"   # 1/0: did the most recent launch have a display
GUARD_COUNT="$GUARD_DIR/boot_fail_count"
GUARD_SAFE="$GUARD_DIR/safe_mode"            # present => stay vanilla
GUARD_FP="$GUARD_DIR/safe_mode_fingerprint"  # payload id captured when latched
GUARD_SESSION="$GUARD_DIR/session_id"        # session the latch and fail count belong to
GUARD_CLIENT_LAST="$GUARD_DIR/last_client"   # steamclient.so id the most recent boot ran
GUARD_CLIENT_GOOD="$GUARD_DIR/good_client"   # steamclient.so id of the last boot that started cleanly
GUARD_LOG="$GUARD_DIR/guard.log"

# Tunables (overridable for testing).
[ -n "${EMBER_GUARD_MAX_FAILS:-}" ] || EMBER_GUARD_MAX_FAILS=3
[ -n "${EMBER_GUARD_STARTUP_SECS:-}" ] || EMBER_GUARD_STARTUP_SECS=180
[ -n "${EMBER_GUARD_DUMPS_DIR:-}" ] || EMBER_GUARD_DUMPS_DIR="/tmp/dumps"

guard_log() {
	printf '%s %s\n' "$(date '+%F %T' 2>/dev/null)" "$1" >> "$GUARD_LOG" 2>/dev/null || true
}
# Non-blocking, auto-dismissing notice (10s, normal urgency). NOT critical:
# the freedesktop spec lets the shell pin critical notifications on screen with
# no timeout (KDE does), which is why an earlier critical recovery notice never
# went away. Best-effort; never blocks the launch.
guard_notify() {
	if command -v notify-send >/dev/null 2>&1; then
		notify-send -u normal -t 10000 "Steam recovery mode" "$1" >/dev/null 2>&1 || true
	fi
}
guard_read_int() {
	_v="$(cat "$1" 2>/dev/null)"
	case "$_v" in ''|*[!0-9]*) printf 0 ;; *) printf '%s' "$_v" ;; esac
}
# Identify the machine's current session. The kernel's boot_id changes on every
# boot; btime (boot epoch) is the fallback. Empty when neither is readable, in
# which case the state simply never looks stale and the guard keeps its old
# persistent behaviour instead of re-arming on every launch.
guard_session_id() {
	_sid="$(cat /proc/sys/kernel/random/boot_id 2>/dev/null || true)"
	[ -n "$_sid" ] || _sid="$(sed -n 's/^btime //p' /proc/stat 2>/dev/null || true)"
	printf '%s' "$_sid"
}
# A launch with no display tells us nothing about the injected stack: Steam
# cannot open a display, dies inside libX11 (XQueryExtension) and writes an
# ordinary crash_*.dmp on its way out - vanilla Steam included. Scripted or
# remote launches must not be able to latch recovery mode for a graphical
# session that was never started.
guard_has_display() {
	[ -n "${DISPLAY:-}" ] || [ -n "${WAYLAND_DISPLAY:-}" ]
}
# Identify the Steam client library (size:mtime of the 32-bit steamclient.so we
# hook). A client self-update rewrites it, changing this id - the signal that a
# startup crash is a compatibility break rather than a one-off. Empty when no
# client is found yet (fresh install): callers then stay conservative.
guard_client_fp() {
	for _r in "$HOME/.steam/steam" "$HOME/.steam/debian-installation" "$HOME/.local/share/Steam"; do
		for _arch in steamrt64 ubuntu12_32; do
			_c="$_r/$_arch/steamclient.so"
			if [ -e "$_c" ]; then
				stat -c '%s:%Y' "$_c" 2>/dev/null || stat -f '%z:%m' "$_c" 2>/dev/null || printf '?'
				return 0
			fi
		done
	done
	printf ''
}
# Fingerprint the injected payload (size:mtime of each piece). Updating ANY of
# it - which the plugin does on reinstall/update - changes this, so a stuck
# safe-mode latch auto-clears once the user has updated.
guard_fingerprint() {
	for _f in "$EMBERDIR/ember-core.so" \
	          "$HOME/.local/share/CloudRedirect/cloud_redirect.so" \
	          "$HOME/.config/ember/sidecar"; do
		if [ -e "$_f" ]; then
			stat -c '%s:%Y' "$_f" 2>/dev/null || stat -f '%z:%m' "$_f" 2>/dev/null || printf '?'
		else
			printf -- '-'
		fi
		printf '|'
	done
}
GUARD_CUR_FP="$(guard_fingerprint)"
[ -n "${EMBER_GUARD_SESSION_ID:-}" ] || EMBER_GUARD_SESSION_ID="$(guard_session_id)"

# True when Steam wrote a FATAL crash minidump during the boot that started at
# epoch $2 (marker file $1), within that boot's first STARTUP_SECS. This is the
# definitive "Steam crashed before it became usable" signal. A clean kill (Game
# Mode <-> Desktop switch, shutdown) or a quick manual quit does NOT write a
# dump, so those never count as failures - which is why we use this instead of a
# bare "the session was short" heuristic.
#
# Match ONLY crash_*.dmp (fatal: segfault/abort that takes the client down).
# Steam also writes assert_*.dmp for NON-FATAL assertions (e.g. CloudRedirect's
# cloud-save path-resolution asserts in remotestoragefilesynccontext.cpp) while
# it keeps running perfectly fine, and those land in the same /tmp/dumps within
# the startup window. A bare '*.dmp' glob counted those as startup crashes and,
# coinciding with a Steam client self-update (client-changed -> latch on first
# crash), wrongly paused the hook on a healthy desktop boot.
guard_startup_crash() {
	[ -d "$EMBER_GUARD_DUMPS_DIR" ] || return 1
	case "$2" in ''|*[!0-9]*) return 1 ;; esac
	[ "$2" -gt 0 ] || return 1
	_ref="$GUARD_DIR/.crash_win_ref"
	touch -d "@$(( $2 + EMBER_GUARD_STARTUP_SECS ))" "$_ref" 2>/dev/null || { rm -f "$_ref" 2>/dev/null; return 1; }
	_hit="$(find "$EMBER_GUARD_DUMPS_DIR" -maxdepth 1 -name 'crash_*.dmp' -newer "$1" ! -newer "$_ref" 2>/dev/null | head -n1)"
	rm -f "$_ref" 2>/dev/null
	[ -n "$_hit" ]
}

# New session? Drop the recovery state and give injection a fresh try. The fail
# count goes with it: crash dumps live in /tmp, so a previous boot's outcome is
# not assessable here anyway, and counting across boots let unrelated launches
# accumulate towards a latch the user never saw happen.
GUARD_PREV_SESSION="$(cat "$GUARD_SESSION" 2>/dev/null || true)"
if [ "$EMBER_GUARD_SESSION_ID" != "$GUARD_PREV_SESSION" ]; then
	if [ -f "$GUARD_SAFE" ]; then
		guard_log "new session -> clearing the recovery latch and retrying injection"
	fi
	rm -f "$GUARD_SAFE" "$GUARD_FP" "$GUARD_COUNT" "$GUARD_LAST" "$GUARD_GFX" 2>/dev/null || true
	printf '%s' "$EMBER_GUARD_SESSION_ID" > "$GUARD_SESSION" 2>/dev/null || true
fi

# Already latched in this session? Stay vanilla until the payload changes (user
# updated) or the session ends.
if [ -f "$GUARD_SAFE" ]; then
	if [ "$(cat "$GUARD_FP" 2>/dev/null)" = "$GUARD_CUR_FP" ]; then
		guard_log "safe mode active -> launching Steam without injection"
		ember_exec_vanilla "$@"
	fi
	guard_log "payload changed since latch -> clearing safe mode, retrying injection"
	rm -f "$GUARD_SAFE" "$GUARD_FP" "$GUARD_COUNT" "$GUARD_LAST" 2>/dev/null || true
fi

# Assess the PREVIOUS boot: it failed only if Steam crashed at startup (wrote a
# minidump in its first STARTUP_SECS). A short but clean session is NOT a
# failure. (find/touch/stat failing degrade to "ok", so the guard never latches
# by accident.)
GUARD_FAILS="$(guard_read_int "$GUARD_COUNT")"
GUARD_CLIENT_CUR="$(guard_client_fp)"
guard_client_changed=0
guard_assessable=0
if [ -f "$GUARD_LAST" ]; then
	if [ "$(cat "$GUARD_GFX" 2>/dev/null)" = 0 ]; then
		guard_log "previous launch had no display -> its outcome is not assessed"
	else
		guard_assessable=1
	fi
fi
if [ "$guard_assessable" = 1 ]; then
	_then="$(stat -c %Y "$GUARD_LAST" 2>/dev/null || stat -f %m "$GUARD_LAST" 2>/dev/null || echo 0)"
	if guard_startup_crash "$GUARD_LAST" "$_then"; then
		GUARD_FAILS=$(( GUARD_FAILS + 1 ))
		# If the client that just crashed differs from the last client we saw
		# boot cleanly, a fresh client update is almost certainly the cause -
		# recover on the FIRST crash instead of making the user sit through
		# MAX_FAILS loops. A one-off crash on an UNCHANGED client keeps the
		# conservative threshold (it is far more likely transient/unrelated).
		_prev_client="$(cat "$GUARD_CLIENT_LAST" 2>/dev/null || true)"
		_good_client="$(cat "$GUARD_CLIENT_GOOD" 2>/dev/null || true)"
		if [ -n "$_good_client" ] && [ "$_prev_client" != "$_good_client" ]; then
			guard_client_changed=1
			guard_log "steamclient.so changed since last clean boot -> first startup crash treated as a compatibility break"
		fi
		guard_log "previous boot crashed at startup -> fail ${GUARD_FAILS}/${EMBER_GUARD_MAX_FAILS}"
	else
		[ "$GUARD_FAILS" -ne 0 ] && guard_log "previous boot ok (no startup crash) -> reset fail count"
		GUARD_FAILS=0
		# Remember the client that just booted cleanly as the known-good baseline.
		_prev_client="$(cat "$GUARD_CLIENT_LAST" 2>/dev/null || true)"
		[ -n "$_prev_client" ] && printf '%s' "$_prev_client" > "$GUARD_CLIENT_GOOD" 2>/dev/null || true
	fi
fi
printf '%s' "$GUARD_FAILS" > "$GUARD_COUNT" 2>/dev/null || true

if [ "$GUARD_FAILS" -ge "$EMBER_GUARD_MAX_FAILS" ] || { [ "$guard_client_changed" = 1 ] && [ "$GUARD_FAILS" -ge 1 ]; }; then
	guard_log "fail count ${GUARD_FAILS} (client_changed=${guard_client_changed}) -> latching safe mode, launching vanilla Steam"
	printf '%s' "$GUARD_CUR_FP" > "$GUARD_FP" 2>/dev/null || true
	: > "$GUARD_SAFE" 2>/dev/null || true
	# Drop the half-injected appinfo.vdf so vanilla Steam rebuilds a clean one;
	# the abandoned splice (written at preinit before the hook abort) is what
	# stalls the engine. Steam regenerates appinfo.vdf on next launch.
	for _r in "$HOME/.steam/steam" "$HOME/.steam/debian-installation" "$HOME/.local/share/Steam"; do
		if [ -f "$_r/appcache/appinfo.vdf" ]; then
			rm -f "$_r/appcache/appinfo.vdf" 2>/dev/null && guard_log "removed $_r/appcache/appinfo.vdf"
		fi
	done
	guard_notify "ember paused itself for this session because Steam kept failing to start. Steam is running normally - injection is retried the next time you reboot, or right away if you update the plugin from the ember menu."
	guard_log "recovery mode latched for this session; Steam will launch unhooked until the next session or a payload update"
	ember_exec_vanilla "$@"
fi

# Mark the start of THIS boot for the next invocation's health check, record
# whether it can produce a meaningful verdict (a launch with no display cannot),
# and record the client this boot is about to run so the next assessment can tell
# whether the client changed across a crash.
: > "$GUARD_LAST" 2>/dev/null || true
if guard_has_display; then
	printf 1 > "$GUARD_GFX" 2>/dev/null || true
else
	printf 0 > "$GUARD_GFX" 2>/dev/null || true
fi
printf '%s' "$GUARD_CLIENT_CUR" > "$GUARD_CLIENT_LAST" 2>/dev/null || true

# Signed pattern metadata preflight. The normal path is cache-only and performs
# no network I/O: when both exact-module catalogs are already valid, Steam
# launches immediately and a serialized remote revalidation runs in the
# background. A new client build has no exact cache, so only that path waits for
# the helper's bounded mirror deadline before continuing with the embedded
# patterns if the network is unavailable.
ember_pattern_steam_root() {
	for _ember_root in "$HOME/.steam/steam" \
	                  "$HOME/.steam/debian-installation" \
	                  "$HOME/.local/share/Steam"; do
		if { [ -f "$_ember_root/steamrt64/steamclient.so" ] && \
		     [ -f "$_ember_root/steamrt64/steamui.so" ]; } || \
		   { [ -f "$_ember_root/ubuntu12_32/steamclient.so" ] && \
		     [ -f "$_ember_root/ubuntu12_32/steamui.so" ]; }; then
			printf '%s' "$_ember_root"
			return 0
		fi
	done
	return 1
}
ember_refresh_patterns() {
	_ember_helper="$EMBERDIR/pattern-refresh"
	[ -x "$_ember_helper" ] || return 0
	_ember_root="$(ember_pattern_steam_root)" || return 0
	_ember_config_root="${XDG_CONFIG_HOME:-$HOME/.config}"
	case "$_ember_config_root" in
		/*) ;;
		*) _ember_config_root="$HOME/.config" ;;
	esac
	if env -u LD_AUDIT -u LD_PRELOAD -u LD_LIBRARY_PATH \
	       "$_ember_helper" --cache-only \
	       --steam-root "$_ember_root" --config-root "$_ember_config_root" \
	       >/dev/null 2>&1 < /dev/null; then
		(
			trap '' HUP
			env -u LD_AUDIT -u LD_PRELOAD -u LD_LIBRARY_PATH \
			    "$_ember_helper" \
			    --steam-root "$_ember_root" --config-root "$_ember_config_root"
		) >/dev/null 2>&1 < /dev/null &
	else
		env -u LD_AUDIT -u LD_PRELOAD -u LD_LIBRARY_PATH \
		    "$_ember_helper" \
		    --steam-root "$_ember_root" --config-root "$_ember_config_root" \
		    >/dev/null 2>&1 < /dev/null || true
	fi
}
ember_refresh_patterns

# CloudRedirect (optional): inject its 32-bit cloud-save hook via LD_PRELOAD.
# Our bundled build is CloudRedirect 2.1.5 (correct save restore via
# StripCasShaLeaf) with the steamclient.so wait extended 10s -> 120s so it
# attaches on slow-bootstrap distros (Arch/CachyOS) too. It is a plain
# LD_PRELOAD library: loading it as an LD_AUDIT auditor corrupts the client
# heap (realloc(): invalid pointer) during init, so it must NOT go in the
# LD_AUDIT list. ember stays on LD_AUDIT (ember-library-inject.so first).
# CloudRedirect's constructor self-removes itself from LD_PRELOAD so child
# processes (the game, steamwebhelper) don't inherit it.
CR_SO="$HOME/.local/share/CloudRedirect/cloud_redirect.so"
# Skip 32-bit cloud_redirect.so when running 64-bit steamrt64 (ELFCLASS64 = 02).
if [ -f "$CR_SO" ] && [ "$(od -An -tx1 -j4 -N1 "$CR_SO" 2>/dev/null | tr -d '[:space:]')" = "02" ]; then
	export LD_PRELOAD="$CR_SO${LD_PRELOAD:+:$LD_PRELOAD}"
fi

# extest (Steam Input on Wayland): SteamOS and Bazzite ship libextest, an X11
# XTEST shim that lets the Steam Controller / Steam Input drive the desktop
# cursor under Wayland (X11 has native XTEST, so it's only needed on Wayland).
# Their own desktop launchers (bazzite-steam / steam-jupiter) LD_PRELOAD it on
# Wayland. We launch the Steam binary directly — on purpose, so our LD_AUDIT and
# CloudRedirect survive (those launchers do `env LD_PRELOAD=...`, which REPLACES
# the list and would drop cloud_redirect.so) — so we replicate just this one
# preload, generically: only on Wayland and only when the lib actually exists
# (a no-op on distros that don't ship it).
if [ "${XDG_SESSION_TYPE:-}" = "wayland" ]; then
	for _ext in /usr/lib/extest/libextest.so \
	            /usr/lib64/extest/libextest.so \
	            /usr/lib/x86_64-linux-gnu/extest/libextest.so; do
		if [ -f "$_ext" ]; then
			export LD_PRELOAD="${LD_PRELOAD:+$LD_PRELOAD:}$_ext"
			break
		fi
	done
fi

# Keep the payload in step with our releases. Steam updates move the things
# ember finds by pattern matching, and a payload that no longer matches is
# what makes games stop launching. The updater is throttled, detached and
# silent: it only swaps files between launches and keeps the old ones in
# backup/. EMBER_NO_AUTOUPDATE=1 turns it off.
if [ -x "$EMBERDIR/ember-autoupdate.sh" ] && [ "${EMBER_NO_AUTOUPDATE:-0}" != 1 ]; then
	if command -v setsid >/dev/null 2>&1; then
		env -u LD_AUDIT -u LD_PRELOAD -u LD_LIBRARY_PATH \
		    setsid "$EMBERDIR/ember-autoupdate.sh" >/dev/null 2>&1 < /dev/null &
	else
		env -u LD_AUDIT -u LD_PRELOAD -u LD_LIBRARY_PATH \
		    "$EMBERDIR/ember-autoupdate.sh" >/dev/null 2>&1 < /dev/null &
	fi
fi

AUDIT="$EMBERDIR/ember-library-inject.so:$EMBERDIR/ember-core.so"

if [ -n "$EMBER_INHERITED_AUDIT" ]; then
	export LD_AUDIT="$AUDIT:$EMBER_INHERITED_AUDIT"
else
	export LD_AUDIT="$AUDIT"
fi
exec "$STEAM_BIN" "$@"
EOF

	# Explicit mode: a user umask such as 0002 must not produce a group-writable
	# launcher. steam-ember is the only entry point, but keep it tight anyway.
	chmod 0755 "$EMBERDIR/path/steam"

	log_success "Steam wrapper created at $EMBERDIR/path/steam"
	echo ""
	return 0
}

# ---------------------------------------------------------------------------
# Launch integration: `steam-ember` on PATH and the "Wired Steam" entry
# ---------------------------------------------------------------------------
# Nothing here touches the distributor's `steam` binary, its "Steam" desktop
# entry or its autostart file. $EMBERDIR/path/steam stays the only piece that
# sets LD_AUDIT; `steam-ember` is a thin exec of it, and the desktop entry names
# that command rather than an absolute path so a changed XDG_BIN_HOME still
# works.

# Older releases prepended $EMBERDIR/path to PATH from the shell rc files, which
# made a plain `steam` resolve to the wrapper. That is exactly the routing this
# model forbids, so strip those lines back out.
remove_legacy_path_entries() {
	local rc removed=0
	for rc in "$HOME/.bashrc" "$HOME/.zshrc" "$HOME/.profile"; do
		[ -f "$rc" ] || continue
		grep -q "ember/path" "$rc" 2>/dev/null || continue
		sed -i '/# ember: Add wrapper to PATH/d' "$rc"
		sed -i '\|\.local/share/ember/path|d' "$rc"
		removed=1
		log_info "Removed the legacy ember PATH entry from $(basename "$rc")"
	done
	[ "$removed" = 1 ] && log_success "PATH no longer routes steam through ember"
	return 0
}

install_steam_ember() {
	mkdir -p "$BIN_DIR" || {
		log_error "Could not create $BIN_DIR"
		return 1
	}
	local tmp="$BIN_DIR/.steam-ember.tmp.$$"
	cat > "$tmp" <<EOF
#!/bin/sh
# generated by ember's setup.sh; removed by --uninstall
exec "$EMBERDIR/path/steam" "\$@"
EOF
	if ! chmod 0755 "$tmp" || ! mv -f "$tmp" "$STEAM_EMBER"; then
		rm -f -- "$tmp"
		log_error "Could not write $STEAM_EMBER"
		return 1
	fi
	log_success "Installed $STEAM_EMBER"
	case ":$PATH:" in
		*":$BIN_DIR:"*) ;;
		*) log_warn "$BIN_DIR is not on PATH here; add it to run steam-ember by name" ;;
	esac
	return 0
}

# A second desktop entry with its own file name, so the distributor's "Steam"
# entry keeps pointing at the vanilla client and `steam://` links keep working.
install_wired_entry() {
	mkdir -p "$USER_APPS" || {
		log_error "Could not create $USER_APPS"
		return 1
	}
	cat > "$WIRED_ENTRY" <<EOF
[Desktop Entry]
$EMBER_ALT_TAG
Type=Application
Name=Wired Steam
GenericName=Game Store
Comment=Steam with ember loaded, for games ember added
Exec=steam-ember %U
Icon=steam
Terminal=false
Categories=Game;
StartupWMClass=Steam
EOF
	chmod 0644 "$WIRED_ENTRY"
	log_success "Wrote $WIRED_ENTRY"
	command -v update-desktop-database >/dev/null 2>&1 &&
		update-desktop-database "$USER_APPS" >/dev/null 2>&1 || true
	return 0
}

# Badge watcher and cloud-push units. The watcher is enabled by default because
# the store-page badge is invisible without it; the cloud timer is installed but
# left off, since a timer running `rclone sync` against a remote the user has not
# configured produces a journal entry every interval.
install_user_units() {
	if ! command -v ecu_install_units >/dev/null 2>&1; then
		return 0
	fi
	local status
	ecu_install_units; status=$?
	if [ "$status" = 0 ] || [ "$status" = 1 ]; then
		ecu_enable_ui_watch
		ecu_install_cloud_units
		log_info "Badge watcher enabled; cloud push installed but off"
		log_info "  enable cloud sync with: systemctl --user enable --now ember-cloud-push.timer"
	else
		log_warn "Could not install the ember user units; the store-page badge needs 'ember ui watch'"
	fi
	return 0
}

setup_launch_integration()
{
	log_info "Configuring the ember launch path"

	# A previous ember may have patched the distributor's own files. Undo that
	# first: this release never owns them, and a stale /usr/bin/steam shim would
	# keep making plain `steam` depend on this tree.
	remove_legacy_path_entries || true
	remove_legacy_guardian_units || true
	if ! restore_legacy_patches; then
		log_warn "A distro launcher still points at ember; see the fix above"
	fi

	local steam_bin
	steam_bin="$(detect_steam_binary)"
	if [ -z "$steam_bin" ]; then
		log_warn "Steam doesn't appear to be installed yet"
		log_warn "Install Steam first, then re-run: ./setup.sh install"
		return 0
	fi
	log_success "Found Steam binary at $steam_bin"
	if ! steam_data_bootstrapped; then
		log_info "Steam is not bootstrapped yet; Wired Steam will inject once it is"
	fi

	install_steam_ember || return 1
	install_wired_entry || return 1

	log_info "Steam's own launcher is untouched; start Wired Steam for ember"
	echo ""
	return 0
}

install_steamstub()
{
	TARGET="$1"
	HELPERSRC="./tools/steamstub-bypass"

	if [ ! -d "$HELPERSRC" ]; then
		log_warn "Helper scripts not found at $HELPERSRC — skipping Steam Stub setup"
		return 1
	fi

	log_info "Installing Steamless helper"
	mkdir -p "$TARGET/steamstub-bypass"
	cp -v "$HELPERSRC/run-steamless.sh"     "$TARGET/steamstub-bypass/"
	cp -v "$HELPERSRC/install-steamless.sh" "$TARGET/steamstub-bypass/"
	cp -v "$HELPERSRC/scan-all.sh"          "$TARGET/steamstub-bypass/"
	chmod u+x "$TARGET/steamstub-bypass/run-steamless.sh" \
	          "$TARGET/steamstub-bypass/install-steamless.sh" \
	          "$TARGET/steamstub-bypass/scan-all.sh"

	echo ""

	# Prefer the Steamless kit bundled in the release (offline, no
	# network).  Only fall back to the GitHub download if the bundle is
	# absent (e.g. a dev tree).  The download path is the historical
	# silent-failure source behind "Application load error 6".
	if [ -f "./tools/steamless-bin/Steamless.CLI.exe" ]; then
		log_info "Installing bundled Steamless kit"
		mkdir -p "$TARGET/steamless-bin"
		cp -r ./tools/steamless-bin/. "$TARGET/steamless-bin/"
	else
		log_warn "No bundled Steamless kit; fetching from upstream (needs internet)"
		bash "$TARGET/steamstub-bypass/install-steamless.sh" \
			--target "$TARGET/steamless-bin" || true
	fi

	# Verify the kit actually landed.  A missing CLI means the SteamStub
	# bypass is silently disabled at runtime and DRM-locked games fail
	# with "Application load error 6" — make that loud here.
	if [ -f "$TARGET/steamless-bin/Steamless.CLI.exe" ]; then
		log_success "Steamless ready ($TARGET/steamless-bin)"
	else
		log_warn "Steamless kit NOT installed — games with Steam DRM will"
		log_warn "fail to launch (Application load error 6). To fix later:"
		log_warn "  bash $TARGET/steamstub-bypass/install-steamless.sh --target $TARGET/steamless-bin"
	fi
	echo ""
}

install_all()
{
	print_banner

	print_section "Stopping Steam"
	kill_steam

	print_section "Installing libraries"
	install_ember

	print_section "Creating Steam wrapper"
	create_steam_wrapper

	print_section "Installing steam-ember and Wired Steam"
	setup_launch_integration

	print_section "Installing badge watcher units"
	install_user_units

	print_section "Installing Steamless helper"
	install_steamstub "$EMBERDIR"

	print_install_complete
}

# ============================================================================
# Uninstall
# ============================================================================

# Run a privileged command, honouring sudo -A when an askpass helper is set.
ember_sudo() {
	if [ -n "${SUDO_ASKPASS:-}" ] && [ -x "${SUDO_ASKPASS:-}" ]; then
		sudo -A "$@"
	else
		sudo "$@"
	fi
}

# Put one of the distributor's own files back the way it shipped. Older ember
# patched these in place and kept either a mirrored copy under
# $EMBERDIR/backup/<absolute path> or a sibling .ember-backup/.pre-ember copy.
# $2 is empty for files we may write directly, or "ember_sudo" for system paths.
restore_legacy_file() {
	local f="$1" privileged="${2:-}"
	local mirrored="$DC_BACKUP_ROOT/${f#/}" backup="" candidate

	[ -e "$f" ] || return 0
	has_ember_marker "$f" || return 0

	for candidate in "$mirrored" "$f.ember-backup" "$f.ember-bak" "$f.pre-ember"; do
		if [ -f "$candidate" ]; then
			backup="$candidate"
			break
		fi
	done

	if [ -z "$backup" ]; then
		log_warn "$f still points at ember and no backup was found; removing it"
		if $privileged rm -f -- "$f"; then
			log_success "Removed $f"
			return 0
		fi
		log_warn "  fix:  sudo rm $f"
		return 1
	fi

	log_info "Restoring $f from $backup"
	if ! $privileged cp -- "$backup" "$f"; then
		log_warn "$f still points at ember"
		log_warn "  fix:  sudo cp $backup $f"
		return 1
	fi
	[ "$backup" = "$mirrored" ] || $privileged rm -f -- "$backup" 2>/dev/null || true
	log_success "Restored $f"
	return 0
}

# Older releases replaced the distro launcher (/usr/bin/steam or
# /usr/games/steam) with a shim and kept the original under
# $EMBERDIR/system-launcher-backup. ember must never own that path, so put the
# original back; if that is impossible, say exactly what is left and how to fix
# it. Returns 0 only when nothing points at ember any more.
restore_legacy_launcher() {
	local launcher found=0 failed=0 backup candidate
	for launcher in /usr/bin/steam /usr/games/steam /usr/local/bin/steam; do
		[ -e "$launcher" ] || [ -L "$launcher" ] || continue
		if [ ! -L "$launcher" ] && ! head -5 "$launcher" 2>/dev/null | grep -q "ember"; then
			continue
		fi
		found=1
		backup=""
		for candidate in \
			"$LEGACY_LAUNCHER_BACKUP/${launcher#/}.orig" \
			"$LEGACY_LAUNCHER_BACKUP/${launcher#/}" \
			"$launcher.ember-backup" \
			"$launcher.orig"; do
			if [ -f "$candidate" ]; then
				backup="$candidate"
				break
			fi
		done
		if [ -z "$backup" ]; then
			failed=1
			log_warn "$launcher still points at ember and no original was found"
			log_warn "  fix:  sudo rm $launcher, then reinstall Steam's launcher package"
			continue
		fi
		if [ -w "$launcher" ] && cp -p -- "$backup" "$launcher"; then
			log_success "Restored $launcher"
		elif command -v sudo >/dev/null 2>&1 && \
		     ember_sudo cp -p -- "$backup" "$launcher"; then
			log_success "Restored $launcher (with administrator access)"
		else
			failed=1
			log_warn "$launcher still points at ember"
			log_warn "  fix:  sudo cp $backup $launcher"
			continue
		fi
	done
	[ "$found" = 1 ] || return 0
	if [ "$failed" = 1 ]; then
		log_warn "ember could not restore every distro launcher; see the commands above"
		log_warn "Keeping $LEGACY_LAUNCHER_BACKUP so a retry can finish the job"
		return 1
	fi
	log_success "Distro Steam launchers no longer point at ember"
	return 0
}

# Restore everything an older ember patched: the distributor's desktop entries,
# its autostart file and the distro launcher. Non-zero means a launcher still
# points at ember; its originals are then kept so a retry can finish the job.
restore_legacy_patches() {
	local rc=0
	restore_legacy_file "$LEGACY_USER_DESKTOP" || rc=1
	restore_legacy_file "$LEGACY_USER_AUTOSTART" || rc=1
	if [ -f "$SYS_DESKTOP" ] && has_ember_marker "$SYS_DESKTOP"; then
		if [ -w "$SYS_DESKTOP" ] || is_immutable_distro || \
		   ! command -v sudo >/dev/null 2>&1; then
			restore_legacy_file "$SYS_DESKTOP" || rc=1
		else
			restore_legacy_file "$SYS_DESKTOP" ember_sudo || rc=1
		fi
	fi
	restore_legacy_launcher || rc=1
	return "$rc"
}

# Desktop-guardian units shipped by older releases exist only to run the
# coverage CLI this version no longer installs, so they would fail every tick.
# The names are ember-specific and were always written by ember.
remove_legacy_guardian_units() {
	local unit_dir="${XDG_CONFIG_HOME:-$HOME/.config}/systemd/user"
	local unit removed=0
	for unit in ember-desktop-guardian.service \
	            ember-desktop-guardian.timer \
	            ember-desktop-guardian.path; do
		[ -f "$unit_dir/$unit" ] || continue
		if command -v systemctl >/dev/null 2>&1; then
			systemctl --user disable --now "$unit" >/dev/null 2>&1 || true
		fi
		rm -f -- "$unit_dir/$unit" && removed=1
	done
	if [ "$removed" = 1 ]; then
		if command -v systemctl >/dev/null 2>&1; then
			systemctl --user daemon-reload >/dev/null 2>&1 || true
			systemctl --user reset-failed >/dev/null 2>&1 || true
		fi
		log_success "Removed the legacy desktop-guardian units"
	fi
	return 0
}

uninstall()
{
	print_banner
	print_section "Uninstalling ember"
	local launcher_restore_complete=1

	kill_steam

	remove_legacy_path_entries || true

	# Badge watcher and cloud-push timer, sentinel-scoped.
	if command -v ecu_remove_units >/dev/null 2>&1; then
		ecu_remove_units || true
	fi
	remove_legacy_guardian_units || true

	# The tarball launcher and its own desktop entry.
	if [ -e "$STEAM_EMBER" ] || [ -L "$STEAM_EMBER" ]; then
		rm -f -- "$STEAM_EMBER" && log_success "Removed $STEAM_EMBER"
	fi
	if [ -e "$WIRED_ENTRY" ] || [ -L "$WIRED_ENTRY" ]; then
		rm -f -- "$WIRED_ENTRY" && log_success "Removed $WIRED_ENTRY"
	fi

	# Undo the patches older releases left on the distributor's own files.
	if restore_legacy_patches; then
		launcher_restore_complete=1
	else
		launcher_restore_complete=0
	fi

	# Coverage policy state from older releases lives outside EMBERDIR.
	local state_home="${XDG_STATE_HOME:-$HOME/.local/state}"
	case "$state_home" in
		/*) : ;;
		*) state_home="$HOME/.local/state" ;;
	esac
	rm -f -- "$state_home/ember/coverage.policy" 2>/dev/null || true
	rm -f -- "$EMBERDIR/coverage-policy.effective" 2>/dev/null || true

	if command -v update-desktop-database >/dev/null 2>&1; then
		update-desktop-database "$USER_APPS" >/dev/null 2>&1 || true
		if ! is_immutable_distro && command -v sudo >/dev/null 2>&1; then
			ember_sudo update-desktop-database "/usr/share/applications" >/dev/null 2>&1 || true
		fi
	fi

	if [ -d "$EMBERDIR" ]; then
		# Drop the payload either way: nothing points at the wrapper once
		# steam-ember and the legacy shims are gone. Keep only the launcher
		# originals while a restore is still pending, so a retry can finish.
		find "$EMBERDIR" -mindepth 1 -maxdepth 1 \
			! -name system-launcher-backup -exec rm -rf -- {} + 2>/dev/null || true
		if [ "$launcher_restore_complete" = 0 ]; then
			log_info "Re-run this uninstaller once $LEGACY_LAUNCHER_BACKUP is no longer needed"
		else
			rm -rf -- "$EMBERDIR" 2>/dev/null || true
			[ -d "$EMBERDIR" ] || log_success "Removed $EMBERDIR"
		fi
	fi

	print_uninstall_complete
}
# ============================================================================
# Entry point
# ============================================================================

if [ "${EMBER_SETUP_LIB_ONLY:-0}" = 1 ]; then
	if [[ "${BASH_SOURCE[0]}" != "$0" ]]; then
		return 0
	fi
	exit 0
fi

if [[ $# -lt 1 ]]; then
	print_banner
	echo -e "${BOLD}Usage:${NC}  $0 ${GREEN}install${NC} | ${GREEN}uninstall${NC}"
	echo ""
	echo "  install    build the ember launcher, install steam-ember and the"
	echo "             \"Wired Steam\" desktop entry, and enable the badge watcher."
	echo "             Steam's own launcher is left untouched."
	echo "  uninstall  remove steam-ember, Wired Steam and the payload, restore"
	echo "             the distributor's files older releases had patched."
	echo ""
	exit 0
fi

# Accept the --install/--uninstall spelling too; both have been documented.
cmd="${1#--}"

if [ "$cmd" == "install" ]; then
	install_all
elif [ "$cmd" == "uninstall" ]; then
	uninstall
else
	log_error "Unknown command: $1"
	echo -e "${BOLD}Usage:${NC}  $0 ${GREEN}install${NC} | ${GREEN}uninstall${NC}"
	exit 1
fi
