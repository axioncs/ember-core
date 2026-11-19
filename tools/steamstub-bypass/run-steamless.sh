#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-only
#
# run-steamless.sh — wrapper-integration helper.  Invokes the
# bundled Steamless build under Wine to process a Windows
# executable. Designed to be called by the ember .so right
# before Steam launches a configured-AdditionalApps binary.
#
# Mirrors Accela's direct-Wine invocation pattern (cleaner than
# `proton run` for this case).
#
# Usage:
#   run-steamless.sh <exe-path>            # full unpack + swap
#   run-steamless.sh --prewarm             # initialise the Wine prefix
#                                          #   only; no exe required
#
# The --prewarm form is meant to be called once at ember startup
# in the background so the slow first-time Wine boot (~30s) completes
# before the user clicks Play. Subsequent unpacks finish in ~3s
# because the prefix is already initialised and the wineserver is
# still warm.
#
# Exit codes:
#   0  helper succeeded (or prewarm finished); when run with an exe
#      argument, original.exe.original.exe is the backup and the
#      processed binary now lives at original.exe
#   1  invalid arguments
#   2  exe doesn't carry the wrapper signature — already in target
#      shape, no work needed
#   3  no usable Wine binary (system wine missing, no Proton installed)
#   4  Steamless run failed (compile/dependency/timeout)
#   5  Steamless ran but produced no .unpacked.exe
#   6  rename step failed
#
# Environment variables (optional):
#   STEAMLESS_HOME    where Steamless.CLI.exe + Plugins/ live;
#                     default: <script dir>/../steamless-bin
#   WINE_BIN          override wine binary path (default: probe Proton then PATH)
#   WINESERVER        override the compatible wineserver binary; must be
#                     executable and match WINE_BIN when set
#   WINE_PREFIX       override prefix path
#                     (default: ~/.local/share/ember/steamless-prefix)
#   QUIET             non-empty -> suppress informational stdout
#                     (errors still go to stderr)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PREWARM=0
EXE_PATH=""

case "${1:-}" in
    --prewarm)
        PREWARM=1
        ;;
    *)
        EXE_PATH="${1:-}"
        ;;
esac

log()  { [ -z "${QUIET:-}" ] && echo "[steamless-bypass] $*"; return 0; }
warn() { echo "[steamless-bypass] WARN: $*" >&2; }
die()  { echo "[steamless-bypass] ERROR: $*" >&2; exit "${2:-1}"; }

# Every bounded Wine/Steamless operation below relies on the GNU coreutils
# timeout command.  Fail before touching the prefix when a minimal host does
# not provide it, instead of leaking a confusing shell rc=127 or leaving a
# half-initialised prefix behind.
TIMEOUT_BIN="$(command -v timeout 2>/dev/null || true)"
[ -n "$TIMEOUT_BIN" ] || die "required command 'timeout' not found in PATH" 4

