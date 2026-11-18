#!/bin/sh
# SPDX-License-Identifier: AGPL-3.0-only
#
# Keep the injected payload current with our own releases.
#
# ember finds Steam's internals by pattern matching, so every client update
# moves something and the payload that used to work starts missing - games fail
# to launch, achievements stay empty. Reinstalling by hand is the old answer;
# this runs detached from the wrapper at launch and swaps in a newer payload.
#
# What it does NOT do: touch the wrapper, the desktop entries or any script.
# Only bin/ember-core.so, bin/ember-library-inject.so and bin/pattern-refresh are
# replaced, and each one is written to a temp file and renamed into place, so a
# Steam launch never loads half a library. The previous files stay under
# backup/autoupdate-<timestamp>/ in case a release turns out to be bad.
#
# Nothing here is allowed to break a launch: no network, no unzip, a bad hash,
# an unknown installed version - all of that just leaves the install alone.
#
# Environment:
#   EMBER_NO_AUTOUPDATE=1       skip everything
#   EMBER_AUTOUPDATE_REPO       owner/repo (default ogb4n/ember-moon-x64)
#   EMBER_AUTOUPDATE_REPO_FALLBACK  owner/repo tried when the first has no
#                              reachable release (default Rafie-kun/ember)
#   EMBER_AUTOUPDATE_INTERVAL   seconds between checks (default 6h)
#   EMBER_STATE_DIR             state/log directory (tests)
#   EMBERDIR                     install directory (default ~/.local/share/ember)

set -u

EMBERDIR="${EMBERDIR:-$HOME/.local/share/ember}"
REPO="${EMBER_AUTOUPDATE_REPO:-ogb4n/ember-moon-x64}"
REPO_FALLBACK="${EMBER_AUTOUPDATE_REPO_FALLBACK:-Rafie-kun/ember}"
INTERVAL="${EMBER_AUTOUPDATE_INTERVAL:-21600}"
STATE="${EMBER_STATE_DIR:-${XDG_STATE_HOME:-$HOME/.local/state}/ember}"
LOG="$STATE/autoupdate.log"

log() {
	printf '%s %s\n' "$(date '+%F %T' 2>/dev/null)" "$1" >>"$LOG" 2>/dev/null || true
}

# newer <candidate> <installed>: true when candidate is a higher x.y.z version.
newer() {
	awk -v a="$1" -v b="$2" 'BEGIN {
		n = split(a, A, "."); m = split(b, B, ".")
		for (i = 1; i <= (n > m ? n : m); i++) {
			x = (i <= n ? A[i] : 0) + 0
			y = (i <= m ? B[i] : 0) + 0
			if (x > y) exit 0
			if (x < y) exit 1
		}
		exit 1
	}' 2>/dev/null
}

[ "${EMBER_NO_AUTOUPDATE:-0}" = 1 ] && exit 0
[ -f "$EMBERDIR/ember-core.so" ] || exit 0
mkdir -p "$STATE" 2>/dev/null || exit 0

# Throttle first, network second: a machine that is offline most of the time
# should not start a download attempt on every single Steam launch.
now="$(date +%s 2>/dev/null)" || exit 0
last="$(cat "$STATE/last-check" 2>/dev/null || echo 0)"
case "$last" in ''|*[!0-9]*) last=0 ;; esac
[ $((now - last)) -lt "$INTERVAL" ] && exit 0
printf '%s\n' "$now" >"$STATE/last-check" 2>/dev/null || exit 0

# Two Steam launches at once must not race each other over the payload.
if command -v flock >/dev/null 2>&1; then
	exec 9>"$STATE/autoupdate.lock" 2>/dev/null || exit 0
	flock -n 9 2>/dev/null || exit 0
fi

# No version recorded means we cannot tell whether the release is newer (an
# install from before version stamping, or a hand-built payload). Leave it be
# and say so once, so `tail autoupdate.log` explains why nothing happens.
installed="$(tr -d '[:space:]' <"$EMBERDIR/version" 2>/dev/null || true)"
if [ -z "$installed" ]; then
	log "no version recorded in $EMBERDIR; not touching the payload"
	exit 0
fi

