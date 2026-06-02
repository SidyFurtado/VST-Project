# Phase 19.5 - GRAVITY CI/CD and Installer Integration Summary

Date: 2026-06-02

## Summary

Integrated GRAVITY into the official ASTRA Suite build workflow as a third product parallel to AUREQ and LUMINAR, with dedicated macOS and Windows installer definitions.

## CI/CD Changes

- Updated `.github/workflows/astra-suite-build.yml`.
- Added `GRAVITY_All` to macOS CMake build targets.
- Added `GRAVITY_All` to Windows CMake build targets.
- Added macOS staging for `GRAVITY.vst3` and `GRAVITY.app`.
- Added Windows staging for `GRAVITY.vst3` and `GRAVITY.exe`.
- Added macOS package generation for GRAVITY.
- Added Windows Inno Setup compilation for GRAVITY.
- Added artifact upload steps:
  - `GRAVITY-${VERSION}-macOS`
  - `GRAVITY-${VERSION}-Windows`

## macOS Installer

- Added `installer/macOS/gravity/distribution/Distribution.xml`.
- Added `installer/macOS/gravity/resources/welcome.html`.
- Added `installer/macOS/gravity/resources/license.html`.
- Package identifiers:
  - `br.com.sidyfurtado.gravity.vst3`
  - `br.com.sidyfurtado.gravity.standalone`

## Windows Installer

- Added `installer/Windows/gravity/scripts/GRAVITY-installer.iss`.
- Added unique Inno Setup `AppId`:
  - `{4F6CF39D-83DE-4A4D-8E4A-6A4A5F0C2B17}`
- Installer destinations:
  - `C:\Program Files\Common Files\VST3\GRAVITY.vst3`
  - `C:\Program Files\ASTRA Audio\GRAVITY`

## Scope Guard

- Did not modify `.github/workflows/windows-build-aureq.yml`.
- Did not modify GRAVITY DSP.
- Did not run terminal commands or build validation.

## Build Validation

Build was intentionally not run in this step. Awaiting user command.
