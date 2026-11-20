#!/usr/bin/env bash
# test-ember-units.sh — drive the real shipped unit library against a sandbox
# HOME and assert the lifecycle: install, idempotency, foreign-unit safety,
# removal, and that systemd accepts the rendered units.
#
# The badge watcher and the cloud-push timer are the difference between "ember
# works when you run a command" and "ember works". Both are systemd user units,
# so a regression here is invisible until a user reports the feature missing —
# which is exactly how the badge stayed dark for weeks.

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
LIB="$REPO_ROOT/tools/ember-units.lib.sh"

PASS=0; FAIL=0
ok()  { echo "  ok   - $*"; PASS=$((PASS+1)); }
bad() { echo "  FAIL - $*" >&2; FAIL=$((FAIL+1)); }

[ -f "$LIB" ] || { echo "unit library not found: $LIB" >&2; exit 1; }

WORK="$(mktemp -d)"
trap 'chmod -R u+w "$WORK" 2>/dev/null; rm -rf "$WORK"' EXIT

export HOME="$WORK/home"
export XDG_CONFIG_HOME="$HOME/.config"
export ECU_UNIT_DIR="$HOME/.config/systemd/user"
# Never talk to the real manager: a no-op that always succeeds.
export ECU_SYSTEMCTL=true

mkdir -p "$HOME/.local/share/ember" "$HOME/.local/share/Steam" "$HOME/.config"
printf '#!/bin/sh\nexit 0\n' > "$HOME/.local/share/ember/ember"
chmod +x "$HOME/.local/share/ember/ember"

# shellcheck source=/dev/null
. "$LIB"

echo "== test-ember-units =="

# ── the CLI and Steam root must be discovered, not assumed ──────────────────
cli="$(ecu_cli_path 2>/dev/null || true)"
[ -n "$cli" ] && ok "ember CLI resolved to $cli" \
              || bad "ember CLI was not found; units would have an empty ExecStart"

root="$(ecu_steam_root 2>/dev/null || true)"
[ -n "$root" ] && ok "Steam root resolved to $root" \
               || bad "Steam root was not found"

# ── install ─────────────────────────────────────────────────────────────────
ecu_install_units >/dev/null 2>&1
rc=$?
[ "$rc" = "0" ] && ok "install writes the units" \
                || bad "install returned $rc, expected 0"

for u in "$ECU_UI_SERVICE" "$ECU_CLOUD_SERVICE" "$ECU_CLOUD_TIMER"; do
	[ -f "$ECU_UNIT_DIR/$u" ] && ok "$u exists" || bad "$u was not written"
done

# ── every unit carries the ownership sentinel, or removal is unsafe ─────────
for u in "$ECU_UI_SERVICE" "$ECU_CLOUD_SERVICE" "$ECU_CLOUD_TIMER"; do
	if grep -qxF "$ECU_UNIT_SENTINEL" "$ECU_UNIT_DIR/$u" 2>/dev/null; then
		ok "$u carries the ember sentinel"
	else
		bad "$u has no sentinel; ember would later refuse to remove it"
	fi
done

# ── idempotent: a second install must not churn the files ───────────────────
ecu_install_units >/dev/null 2>&1
rc=$?
[ "$rc" = "1" ] && ok "second install is a no-op" \
                || bad "second install returned $rc, expected 1"

# ── ExecStart must carry an absolute path, or systemd cannot start it ───────
if grep -q "^ExecStart=/" "$ECU_UNIT_DIR/$ECU_UI_SERVICE" 2>/dev/null; then
	ok "ui-watch ExecStart uses an absolute path"
else
	bad "ui-watch ExecStart is not absolute"
fi
if grep -q -- "ui watch" "$ECU_UNIT_DIR/$ECU_UI_SERVICE" 2>/dev/null; then
	ok "ui-watch actually runs 'ember ui watch'"
else
	bad "ui-watch does not invoke 'ember ui watch'"
fi
if grep -q -- "cloud push" "$ECU_UNIT_DIR/$ECU_CLOUD_SERVICE" 2>/dev/null; then
	ok "cloud-push actually runs 'ember cloud push'"
else
	bad "cloud-push does not invoke 'ember cloud push'"
fi

# ── the timer must be persistent, or a machine that was off loses a save ────
grep -q '^Persistent=true' "$ECU_UNIT_DIR/$ECU_CLOUD_TIMER" 2>/dev/null \
	&& ok "cloud-push timer is persistent" \
	|| bad "cloud-push timer is not persistent"

# ── a foreign unit of the same name must survive untouched ──────────────────
printf 'my own unit\n' > "$ECU_UNIT_DIR/$ECU_UI_SERVICE"
ecu_install_units >/dev/null 2>&1
rc=$?
[ "$rc" = "2" ] && ok "install refuses to overwrite a foreign unit" \
                || bad "install returned $rc over a foreign unit, expected 2"
[ "$(cat "$ECU_UNIT_DIR/$ECU_UI_SERVICE")" = "my own unit" ] \
	&& ok "the foreign unit is byte-identical afterwards" \
	|| bad "the foreign unit was modified"

# ── systemd itself must accept what we rendered ─────────────────────────────
if command -v systemd-analyze >/dev/null 2>&1; then
	rm -f "$ECU_UNIT_DIR/$ECU_UI_SERVICE"
	ecu_install_units >/dev/null 2>&1
	if systemd-analyze verify \
		"$ECU_UNIT_DIR/$ECU_UI_SERVICE" \
		"$ECU_UNIT_DIR/$ECU_CLOUD_SERVICE" \
		"$ECU_UNIT_DIR/$ECU_CLOUD_TIMER" >/dev/null 2>&1; then
		ok "systemd-analyze verify accepts all three units"
	else
		bad "systemd-analyze verify rejected the rendered units"
	fi
else
	echo "  skip - systemd-analyze not available"
fi

# ── removal clears ours and leaves the foreign one ──────────────────────────
chmod u+w "$ECU_UNIT_DIR/$ECU_UI_SERVICE" 2>/dev/null || true
printf '%s\n' "$ECU_UNIT_SENTINEL" > "$ECU_UNIT_DIR/$ECU_UI_SERVICE"
ecu_remove_units >/dev/null 2>&1
rc=$?
[ "$rc" = "0" ] && ok "remove succeeds" || bad "remove returned $rc, expected 0"

left=0
for u in "$ECU_UI_SERVICE" "$ECU_CLOUD_SERVICE" "$ECU_CLOUD_TIMER"; do
	[ -e "$ECU_UNIT_DIR/$u" ] && { bad "$u survived removal"; left=1; }
done
[ "$left" = "0" ] && ok "all ember units are gone after removal"

ecu_remove_units >/dev/null 2>&1
rc=$?
[ "$rc" = "1" ] && ok "remove with nothing owned is a no-op" \
                || bad "remove returned $rc when nothing was owned, expected 1"

echo "== total: $PASS passed, $FAIL failed =="
[ "$FAIL" -eq 0 ]