command -v curl >/dev/null 2>&1 || exit 0
json="$(curl -fsSL --max-time 10 "https://api.github.com/repos/$REPO/releases/latest" 2>/dev/null)" || json=""
if ! printf '%s' "$json" | grep -q '"tag_name"'; then
	# The x64 port repo can be private or have no release yet; our fork builds
	# the same payload and always has one.
	[ -n "$REPO_FALLBACK" ] || exit 0
	log "no release from $REPO; trying $REPO_FALLBACK"
	json="$(curl -fsSL --max-time 10 "https://api.github.com/repos/$REPO_FALLBACK/releases/latest" 2>/dev/null)" || exit 0
	REPO="$REPO_FALLBACK"
fi
tag="$(printf '%s' "$json" | sed -n 's/.*"tag_name"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n 1)"
latest="${tag#v}"
[ -n "$latest" ] || exit 0
newer "$latest" "$installed" || exit 0

url="$(printf '%s' "$json" \
	| grep -o '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]*"' \
	| sed 's/.*"\(https[^"]*\)"$/\1/' \
	| grep -E '/(ember|ember-moon-x64)-linux-[^/]*\.zip$' \
	| head -n 1)"
[ -n "$url" ] || exit 0

tmp="$(mktemp -d 2>/dev/null)" || exit 0
trap 'rm -rf "$tmp"' EXIT INT TERM
zip="$tmp/payload.zip"
curl -fsSL --max-time 120 "$url" >"$zip" 2>/dev/null || exit 0

# The release workflow publishes a sha256 sidecar next to the zip. Without a
# readable one we skip instead of unpacking something half-downloaded.
expected="$(curl -fsSL --max-time 15 "$url.sha256" 2>/dev/null | awk '{print $1}' | head -c 64)"
[ "${#expected}" -eq 64 ] || { log "no sha256 sidecar for $tag; skipping"; exit 0; }
if command -v sha256sum >/dev/null 2>&1; then
	actual="$(sha256sum "$zip" 2>/dev/null | awk '{print $1}')"
else
	actual="$(shasum -a 256 "$zip" 2>/dev/null | awk '{print $1}')"
fi
[ "$actual" = "$expected" ] || { log "sha256 mismatch for $tag; skipping"; exit 0; }

command -v unzip >/dev/null 2>&1 || exit 0
mkdir -p "$tmp/x" || exit 0
unzip -q "$zip" -d "$tmp/x" 2>/dev/null || exit 0
src="$(find "$tmp/x" -mindepth 1 -maxdepth 1 -type d 2>/dev/null | head -n 1)"
[ -n "$src" ] || exit 0
for f in ember-core.so ember-library-inject.so pattern-refresh; do
	[ -s "$src/bin/$f" ] || exit 0
done
# Refuse to install something that is not an ELF shared object / binary.
[ "$(head -c 4 "$src/bin/ember-core.so" | od -An -tx1 | tr -d ' \n')" = "7f454c46" ] || exit 0

backup="$EMBERDIR/backup/autoupdate-$(date '+%Y%m%d-%H%M%S')"
mkdir -p "$backup" 2>/dev/null || exit 0
for f in ember-core.so ember-library-inject.so pattern-refresh; do
	cp -p "$EMBERDIR/$f" "$backup/$f" 2>/dev/null || true
done

replaced=0
for f in ember-core.so ember-library-inject.so pattern-refresh; do
	if cp "$src/bin/$f" "$EMBERDIR/.$f.new" 2>/dev/null \
	   && chmod 755 "$EMBERDIR/.$f.new" 2>/dev/null \
	   && mv -f "$EMBERDIR/.$f.new" "$EMBERDIR/$f" 2>/dev/null; then
		replaced=$((replaced + 1))
	else
		rm -f "$EMBERDIR/.$f.new" 2>/dev/null || true
	fi
done
if [ "$replaced" -ne 3 ]; then
	log "could not replace the payload for $tag; old files are in $backup"
	exit 1
fi
printf '%s\n' "$latest" >"$EMBERDIR/version" 2>/dev/null || true
log "payload updated $installed -> $latest"

# Keep the last three backups; older ones are just disk.
ls -dt "$EMBERDIR"/backup/autoupdate-* 2>/dev/null | tail -n +4 | while IFS= read -r old; do
	rm -rf "$old" 2>/dev/null || true
done
