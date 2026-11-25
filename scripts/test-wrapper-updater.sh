#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-only
#
# test-wrapper-updater.sh - the wrapper's payload-updater decision.
#
# The updater used to run silently from the wrapper with its output discarded,
# so a broken updater looked identical to a working one. This drives the
# GENERATED wrapper (not a copy) with a fake updater and a fake Steam and checks
# that it is launched detached, and that EMBER_NO_AUTOUPDATE=1 keeps it off.

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
export XDG_RUNTIME_DIR="$WORK/run"
mkdir -p "$XDG_RUNTIME_DIR"

EMBERDIR="$HOME/.local/share/ember"
mkdir -p "$EMBERDIR"
printf 'so'  >"$EMBERDIR/ember-core.so"
printf 'inj' >"$EMBERDIR/ember-library-inject.so"

# Fake updater, off unless a case turns it on.
UPDATER_RAN="$WORK/updater-ran"
cat >"$EMBERDIR/ember-autoupdate.sh" <<EOF
#!/bin/sh
echo "updater ran" >> "$UPDATER_RAN"
EOF
chmod 755 "$EMBERDIR/ember-autoupdate.sh"

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

reset() { rm -f "$UPDATER_RAN"; }
run_wrapper() {
	sh "$WRAP" >/dev/null 2>&1
	sleep 0.4  # the updater is started detached
}

echo "== payload updater runs in the background"
reset
run_wrapper
[ -f "$UPDATER_RAN" ] && ok "updater ran" || bad "updater did not run"

echo "== EMBER_NO_AUTOUPDATE=1 keeps the updater off"
reset
EMBER_NO_AUTOUPDATE=1 run_wrapper
[ -f "$UPDATER_RAN" ] && bad "updater ran despite EMBER_NO_AUTOUPDATE=1" || ok "updater skipped"

echo
if [ "$FAIL" -ne 0 ]; then
	echo "test-wrapper-updater: $PASS passed, $FAIL failed" >&2
	exit 1
fi
echo "test-wrapper-updater: all $PASS passed"
