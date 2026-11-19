#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-only
#
# test-steamstub-container.sh — prove the helper survives the environment
# ember actually forks it into.
#
# ember calls run-steamless.sh from inside the injected library, which lives
# in the Steam client. On Linux that client runs inside the pressure-vessel
# container, so the helper inherits PATH=/usr/bin:/bin — a minimal coreutils
# set with NO python3 and NO xxd. A helper that assumes a desktop PATH dies
# with rc=127 ("command not found") before it ever examines the executable,
# and SteamStub DRM is silently left in place: the game then fails with
# "Application load error 6".
#
# A second trap lives one step later: the Steamless kit ships inside the Nix
# store, which is read-only. The script used to `cp` Steamless.API.dll beside
# the CLI, so on a NixOS install the copy failed and the run aborted even
# with detection fixed.
#
# This test runs the real shipped helper twice, under a PATH that contains
# coreutils but deliberately no python3/xxd:
#   1. detection only (empty STEAMLESS_HOME) -> must not exit 127
#   2. a read-only STEAMLESS_HOME with a Plugins/ layout -> must stage a
#      writable copy instead of writing into the read-only dir
#
# A stub wine records its argv and emits the .unpacked.exe the script expects,
# so the whole flow runs without touching a real prefix.

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
HELPER="$REPO_ROOT/tools/steamstub-bypass/run-steamless.sh"

PASS=0; FAIL=0
ok()  { echo "  ok   - $*"; PASS=$((PASS+1)); }
bad() { echo "  FAIL - $*" >&2; FAIL=$((FAIL+1)); }

[ -f "$HELPER" ] || { echo "helper not found: $HELPER" >&2; exit 1; }

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# ── a PATH with coreutils but no python3 / xxd ──────────────────────────────
# Prefer the coreutils the system already ships; fall back to /usr/bin minus
# nothing (the assertions below still fail loudly if python3 is present).
CU="$(dirname "$(command -v od)")"
[ -x "$CU/od" ] || { echo "cannot locate coreutils (od) to build the test PATH" >&2; exit 1; }
SED_DIR="$(dirname "$(command -v sed)")"

STUB_BIN="$WORK/bin"
mkdir -p "$STUB_BIN"

# Seed a prefix so the wineboot step is skipped and wine itself is exercised.
FAKE_HOME="$WORK/home"
mkdir -p "$FAKE_HOME/.local/share/ember/steamless-prefix"
: > "$FAKE_HOME/.local/share/ember/steamless-prefix/system.reg"

STUB_LOG="$WORK/wine.log"
: > "$STUB_LOG"

cat > "$STUB_BIN/wine" <<'WR'
#!/run/current-system/sw/bin/bash
echo "WINE-CALLED: $*" >> "$STUB_LOG"
# The helper cd's into the Steamless home, so an absolute output path is
# required: a relative one lands inside the stage and the script's own
# existence check never sees it. The path arrives Windows-style
# (Z:\home\...\game.exe), so flip the separators and drop the drive letter.
for a in "$@"; do
  case "$a" in
    *.exe)
      # Only unpack the target, never Steamless.CLI.exe itself.
      case "$a" in
        *Steamless.CLI.exe) continue ;;
      esac
      u="${a#Z:}"
      u="${u//\\//}"
      printf 'FAKE-UNPACKED' > "$u.unpacked.exe"
      ;;
  esac
done
exit 0
WR
cat > "$STUB_BIN/wineserver" <<'WS'
#!/run/current-system/sw/bin/bash
exit 0
WS
chmod +x "$STUB_BIN/wine" "$STUB_BIN/wineserver"

# A v2-flagged executable: 'VLV\0' at 0x40 over a bare MZ header.
make_v2() {
    local out="$1"
    dd if=/dev/zero of="$out" bs=1 count=512 status=none
    printf 'MZ' | dd of="$out" bs=1 seek=0 conv=notrunc status=none
    printf 'VLV\0' | dd of="$out" bs=1 seek=64 conv=notrunc status=none
}
make_v2 "$WORK/v2.exe"

