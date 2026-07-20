# Phase 23.5A - AU Format Validation / Manufacturer Code Fix

Date: 2026-06-04

## Objective

Correct the Audio Unit identity for the three active ASTRA Suite plugins before public macOS AU-first distribution.

The previous CMake argument was:

```cmake
MANUFACTURER_CODE AStr
```

JUCE CMake expects:

```cmake
PLUGIN_MANUFACTURER_CODE AStr
```

Because the old argument was not recognized by `juce_add_plugin`, JUCE used its default manufacturer code `Manu` in generated AU metadata.

## Plugins

| Plugin | AU Type | Subtype | Previous Manufacturer | Correct Manufacturer |
|---|---:|---:|---:|---:|
| AUREQ | `aufx` | `Aurq` | `Manu` | `AStr` |
| LUMINAR | `aufx` | `Lmnr` | `Manu` | `AStr` |
| GRAVITY | `aufx` | `Grvt` | `Manu` | `AStr` |

## Files Changed

- `plugins/01_EQ/CMakeLists.txt`
- `plugins/02_AIR/CMakeLists.txt`
- `plugins/03_GRAVITY/CMakeLists.txt`
- `plugins/01_EQ/Docs/implementation-plan.md`
- `plugins/02_AIR/Docs/implementation-plan.md`
- `plugins/03_GRAVITY/Docs/implementation-plan.md`

## Scope Guard

No DSP, APVTS, parameter IDs, presets, UI, installers, GitHub Actions, bundle IDs, plugin codes, product names, or versions were changed.

## Validation Result

Executed successfully:

```bash
cmake --build build
./build/tests/AUREQ_FilterTests
```

Build result: PASS.

Test result: PASS. `AUREQ_FilterTests` completed all DSP filter validation suites successfully.

AU components to confirm:

- `build/plugins/01_EQ/AUREQ_artefacts/AU/AUREQ.component`
- `build/plugins/02_AIR/LUMINAR_artefacts/AU/LUMINAR.component`
- `build/plugins/03_GRAVITY/GRAVITY_artefacts/AU/GRAVITY.component`

All three AU components exist.

## Metadata Confirmed

`Info.plist` inspection confirmed:

| Plugin | AU Type | Subtype | Manufacturer |
|---|---:|---:|---:|
| AUREQ | `aufx` | `Aurq` | `AStr` |
| LUMINAR | `aufx` | `Lmnr` | `AStr` |
| GRAVITY | `aufx` | `Grvt` | `AStr` |

Generated JUCE metadata also confirms:

```text
JucePlugin_ManufacturerCode=0x41537472
```

`0x41537472` corresponds to `AStr`.

## Future auval Commands

After installing/registering the rebuilt AU components in a future validation pass:

```bash
auval -v aufx Aurq AStr
auval -v aufx Lmnr AStr
auval -v aufx Grvt AStr
```

The old `Manu` commands were only diagnostic commands for the incorrect pre-fix state:

```bash
auval -v aufx Aurq Manu
auval -v aufx Lmnr Manu
auval -v aufx Grvt Manu
```

## Next Recommended Phase

Phase 23.5B - macOS AU-First Installer Plan.
