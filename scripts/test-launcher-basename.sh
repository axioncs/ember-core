#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-only
#
# test-launcher-basename.sh - the ember launcher must never execute a Steam
# launcher under a name it does not recognise.
#
# Why this exists
# ---------------
# On Fedora/Nobara (and any distro shipping Valve's launcher directly)
# /usr/bin/steam is a symlink to /usr/lib/steam/bin_steam.sh, and that script
# derives its whole identity from its own argv[0]:
#
#     STEAMPACKAGE="${0##*/}"
#     if [ "$STEAMPACKAGE" = bin_steam.sh ]; then STEAMPACKAGE=steam; fi
#     case "$STEAMPACKAGE" in
#         steam) ... ;; steambeta) ... ;;
#         *) log "Unknown Steam package '$STEAMPACKAGE'"; exit 1 ;;
#     esac
#
# Executing a copy kept under any other name (a restored *.orig, a pinned
# EMBER_STEAM_BIN) aborts the launch with "Unknown Steam package 'steam.orig'"
# BEFORE Steam opens its logger, so the user sees Steam simply not starting and
# no log is produced anywhere. The generated wrapper must therefore normalise
# ANY name-unsafe launcher it is told to run.
set -uo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SETUP="$REPO_ROOT/setup.sh"
TMP="$(mktemp -d)"
trap 'chmod -R u+w "$TMP" 2>/dev/null || true; rm -rf "$TMP"' EXIT
PASS=0; FAIL=0
check() {
	if [ "$2" = "$3" ]; then
		printf 'ok   - %s\n' "$1"; PASS=$((PASS+1))
	else
		printf 'FAIL - %s (want=[%s] got=[%s])\n' "$1" "$2" "$3" >&2; FAIL=$((FAIL+1))
	fi
}

# A stand-in for Valve's bin_steam.sh: it refuses to run under any other name.
# When VL_LOG is set it also records the name it ran under and whether the
# injected loader environment survived.
write_valve_launcher() {
	cat > "$1" <<'VL'
#!/bin/sh
case "${0##*/}" in
	steam|steambeta|bin_steam.sh) ;;
	*) printf "bin_steam.sh[$$]: Unknown Steam package '%s'\n" "${0##*/}" >&2; exit 1 ;;
esac
if [ -n "${VL_LOG:-}" ]; then
	printf 'argv0=%s audit=%s\n' "${0##*/}" "${LD_AUDIT:+set}" >> "$VL_LOG"
fi
printf 'VALVE-LAUNCHER %s\n' "$*"
VL
	chmod 0755 "$1"
}

# Generated from setup.sh (not a copy), exactly like test-wrapper-guard.sh.
FN="$(awk '
	/^create_steam_wrapper\(\)/ { f=1 }
	f && /<< '\''EOF'\''/ { inhd=1 }
	f { print }
	f && inhd && /^EOF$/ { inhd=0; next }
	f && !inhd && /^}/ { exit }
' "$SETUP")"
[ -n "$FN" ] || { echo "could not extract create_steam_wrapper from setup.sh" >&2; exit 1; }

HOME="$TMP/home"
export HOME
export XDG_STATE_HOME="$HOME/.local/state"
W_EMBERDIR="$HOME/.local/share/ember"
W_STEAM_ROOT="$HOME/steam-root"
UNSAFE="$W_EMBERDIR/launcher-backup/usr/bin/steam.orig"
mkdir -p "$W_EMBERDIR" "$HOME/.steam" "$W_STEAM_ROOT" "$(dirname "$UNSAFE")"
printf 'so'  > "$W_EMBERDIR/ember-core.so"
printf 'inj' > "$W_EMBERDIR/ember-library-inject.so"
printf '#!/bin/sh\nprintf "DATA-STEAMSH %%s\\n" "$*"\n' > "$W_STEAM_ROOT/steam.sh"
chmod 0755 "$W_STEAM_ROOT/steam.sh"
ln -s "$W_STEAM_ROOT" "$HOME/.steam/steam"
write_valve_launcher "$UNSAFE"
( log_info() { :; }; log_success() { :; }; log_warn() { :; }; eval "$FN"; EMBERDIR="$W_EMBERDIR" create_steam_wrapper ) >/dev/null
WRAP="$W_EMBERDIR/path/steam"
[ -x "$WRAP" ] || { echo "wrapper was not generated at $WRAP" >&2; exit 1; }

VL_LOG="$HOME/valve.log"
export VL_LOG
: > "$VL_LOG"
out="$(EMBER_STEAM_BIN="$UNSAFE" sh "$WRAP" -silent 2>&1)"
check "wrapper normalises a name-unsafe launcher" "VALVE-LAUNCHER" \
	"$(printf '%s\n' "$out" | grep -o 'VALVE-LAUNCHER' | head -n1)"
check "wrapper preserves the launcher arguments while normalising" "VALVE-LAUNCHER -silent" \
	"$(printf '%s\n' "$out" | grep 'VALVE-LAUNCHER' | head -n1)"
check "normalised launch reaches the launcher as 'steam'" "argv0=steam" \
	"$(grep -o 'argv0=[^ ]*' "$VL_LOG" | head -n1)"
check "wrapper keeps injecting while normalising" "audit=set" \
	"$(grep -o 'audit=[^ ]*' "$VL_LOG" | head -n1)"

# A name-safe target must be handed through untouched (no needless aliasing).
SAFE_DIR="$HOME/safe-launcher"
mkdir -p "$SAFE_DIR"
write_valve_launcher "$SAFE_DIR/steam"
out_safe="$(EMBER_STEAM_BIN="$SAFE_DIR/steam" sh "$WRAP" -foo 2>&1)"
check "wrapper runs a name-safe launcher directly" "VALVE-LAUNCHER -foo" \
	"$(printf '%s\n' "$out_safe" | grep 'VALVE-LAUNCHER' | head -n1)"

# The launcher the caller pinned is never rewritten or removed.
check "the pinned launcher is left in place" "yes" \
	"$([ -f "$UNSAFE" ] && echo yes || echo no)"

printf '\n%s passed, %s failed\n' "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
