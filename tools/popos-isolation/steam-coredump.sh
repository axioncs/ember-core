#!/bin/sh
# Launches Steam under ember (LD_AUDIT) with core dumps enabled so we can
# get a precise backtrace of the SIGILL in the IPC:CSteamEngine thread.
ulimit -c unlimited
EMBERDIR="$HOME/.local/share/ember"
LD_AUDIT="$EMBERDIR/ember-library-inject.so:$EMBERDIR/ember-core.so${LD_AUDIT:+:$LD_AUDIT}" exec /usr/games/steam "$@"
