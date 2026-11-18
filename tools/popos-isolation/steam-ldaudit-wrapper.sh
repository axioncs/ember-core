#!/bin/sh
# ember wrapper - injects via LD_AUDIT (rtld-audit).
EMBERDIR="$HOME/.local/share/ember"
LD_AUDIT="$EMBERDIR/ember-library-inject.so:$EMBERDIR/ember-core.so${LD_AUDIT:+:$LD_AUDIT}" exec /usr/games/steam "$@"