# ── 1. validate args ────────────────────────────────────────────────────
if [ "$PREWARM" -eq 0 ]; then
    [ -n "$EXE_PATH" ] || die "usage: $0 <exe-path>  |  $0 --prewarm" 1
    [ -f "$EXE_PATH" ] || die "exe not found: $EXE_PATH" 1

    # Refuse our own artefacts so a buggy caller can't recurse on the
    # backup we left behind (`<exe>.original.exe`) or the transient
    # output (`<exe>.unpacked.exe`). Treated as a no-op success so
    # batch callers don't bail.
    case "$(basename "$EXE_PATH")" in
        *.original.exe|*.unpacked.exe)
            log "exe is a Steamless artefact ($(basename "$EXE_PATH")); skipping"
            exit 2
            ;;
    esac

    # ── 2. probe SteamStub markers ──────────────────────────────────────
    # Two variants we handle:
    #   - v2 (x86): 'VLV\0' magic at file offset 0x40.
    #   - v3 (x86/x64): no fixed offset magic, but the unpacker header
    #     lives in a PE section named ".bind". Reading the section
    #     table is the canonical detection (Steamless does the same).
    # Quick exit if neither marker is present, to avoid the 5-30 s
    # Wine startup cost on plain non-stub'd binaries.
    #
    # Detection runs on od alone. The helper is forked from inside the
    # Steam client, so it inherits the runtime container's PATH, which is
    # a minimal coreutils set with no python3 — a python3-based probe
    # aborts here with rc=127 and silently disables DRM removal.
    hex_at() {
        od -An -v -tx1 -j "$1" -N "$2" "$3" 2>/dev/null | tr -d ' \n'
    }

    detect_stub_kind() {
        local exe="$1" h name rest i
        local e_lfanew nsec optsize sec_off

        [ "$(hex_at 64 4 "$exe")" = "564c5600" ] && { echo v2; return; }
        [ "$(hex_at 0 2 "$exe")" = "4d5a" ] || { echo none; return; }

        h="$(hex_at 60 4 "$exe")"
        e_lfanew=$(( 16#${h:0:2} | 16#${h:2:2} << 8 | 16#${h:4:2} << 16 | 16#${h:6:2} << 24 ))
        [ "$(hex_at "$e_lfanew" 4 "$exe")" = "50450000" ] || { echo none; return; }

        h="$(hex_at $((e_lfanew + 6)) 2 "$exe")"
        nsec=$(( 16#${h:0:2} | 16#${h:2:2} << 8 ))
        h="$(hex_at $((e_lfanew + 20)) 2 "$exe")"
        optsize=$(( 16#${h:0:2} | 16#${h:2:2} << 8 ))
        sec_off=$(( e_lfanew + 24 + optsize ))

        [ "$nsec" -gt 0 ] && [ "$nsec" -le 96 ] || { echo none; return; }

        i=0
        while [ "$i" -lt "$nsec" ]; do
            name="$(hex_at $((sec_off + i * 40)) 8 "$exe")"
            case "$name" in
                2e62696e64*)
                    rest="${name:10}"
                    case "$rest" in
                        *[!0]*) ;;
                        *) echo v3; return ;;
                    esac
                    ;;
            esac
            i=$((i + 1))
        done

        echo none
    }

    stub_kind="$(detect_stub_kind "$EXE_PATH")"
    case "$stub_kind" in
        v2)
            log "SteamStub v2 (VLV) signature detected — proceeding"
            ;;
        v3)
            log "SteamStub v3 (.bind section) detected — proceeding"
            ;;
        *)
            log "exe is not SteamStub-wrapped (no VLV magic, no .bind section); skipping"
            exit 2
            ;;
    esac
fi

# ── 3. resolve Steamless home (skipped in --prewarm) ───────────────────
if [ "$PREWARM" -eq 0 ]; then
    STEAMLESS_HOME="${STEAMLESS_HOME:-$SCRIPT_DIR/../steamless-bin}"
    STEAMLESS_CLI="$STEAMLESS_HOME/Steamless.CLI.exe"
    [ -f "$STEAMLESS_CLI" ] || die "Steamless.CLI.exe not found at $STEAMLESS_CLI" 4

    # Steamless's CLI exe loads Steamless.API.dll from the working
    # directory. Plugins live in Plugins/. If the API dll is only in
    # Plugins/, copy it up so the CLI can find it. Idempotent.
    #
    # The bundled kit may live in a read-only store path, so never write
    # beside it: stage a writable copy under the ember data dir and run
    # from there instead.
    stage_steamless_kit() {
        STAGING="${EMBER_STEAMLESS_STAGE:-$HOME/.local/share/ember/steamless-bin}"
        mkdir -p "$STAGING" || die "cannot create writable Steamless stage at $STAGING" 4
        # -L dereferences, --no-preserve drops the read-only modes the
        # store ships with: the stage has to stay writable so a later
        # run can refresh it, and so cleanup can remove it.
        cp -rfL --no-preserve=mode,ownership,timestamps \
            "$STEAMLESS_HOME/." "$STAGING/" 2>/dev/null || true
        chmod -R u+w "$STAGING" 2>/dev/null || true
        [ -f "$STAGING/Steamless.CLI.exe" ] \
            || die "staged Steamless kit is incomplete at $STAGING" 4
        # The CLI resolves the API dll beside itself, not under Plugins/, so
        # staging alone is not enough: without this the run starts and then
        # dies inside Steamless with the DRM still applied.
        if [ ! -f "$STAGING/Steamless.API.dll" ] \
           && [ -f "$STAGING/Plugins/Steamless.API.dll" ]; then
            cp "$STAGING/Plugins/Steamless.API.dll" "$STAGING/Steamless.API.dll"
        fi
        [ -f "$STAGING/Steamless.API.dll" ] \
            || die "staged kit has no Steamless.API.dll beside the CLI" 4
        log "staged Steamless kit into writable $STAGING"
        STEAMLESS_HOME="$STAGING"
        STEAMLESS_CLI="$STEAMLESS_HOME/Steamless.CLI.exe"
    }

    if [ ! -f "$STEAMLESS_HOME/Steamless.API.dll" ] \
       && [ -f "$STEAMLESS_HOME/Plugins/Steamless.API.dll" ]; then
        if [ -w "$STEAMLESS_HOME" ]; then
            cp "$STEAMLESS_HOME/Plugins/Steamless.API.dll" "$STEAMLESS_HOME/Steamless.API.dll"
        else
            stage_steamless_kit
        fi
    fi
