# Keeping the patterns alive

ember hooks a handful of functions inside `steamclient.so`. Valve ships a new client
every few weeks, and when one of those functions moves, its byte signature stops
matching. That is the single most common way ember breaks, and it is the reason the
previous project in this space died: nobody was left to re-derive the signatures.

This is the procedure for doing it.

## How it works now

`src/patterns.cpp` holds one entry per target: a name, a byte pattern with `?`
wildcards, and a follow mode.

- `SigFollowMode::None` — the match address is the function.
- `SigFollowMode::Relative` — the match is a `call`/`jmp` and the real function is
  its target, resolved from the relative displacement.
- `SigFollowMode::PrologueUpwards` — the match is inside the function and the real
  entry is found by walking up to the prologue bytes given alongside.

On startup `Patterns::init()` resolves every entry. If any fails, it logs:

```
Pattern not found: CUser::CheckAppOwnership
Failed to find all patterns! Aborting...
```

and the library unhooks itself. Steam then runs stock. `ember doctor` surfaces the
same list under `patterns`.

## Diagnosing an update

```sh
rg 'Pattern not found' ~/.ember.log
```

That names exactly what moved. Everything else is fine — only the listed ones need
work. Note that a *single* missing required pattern aborts the whole set, so fix
them all before testing.

## Re-deriving a signature

Pick the target, then find it in the new binary. Three approaches, in order of
reliability:

### 1. A nearby string

Most hooked functions touch a log string, a file name, or a network field name.
Search the binary for the string, find the code that references it, and disassemble
around it.

```sh
strings -a -t x steamclient.so | rg 'some log text'
```

The offset is in the file; `objdump -d --start-address=… --stop-address=…` on the
corresponding virtual address gives the surrounding code. The function boundary is
usually a `endbr64` / `push rbp` preceded by `ret` or `int3` padding.

### 2. A call site you can still find

If the target calls something whose pattern still resolves, disassemble the caller.
`CUser::CheckAppOwnership` is reached from few places; `CSteamEngine::GetServerPipe`
is called all over the client and is therefore easy to find from the other end.

### 3. Behaviour

Set a breakpoint on the surviving neighbour, step to the call you care about, and
read the address. `gdb -p <steam pid>` with `set follow-fork-mode child` works;
Steam is statically linked and the client binary is not stripped of code.

## Turning an address into a pattern

Take 16–32 bytes starting at the function entry, and wildcard everything that is a
displacement, an immediate that can change between builds, or a call target:

```
48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 8B 4B
```

Rules that keep a pattern stable across builds:

- Wildcard all four bytes of any `rel32` (`E8`, `E9`, `0F 84`…).
- Wildcard `mov`/`lea` displacements into data (`48 8B 05 ? ? ? ?`).
- Keep at least six concrete bytes. Fewer and it matches in several places.
- Never start with `40`/`41` (REX prefixes vary) or with stack offsets.

Verify uniqueness before committing — the resolver takes the first match, so a
pattern that matches twice will hook the wrong thing:

```sh
python3 - <<'PY'
import re
data = open("steamclient.so", "rb").read()
pat = "48 89 5C 24 ? 57 48 83 EC ?".split()
rx = re.compile(b"".join(b"." if p == "?" else re.escape(bytes([int(p, 16)])) for p in pat), re.S)
print(len(rx.findall(data)), "match(es)")
PY
```

## Testing

```sh
# build through the ember flake so includes and rpaths are right
cd ~/.local/share/ember && nix build .#steam

# launch that build, then
rg 'Patterns:|Pattern not found' ~/.ember.log
```

`Patterns: all N signature(s) resolved` means every entry matched. Then exercise
the feature that pattern feeds — ownership needs a game to appear owned, the depot
plan needs an install to show a real size.

## When upstream moves

ember's `ember` branch descends from upstream's `dev-multiarch`: every upstream
commit is an ancestor, and ember's own work sits on top. `git merge` is therefore
the right operation, and a clean merge is the normal outcome.

A rebase is only needed if the branch was re-imported by hand, which shows up as
`fatal: refusing to merge unrelated histories` and as a non-zero merge-base
check. If `git merge-base ember upstream/dev-multiarch` prints a commit, no rebase
is needed — verify with `git rev-list --count ember..upstream/dev-multiarch` (0
means up to date) rather than assuming.

The `upstream` workflow in this repository runs daily. It fetches
`AceSLS/SLSsteam@dev-multiarch` and either opens a pull request (clean merge) or
updates a single standing issue (conflict). A new upstream commit usually means a
new Steam client, so:

1. Read the PR/issue and look for new pattern values upstream already derived.
2. Merge, then run the build and `rg 'Pattern not found'`.
3. Derive whatever upstream did not cover.
4. Bump `res/version.txt` so the release is identifiable.

Do not force a conflicting merge. `src/feats/apps.cpp` and `src/hooks.cpp` carry a
lot of ember-specific code; resolve those by hand.

If the workflow ever reports `refusing to merge unrelated histories`, the branch
has been re-imported rather than rebased — the two histories share no commit.
That is not a conflict to resolve in the merge; rebase ember's own commits onto
`upstream/dev-multiarch` instead:

```sh
# The newest commit ember shares with upstream — the fork point.
base=$(git merge-base ember upstream/dev-multiarch)
git rebase --onto upstream/dev-multiarch "$base" ember
```

If `git merge-base` returns nothing, the histories are genuinely unrelated and
the fork point must be identified by subject match instead:

```sh
# Upstream's copy of ember's fork-point commit, by identical subject line.
git log --format='%s' ember | tail -n +1 > /tmp/subjects
git log --format='%h %s' upstream/dev-multiarch | grep -Fxf /tmp/subjects | tail -1
```

Afterwards `git rev-list --count ember..upstream/dev-multiarch` is 0, and the
workflow's clean-merge path works again. Force-push the branch (`--force-with-lease`)
and re-point any tag that sat on the old history — `v0.1.0` was on a pre-rebase
commit and had to be moved to its rebased equivalent.

ember's history is **related** to upstream: 1196 of its commits are upstream's,
replayed with new SHAs. A `git log --format=%s` overlap of ~1189 with upstream is
the signature of that, and it means patch-id comparison is the right tool, not SHA.

## Inventory

Which feature each pattern feeds, so a failure points at the user-visible symptom.

| Pattern | Feeds |
|---|---|
| `CUser::CheckAppOwnership` | Whether a game shows as owned. The most visible failure |
| `CUser::GetSubscribedApps` | The subscribed-apps list the client builds |
| `CUser::UpdateAppOwnershipTicket` | Ownership ticket refresh |
| `CUser::SpawnGameId` | Launching a game through a different app id |
| `CUser::PostCallback` / `PostCallbackToAppId` | Callbacks routed to the right app pipe |
| `CSteamEngine::ProcessIPCFrame` | Where the per-pipe hooks get installed |
| `CSteamEngine::SetAppIdForCurrentPipe` | Knowing which app a pipe belongs to |
| `CSteamEngine::GetServerPipe` | The pipe table used by several features |
| `CUserAppManager::BuildDepotDependency` | The install plan and its size |
| `CAPIJob::SendAndRecv` | Requests the client makes to Valve |
| `CAppDataCache::BParseResponseMessage` | App metadata the plan reads |
| `CCMInterface::RecvPkt` (via VFT) | Ticket stamp, depot keys, achievements |
| `CWebSocketConnection::BBuildAndAsyncSendFrame` | Client messages carrying app info |
| `CUDPConnection::BAsyncSend` | Game-server queries |
| `TraceIPC` | Diagnostics only; safe to lose |
