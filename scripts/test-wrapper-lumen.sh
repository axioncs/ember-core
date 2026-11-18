#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-only
#
# test-wrapper-lumen.sh - the wrapper's sidecar decisions.
#
# Two things used to be invisible: Lumen's output went to /dev/null, and the
# wrapper happily started a second sidecar next to the user service (Lumen's own
# lock then dropped one of them). This drives the GENERATED wrapper with a fake
# Lumen, a fake systemctl and a fake Steam, and checks which of the two owns the
# sidecar, that the log is written, and that the payload updater runs (or does
# not, when EMBER_NO_AUTOUPDATE=1).

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SETUP="$REPO_ROOT/setup.sh"

PASS=0; FAIL=0
ok()  { echo "  ok   - $*"; PASS=$((PASS+1)); }
bad() { echo "  FAIL - $*" >&2; FAIL=$((FAIL+1)); }

FN="$(awk '
	/^create_steam_wrapper\(\)/ { f=1 }
	f && /<< '\''EOF'\''/ { inhd=1 }
	f { print }
	f && inhd && /^EOF$/ { inhd=0; next }
	f && !inhd && /^}/ { exit }
' "$SETUP")"
[ -n "$FN" ] || { echo "could not extract create_steam_wrapper" >&2; exit 1; }

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
export HOME="$WORK/home"
export XDG_STATE_HOME="$HOME/.local/state"
export XDG_CONFIG_HOME="$HOME/.config"
mkdir -p "$HOME" "$WORK/bin"
# The wrapper only trusts the user service when it can actually reach it, so the
# harness needs a session bus socket to exist.
export XDG_RUNTIME_DIR="$WORK/run"
mkdir -p "$XDG_RUNTIME_DIR"
python3 - "$XDG_RUNTIME_DIR/bus" <<'PY'
import socket, sys
s = socket.socket(socket.AF_UNIX)
s.bind(sys.argv[1])
PY
EMBERDIR="$HOME/.local/share/ember"
LUMEN_DIR="$HOME/.local/share/Lumen"
UNIT="$XDG_CONFIG_HOME/systemd/user/lumen.service"

mkdir -p "$EMBERDIR" "$LUMEN_DIR"
printf 'so'  >"$EMBERDIR/ember-core.so"
printf 'inj' >"$EMBERDIR/ember-library-inject.so"

# Fake Lumen: records that it was started, then exits.
LUMEN_RAN="$WORK/lumen-ran"
cat >"$LUMEN_DIR/lumen" <<EOF
#!/bin/sh
echo "lumen ran" >> "$LUMEN_RAN"
EOF
chmod 755 "$LUMEN_DIR/lumen"

# Fake updater, off unless a case turns it on.
UPDATER_RAN="$WORK/updater-ran"
cat >"$EMBERDIR/ember-autoupdate.sh" <<EOF
#!/bin/sh
echo "updater ran" >> "$UPDATER_RAN"
EOF
chmod 755 "$EMBERDIR/ember-autoupdate.sh"

# Fake systemctl: is-enabled answers from a file so cases can flip it.
SYSTEMCTL_CALLS="$WORK/systemctl.calls"
cat >"$WORK/bin/systemctl" <<'EOF'
#!/bin/sh
printf '%s\n' "$*" >>"$SYSTEMCTL_CALLS"
case "$*" in
	*is-enabled*) [ -f "$SYSTEMCTL_ENABLED" ] && exit 0 || exit 1 ;;
	*is-active*)  exit 1 ;;
	*) exit 0 ;;
esac
EOF
chmod 755 "$WORK/bin/systemctl"
export SYSTEMCTL_CALLS SYSTEMCTL_ENABLED="$WORK/systemctl-enabled"
export PATH="$WORK/bin:$PATH"

# Fake Steam, so the wrapper's real exec target is harmless.
cat >"$WORK/steam" <<'EOF'
#!/bin/sh
exit 0
EOF
chmod 755 "$WORK/steam"
export EMBER_STEAM_BIN="$WORK/steam"
export DISPLAY="${DISPLAY:-:0}"
export EMBER_GUARD_SESSION_ID=harness-session
export EMBER_GUARD_DUMPS_DIR="$WORK/dumps"
mkdir -p "$EMBER_GUARD_DUMPS_DIR"

( umask 022; log_info() { :; }; log_success() { :; }; log_warn() { :; }; eval "$FN"; EMBERDIR="$EMBERDIR" create_steam_wrapper )
WRAP="$EMBERDIR/path/steam"
[ -x "$WRAP" ] || { echo "wrapper not generated" >&2; exit 1; }

reset() {
	rm -rf "$XDG_CONFIG_HOME/systemd" "$LUMEN_DIR/lumen.log" "$LUMEN_RAN" "$UPDATER_RAN" "$SYSTEMCTL_CALLS" "$SYSTEMCTL_ENABLED"
	mkdir -p "$XDG_CONFIG_HOME/systemd/user"
}
run_wrapper() {
	sh "$WRAP" >/dev/null 2>&1
	sleep 0.4  # the sidecar and updater are started detached
}

echo "== no service: wrapper starts Lumen and logs it"
reset
run_wrapper
[ -f "$LUMEN_RAN" ] && ok "Lumen started by the wrapper" || bad "Lumen was not started"
grep -q 'starting Lumen' "$LUMEN_DIR/lumen.log" 2>/dev/null \
	&& ok "launch recorded in lumen.log" || bad "nothing written to lumen.log"

echo "== enabled service: wrapper leaves the sidecar to systemd"
reset
: >"$UNIT"
touch "$SYSTEMCTL_ENABLED"
run_wrapper
grep -q 'user start lumen.service' "$SYSTEMCTL_CALLS" 2>/dev/null \
	&& ok "systemd asked to start the unit" || bad "systemd was not asked to start the unit"
[ -f "$LUMEN_RAN" ] && bad "wrapper started a second sidecar" || ok "no second sidecar"

echo "== service disabled again: wrapper takes over"
reset
: >"$UNIT"
run_wrapper
[ -f "$LUMEN_RAN" ] && ok "Lumen started by the wrapper" || bad "Lumen was not started"

echo "== payload updater runs in the background"
reset
run_wrapper
[ -f "$UPDATER_RAN" ] && ok "updater ran" || bad "updater did not run"

echo "== opt-out keeps the updater off"
reset
EMBER_NO_AUTOUPDATE=1 run_wrapper
[ -f "$UPDATER_RAN" ] && bad "updater ran despite EMBER_NO_AUTOUPDATE=1" || ok "updater skipped"

echo
if [ "$FAIL" -ne 0 ]; then
	echo "test-wrapper-lumen: $PASS passed, $FAIL failed" >&2
	exit 1
fi
echo "test-wrapper-lumen: all $PASS passed"
