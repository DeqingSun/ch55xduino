# GitHub Release 0.0.26 — Upload Checklist

Create a GitHub release tagged **`0.0.26`** and upload these files from
`ch55xduino/filePacker/packedFiles/` (exact bytes — do not re-pack after checksums were generated):

| File | SHA-256 |
|---|---|
| `ch55xduino-core-0.0.26.tar.bz2` | `435610ce367f6952f71beba25228d34b435dc15efe4d1dc46855a7f0034571b0` |
| `ch55xduino-tools_macosx-2026.07.10.tar.bz2` | `dd57aac57b551da76d78af8daa7af9243ea554237381c6638011ce98061a85a3` |
| `ch55xduino-tools_linux64-2026.07.10.tar.bz2` | `af976f6ad99f1aeaec87c17c058a329af02f2a642fdbf144a2380a119478cab4` |
| `ch55xduino-tools_aarch64-2026.07.10.tar.bz2` | `0cc8327eb53756d0239288902995705515e5e99138b94131b645fa614563306b` |
| `ch55xduino-tools_mingw32-2026.07.10.tar.bz2` | `df874247dfeb44b23aae3c0da813363bb156e802c4d29a3cba975027ee14bc63` |

## What's in this release

- **Core 0.0.26**: `platform.txt` typo fix for `recipe.preproc.includes`
- **MCS51Tools 2026.07.10**: `sdcc.sh` fix for Arduino CLI 1.5.x (`*.cpp.merged`, `re12` preproc)
- **sdcc**: unchanged (`build.13407_4`, no new tarball)

## After upload

1. Commit and push [`package_ch55xduino_mcs51_index.json`](../../package_ch55xduino_mcs51_index.json)
2. Smoke test:

```sh
arduino-cli core update-index
arduino-cli core install CH55xDuino:mcs51@0.0.26
arduino-cli compile --fqbn CH55xDuino:mcs51:ch552 path/to/Blink
```
