#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-only
#
# test-autoupdate.sh - exercise tools/ember-autoupdate.sh without a network.
#
# The updater is the thing that decides whether a Steam launch quietly replaces
# the injected payload, so the cases that matter are the boring ones: a normal
# update, a throttle that stops it running every launch, a bad hash, a downgrade
# and an install that predates version stamping. All of those must leave the
# files on disk exactly as they were.

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
UPDATER="$REPO_ROOT/tools/ember-autoupdate.sh"

PASS=0; FAIL=0
ok()  { echo "  ok   - $*"; PASS=$((PASS+1)); }
bad() { echo "  FAIL - $*" >&2; FAIL=$((FAIL+1)); }

[ -f "$UPDATER" ] || { echo "missing $UPDATER" >&2; exit 1; }

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# ── fake release ───────────────────────────────────────────────────────────
FAKE="$WORK/fake"
mkdir -p "$FAKE/bin" "$FAKE/stage/ember-9.9/bin"

printf '\177ELF\x02\x01\x01 fake payload\n' >"$FAKE/stage/ember-9.9/bin/ember-core.so"
printf 'new library-inject\n' >"$FAKE/stage/ember-9.9/bin/ember-library-inject.so"
printf '#!/bin/sh\necho new pattern-refresh\n' >"$FAKE/stage/ember-9.9/bin/pattern-refresh"
chmod 755 "$FAKE/stage/ember-9.9/bin/pattern-refresh"
( cd "$FAKE/stage" && zip -qr "$FAKE/asset.zip" ember-9.9 )
(cd "$FAKE" && sha256sum asset.zip | awk '{print $1 "  asset.zip"}' >asset.zip.sha256)

write_release() {
	local tag="$1"
	cat >"$FAKE/release.json" <<EOF
{"tag_name":"$tag","assets":[
 {"name":"ember-linux-${tag#v}.zip","browser_download_url":"https://example.invalid/ember-linux-${tag#v}.zip"},
 {"name":"ember-linux-${tag#v}.zip.sha256","browser_download_url":"https://example.invalid/ember-linux-${tag#v}.zip.sha256"}]}
EOF
}

cat >"$FAKE/bin/curl" <<'EOF'
#!/bin/sh
url=""
for arg in "$@"; do url="$arg"; done
case "$url" in
	*/repos/fake/port/*) exit 22 ;;
	*api.github.com*/releases/latest) cat "$FAKE_DIR/release.json" ;;
	*.zip.sha256) cat "$FAKE_DIR/asset.zip.sha256" ;;
	*.zip) cat "$FAKE_DIR/asset.zip" ;;
	*) exit 22 ;;
esac
EOF
chmod 755 "$FAKE/bin/curl"
export FAKE_DIR="$FAKE"
export PATH="$FAKE/bin:$PATH"

# ── helper: fresh install dir ──────────────────────────────────────────────
new_install() { # $1 = recorded version ('' for none)
	local dir="$WORK/sls$RANDOM"
	mkdir -p "$dir/bin" "$dir/backup"
	printf 'old ember\n' >"$dir/ember-core.so"
	printf 'old library-inject\n' >"$dir/ember-library-inject.so"
	printf '#!/bin/sh\necho old pattern-refresh\n' >"$dir/pattern-refresh"
	[ -n "${1:-}" ] && printf '%s\n' "$1" >"$dir/version"
	printf '%s' "$dir"
}

run_updater() { # $1 = slsdir, rest = env pairs
	local sls="$1"; shift
	env HOME="$WORK/home" EMBERDIR="$sls" EMBER_STATE_DIR="$WORK/state$RANDOM" \
	    EMBER_AUTOUPDATE_INTERVAL=0 EMBER_AUTOUPDATE_REPO=fake/repo "$@" \
	    sh "$UPDATER"
}

echo "== a newer release replaces the payload"
write_release v9.9
STATE_DIR="$(new_install 1.0)"
run_updater "$STATE_DIR"
grep -q 'fake payload' "$STATE_DIR/ember-core.so" && ok "ember-core.so replaced" || bad "ember-core.so not replaced"
[ -x "$STATE_DIR/pattern-refresh" ] && ok "pattern-refresh executable" || bad "pattern-refresh not executable"
[ "$(cat "$STATE_DIR/version" 2>/dev/null)" = "9.9" ] && ok "version stamped 9.9" || bad "version not stamped"
ls -d "$STATE_DIR"/backup/autoupdate-* >/dev/null 2>&1 && ok "old payload backed up" || bad "no backup"
grep -q 'payload updated 1.0 -> 9.9' "$WORK"/state*/autoupdate.log 2>/dev/null \
	&& ok "update logged" || bad "update not logged"