fi

# ── 4. locate wine binary ───────────────────────────────────────────────
# Order: explicit override > Proton-experimental in standard Steam paths >
#        Proton in compatibilitytools.d > system wine.
find_wine() {
    if [ -n "${WINE_BIN:-}" ] && [ -x "$WINE_BIN" ]; then
        echo "$WINE_BIN"; return
    fi

    local candidates=()
    for steam_root in \
        "$HOME/.steam/steam" \
        "$HOME/.steam/debian-installation" \
        "$HOME/.local/share/Steam"
    do
        candidates+=( "$steam_root/steamapps/common/Proton - Experimental/files/bin/wine" )
        candidates+=( "$steam_root/steamapps/common/Proton"*/files/bin/wine )
        candidates+=( "$steam_root/compatibilitytools.d/"*/files/bin/wine )
        candidates+=( "$steam_root/compatibilitytools.d/"*/dist/bin/wine )
    done
    # System-wide compat tools (e.g. CachyOS ships Proton here).
    candidates+=( "/usr/share/steam/compatibilitytools.d/"*/files/bin/wine )
    candidates+=( "/usr/share/steam/compatibilitytools.d/"*/dist/bin/wine )
    candidates+=( "$(command -v wine 2>/dev/null || true)" )

    for c in "${candidates[@]}"; do
        if [ -n "$c" ] && [ -x "$c" ]; then
            echo "$c"; return
        fi
    done
    return 1
}

WINE_BIN="$(find_wine)" || die "no usable Wine binary found" 3
log "using wine: $WINE_BIN"

# Wine and wineserver have to come from the same install or the
# version mismatch crashes the prefix. Prefer the paired binary, then an
# explicit override, and finally a PATH-resolved wineserver for system Wine
# layouts where the two executables live in different directories.
WINE_DIR="$(dirname "$WINE_BIN")"
WINESERVER_BIN="$WINE_DIR/wineserver"
if [ -n "${WINESERVER:-}" ]; then
    [ -x "$WINESERVER" ] || die "configured WINESERVER is not executable: $WINESERVER" 3
elif [ -x "$WINESERVER_BIN" ]; then
    WINESERVER="$WINESERVER_BIN"
else
    WINESERVER="$(command -v wineserver 2>/dev/null || true)"
    [ -n "$WINESERVER" ] || die "no usable wineserver found at $WINESERVER_BIN or on PATH" 3
    warn "wineserver not at $WINESERVER_BIN; using PATH binary $WINESERVER"
fi

# ── 5. set up environment ───────────────────────────────────────────────
WINE_PREFIX="${WINE_PREFIX:-$HOME/.local/share/ember/steamless-prefix}"
mkdir -p "$WINE_PREFIX"

# If using Proton, point the dynamic linker at Proton's bundled libs.
# Otherwise let the system loader resolve it. The Proton lib paths
# follow the standard layout: <proton_root>/files/{lib,lib64}.
if [[ "$WINE_BIN" == */Proton*/files/bin/wine ]]; then
    PROTON_FILES="$(dirname "$(dirname "$WINE_DIR")")"
    LD_LIBRARY_PATH="$PROTON_FILES/lib64:$PROTON_FILES/lib:${LD_LIBRARY_PATH:-}"
    WINEDLLPATH="$PROTON_FILES/lib64/wine:$PROTON_FILES/lib/wine"
    export LD_LIBRARY_PATH WINEDLLPATH
    log "configured Proton library paths"
fi

export WINEPREFIX="$WINE_PREFIX"
export WINEDEBUG="${WINEDEBUG:--all}"
export WINESERVER
export WINEARCH="${WINEARCH:-win64}"

# Steamless is a .NET app and renders no HTML, so disable Wine's Gecko
# (mshtml) package — otherwise a fresh prefix pops the "install Gecko"
# dialog, which hangs a headless run.  Leave Mono (mscoree) at its
# default so the .NET runtime still loads.
export WINEDLLOVERRIDES="${WINEDLLOVERRIDES:-mshtml=}"

# Different Steam runtime ships overlapping wineservers. Kill any
# stragglers before kicking off our own — version mismatch otherwise
# crashes immediately.
pkill -9 -f wineserver 2>/dev/null || true

