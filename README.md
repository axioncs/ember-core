# ember-core

The shared library behind [ember](https://github.com/axioncs/ember). Steam loads it
at startup and it answers Steam's ownership and licence questions, builds install
plans for apps the account does not own, supplies depot keys, and stages manifests.

A fork of [AceSLS/SLSsteam](https://github.com/AceSLS/SLSsteam), branch `ember`,
based on its `dev-multiarch` work. Everything below in *What this fork adds* is ours;
the rest is upstream's and stays theirs.

## What this fork adds

- **stplug-in importer** — reads `~/.steam/steam/config/stplug-in/*.lua` for app
  ids, depot keys and manifest pins, and reloads when the folder changes.
- **Depot plan synthesis** — Steam returns an empty depot list for unowned apps, so
  the plan is built from the imported pins, filtered by appinfo `oslist`,
  `language` and `baselanguages`, and split into normal and shared depots.
- **Manifest store publishing** — copies staged manifests into Steam's depot cache
  on every config load, refusing anything that is not a real manifest.
- **Ticket stamp** — stamps denied ownership-ticket responses so the downloader
  proceeds.
- **Depot key substitution** — supplies an imported key if the CM ever refuses one.
- **Crash guard** — a marker written before hooks are placed; if it survives, the
  next start runs vanilla so the client is always reachable.
- **Ownership SteamID fix** — ignores the anonymous sentinel the client returns
  before login, which otherwise disables every ownership check.
- **Diagnostics** — names each code signature that fails to resolve after a Steam
  update.

## Build

Requires `g++`, `make`, `pkg-config`, and the development files for `libcurl`,
`dbus-1` and `luajit`.

```sh
make audit-libs        # produces bin/SLSsteam.so
```

On NixOS, build it through the ember flake instead — it supplies the include paths
and pins the runtime dependencies:

```sh
nix build github:axioncs/ember#core
```

## Test

```sh
make test-stplugin
```

Parses a set of fixtures covering the formats and failure modes seen in the wild.
No Steam required.

## Layout

| Path | What |
|---|---|
| `src/main.cpp` | Startup: crash guard, module discovery, hook installation |
| `src/hooks.cpp` | Every detour, in one place |
| `src/patterns.cpp` | Byte signatures into `steamclient.so` |
| `src/vftableinfo.cpp` | Hookable interface methods, resolved by name |
| `src/feats/` | Features: apps, dlc, ticket, depot keys, manifests, achievements |
| `src/sdk/` | Steam interface and structure definitions |
| `tools/test_stplugin.cpp` | Parser tests |

## License

AGPL-3.0-only. See `LICENSE`.

Upstream: [AceSLS/SLSsteam](https://github.com/AceSLS/SLSsteam). Bundled libraries:
libmem (rdbo), protobuf (Google), yaml-cpp (jbeder), LuaJIT.