STUB_PATH="$STUB_BIN:$CU:$SED_DIR"

echo "== test-steamstub-container =="

# 1. the test PATH must really lack python3, or this proves nothing.
if env -i PATH="$STUB_PATH" /run/current-system/sw/bin/bash -c 'command -v python3' >/dev/null 2>&1; then
    bad "test PATH still resolves python3; cannot prove the container case"
else
    ok "test PATH has no python3 (matches the pressure-vessel container)"
fi

# 2. detection must survive that PATH. An empty STEAMLESS_HOME makes a
#    detected stub die with rc=4 (CLI not found) right after detection;
#    rc=127 would mean a tool was missing.
rc="$(env -i HOME="$FAKE_HOME" PATH="$STUB_PATH" \
      STEAMLESS_HOME="$WORK/no-such-home" QUIET=1 \
      /run/current-system/sw/bin/bash "$HELPER" "$WORK/v2.exe" >/dev/null 2>"$WORK/err1.log"; echo $?)"
[ "$rc" = "4" ] && ok "detection runs without python3 (rc=4, CLI-not-found as expected)" \
                 || bad "detection expected rc=4 under a python3-free PATH, got $rc"

# 3. a read-only Steamless home (the Nix store case) must be staged, not
#    written into. Build the Plugins/-only layout the store ships.
RO_HOME="$WORK/ro-steamless-bin"
mkdir -p "$RO_HOME/Plugins"
printf 'FAKE-CLI'   > "$RO_HOME/Steamless.CLI.exe"
printf 'FAKE-CONFIG' > "$RO_HOME/Steamless.CLI.exe.config"
printf 'FAKE-API'   > "$RO_HOME/Plugins/Steamless.API.dll"
chmod -R a-w "$RO_HOME"
chmod a-w "$RO_HOME"

STAGE="$WORK/stage"
mkdir -p "$STAGE"

set +e
env -i HOME="$FAKE_HOME" PATH="$STUB_PATH" \
    STEAMLESS_HOME="$RO_HOME" EMBER_STEAMLESS_STAGE="$STAGE" \
    STUB_LOG="$STUB_LOG" QUIET=1 \
    /run/current-system/sw/bin/bash "$HELPER" "$WORK/v2.exe" \
    >/dev/null 2>"$WORK/err2.log"
rc=$?
set -e 2>/dev/null || true

if [ "$rc" = "0" ]; then
    ok "read-only Steamless home staged and run completed (rc=0)"
else
    bad "read-only Steamless home expected rc=0, got $rc"
fi

[ -f "$STAGE/Steamless.CLI.exe" ] \
    && ok "writable stage received the Steamless kit" \
    || bad "writable stage is missing Steamless.CLI.exe"

# The property that matters: the CLI resolves Steamless.API.dll beside itself,
# not under Plugins/. A stage that only has it in Plugins/ looks correct here
# and still fails at runtime, so assert the top level explicitly.
[ -f "$STAGE/Steamless.API.dll" ] \
    && ok "Steamless.API.dll sits beside the CLI (what the CLI actually loads)" \
    || bad "Steamless.API.dll is not beside the CLI; Steamless will fail at runtime"

[ -f "$STAGE/Steamless.CLI.exe" ] \
    && ok "Steamless.CLI.exe staged and runnable" \
    || bad "Steamless.CLI.exe missing from the stage"

[ -f "$WORK/v2.exe.original.exe" ] \
    && ok "original executable backed up before the swap" \
    || bad "no .original.exe backup was left behind"

[ -f "$WORK/v2.exe.steamless_done" ] \
    && ok "completion marker written (repeat launches skip the unpack)" \
    || bad "no .steamless_done marker was written"

grep -q 'Steamless.CLI.exe' "$STUB_LOG" 2>/dev/null \
    && ok "wine invoked Steamless.CLI.exe" \
    || bad "wine was not invoked with Steamless.CLI.exe"

chmod -R u+w "$RO_HOME" 2>/dev/null || true

echo "== total: $PASS passed, $FAIL failed =="
[ "$FAIL" -eq 0 ]