# ── 6. lazy prefix init (~30s on first run) ─────────────────────────────
if [ ! -f "$WINEPREFIX/system.reg" ]; then
    log "initializing Wine prefix at $WINEPREFIX (one-time, ~30s)"
    # Both operations can wait on a broken Wine runtime forever.  Keep the
    # prewarm worker bounded so a detach-failure recovery join cannot hang
    # Steam indefinitely; the launch-time path remains the fallback.
    if ! "$TIMEOUT_BIN" 90 "$WINE_BIN" wineboot --init >/dev/null 2>&1; then
        die "wineboot --init failed or timed out" 4
    fi
    if ! "$TIMEOUT_BIN" 90 "$WINESERVER" -w 2>/dev/null; then
        die "wineserver wait failed or timed out" 4
    fi
fi

# Prewarm path stops here — the prefix is now initialised, the wineserver
# is warm, and the next call with a real exe path can run Steamless
# immediately.
if [ "$PREWARM" -eq 1 ]; then
    log "prewarm complete"
    exit 0
fi

# ── 7. invoke Steamless ─────────────────────────────────────────────────
# Steamless writes <exe>.unpacked.exe next to the input. Wine paths
# need to be drive-mapped — Z:\ maps to /. Everything else is just
# slash-flipping.
WIN_PATH="Z:${EXE_PATH//\//\\}"
log "running Steamless on $EXE_PATH"

cd "$STEAMLESS_HOME"

# 90s timeout is generous; Skyrim's 37 MB exe takes ~3s on a warm prefix.
# Steamless CLI exit codes: 0 = unpacked OK, 1 = no SteamStub DRM present
# (benign — treat as "nothing to do"), >1 = real failure.
# The log used to be redirected to /tmp/steamless-bypass.$$.log. A shell
# redirect FOLLOWS symlinks, so a local process that guessed the pid and
# pre-created that path as a symlink had an arbitrary file of the user's
# truncated and overwritten — and the `cat` below would then have printed
# whatever it pointed at. mktemp creates the file itself, refusing an existing
# name, inside a private directory.
# Own subdirectory, deliberately NOT the ember directory the injected library
# manages. Two owners chmod'ing one directory with their results discarded means a
# failure here would make the C++ side refuse that directory and silently lose
# both the API contract and the load lock.
SL_LOG_DIR="${XDG_RUNTIME_DIR:-$HOME/.cache}/ember-steamless"
mkdir -p "$SL_LOG_DIR" 2>/dev/null || true
chmod 700 "$SL_LOG_DIR" 2>/dev/null || true
SL_LOG="$(mktemp "$SL_LOG_DIR/steamless-bypass.XXXXXX.log" 2>/dev/null)" \
    || SL_LOG="$(mktemp 2>/dev/null)" \
    || die "could not create a private log file" 4
chmod 600 "$SL_LOG" 2>/dev/null || true
trap 'rm -f "$SL_LOG"' EXIT

set +e
"$TIMEOUT_BIN" 90 "$WINE_BIN" Steamless.CLI.exe \
        --quiet --realign --recalcchecksum -f "$WIN_PATH" \
        > "$SL_LOG" 2>&1
sl_rc=$?
set -e

if [ "$sl_rc" -eq 1 ] && [ ! -f "$EXE_PATH.unpacked.exe" ]; then
    log "Steamless reports no SteamStub DRM; nothing to do"
    rm -f "$SL_LOG"
    exit 2
fi
if [ "$sl_rc" -ne 0 ]; then
    warn "Steamless invocation failed (rc=$sl_rc); log follows:"
    sed 's/^/  /' "$SL_LOG" >&2
    rm -f "$SL_LOG"
    die "Steamless failed" 4
fi
[ -z "${QUIET:-}" ] && cat "$SL_LOG"
rm -f "$SL_LOG"

UNPACKED="$EXE_PATH.unpacked.exe"
[ -f "$UNPACKED" ] || die "no unpacked output at $UNPACKED" 5

# ── 8. atomic-ish rename ────────────────────────────────────────────────
BACKUP="$EXE_PATH.original.exe"
log "swapping unpacked into place (backup: $BACKUP)"
if [ -f "$BACKUP" ]; then
    rm -f "$BACKUP"
fi
mv "$EXE_PATH" "$BACKUP" || die "backup move failed" 6
mv "$UNPACKED" "$EXE_PATH" || {
    warn "swap-in failed; rolling back"
    mv "$BACKUP" "$EXE_PATH"
    die "swap failed" 6
}
chmod +x "$EXE_PATH"

# Marker so we don't reprocess the same exe forever.
touch "$EXE_PATH.steamless_done"

log "done — $EXE_PATH processed"
exit 0