echo "== an unreachable port repo falls back to the fork"
write_release v9.9
STATE_DIR="$(new_install 1.0)"
STATE="$WORK/fallback-state"
env HOME="$WORK/home" EMBERDIR="$STATE_DIR" EMBER_STATE_DIR="$STATE" \
    EMBER_AUTOUPDATE_INTERVAL=0 \
    EMBER_AUTOUPDATE_REPO=fake/port EMBER_AUTOUPDATE_REPO_FALLBACK=fake/fork sh "$UPDATER"
grep -q 'fake payload' "$STATE_DIR/ember-core.so" && ok "payload updated from fallback" || bad "fallback did not update"
grep -q 'no release from fake/port' "$STATE/autoupdate.log" 2>/dev/null \
	&& ok "fallback logged" || bad "fallback not logged"

echo "== same version does nothing"
write_release v9.9
STATE_DIR="$(new_install 9.9)"
run_updater "$STATE_DIR"
grep -q 'old ember' "$STATE_DIR/ember-core.so" && ok "payload untouched" || bad "payload changed"

echo "== throttle stops an immediate re-check"
write_release v9.9
STATE_DIR="$(new_install 1.0)"
STATE="$WORK/throttle-state"
env HOME="$WORK/home" EMBERDIR="$STATE_DIR" EMBER_STATE_DIR="$STATE" \
    EMBER_AUTOUPDATE_INTERVAL=3600 EMBER_AUTOUPDATE_REPO=fake/repo sh "$UPDATER"
grep -q 'fake payload' "$STATE_DIR/ember-core.so" && ok "first run updated" || bad "first run did not update"
printf 'old again\n' >"$STATE_DIR/ember-core.so"
printf '1.0\n' >"$STATE_DIR/version"
env HOME="$WORK/home" EMBERDIR="$STATE_DIR" EMBER_STATE_DIR="$STATE" \
    EMBER_AUTOUPDATE_INTERVAL=3600 EMBER_AUTOUPDATE_REPO=fake/repo sh "$UPDATER"
grep -q 'old again' "$STATE_DIR/ember-core.so" && ok "second run throttled" || bad "second run ignored the throttle"

echo "== a bad hash leaves the install alone"
write_release v9.9
printf '%064d\n' 0 >"$FAKE/asset.zip.sha256"
STATE_DIR="$(new_install 1.0)"
run_updater "$STATE_DIR"
grep -q 'old ember' "$STATE_DIR/ember-core.so" && ok "payload untouched" || bad "payload replaced anyway"
[ "$(cat "$STATE_DIR/version" 2>/dev/null)" = "1.0" ] && ok "version untouched" || bad "version moved"
grep -q 'sha256 mismatch' "$WORK"/state*/autoupdate.log 2>/dev/null \
	&& ok "mismatch logged" || bad "mismatch not logged"

echo "== opt-out is honoured"
write_release v9.9
STATE_DIR="$(new_install 1.0)"
run_updater "$STATE_DIR" EMBER_NO_AUTOUPDATE=1
grep -q 'old ember' "$STATE_DIR/ember-core.so" && ok "payload untouched" || bad "payload replaced"

echo "== an install without a version stamp is left alone"
write_release v9.9
STATE_DIR="$(new_install '')"
run_updater "$STATE_DIR"
grep -q 'old ember' "$STATE_DIR/ember-core.so" && ok "payload untouched" || bad "payload replaced"
grep -q 'no version recorded' "$WORK"/state*/autoupdate.log 2>/dev/null \
	&& ok "reason logged" || bad "reason not logged"

echo "== an older release never downgrades the install"
write_release v1.0
STATE_DIR="$(new_install 9.9)"
run_updater "$STATE_DIR"
grep -q 'old ember' "$STATE_DIR/ember-core.so" && ok "payload untouched" || bad "payload downgraded"

echo "== wrapper and installer wire the updater up"
grep -q 'ember-autoupdate.sh' "$REPO_ROOT/setup.sh" \
	&& ok "setup.sh mentions the updater" || bad "setup.sh does not install it"
grep -q 'ember-autoupdate.sh' "$REPO_ROOT/scripts/package.sh" \
	&& ok "the release zip carries the updater" || bad "package.sh leaves the updater out"
grep -q 'EMBER_NO_AUTOUPDATE' "$REPO_ROOT/setup.sh" \
	&& ok "wrapper honours the opt-out" || bad "wrapper ignores the opt-out"

echo
if [ "$FAIL" -ne 0 ]; then
	echo "test-autoupdate: $PASS passed, $FAIL failed" >&2
	exit 1
fi
echo "test-autoupdate: all $PASS passed"
