# Phase 23.5F - Download Artifact Hosting Fix

Date: 2026-06-04

## Objective

Fix public GitHub Pages download links so the ASTRA Audio Suite site uses GitHub Releases as the official artifact source.

## Diagnosis

`docs/index.html` and `docs/site/index.html` previously linked macOS downloads to:

```text
../installer/macOS/au-first-suite/output/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg
```

That path is outside the GitHub Pages `/docs` publishing root and returns 404 publicly.

Windows download links already used GitHub Releases and were confirmed working.

## Hosting Fix

The macOS AU-only suite package was uploaded to GitHub Release `v1.0.0`:

```text
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg
```

All public macOS buttons in `docs/index.html` and `docs/site/index.html` now point to this release asset.

Windows buttons remain on the existing VST3-first installers:

```text
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/AUREQ-1.0.0-Windows-Setup.exe
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/LUMINAR-1.0.0-Windows-Setup.exe
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/GRAVITY-1.0.0-Windows-Setup.exe
```

## Text Alignment

- macOS public/tester distribution is described as Audio Unit / AU-first.
- Windows public/tester distribution remains VST3-first.
- Standalone is not presented as a primary public product.
- Generic top-level copy that described the whole suite as "VST3" was replaced with AU macOS / VST3 Windows language.

## Files Changed

- `docs/index.html`
- `docs/site/index.html`
- `docs/beta/AUREQ-0.9.0-rc1-private-beta-instructions.md`
- `docs/beta/AUREQ-0.9.0-rc1-windows-beta-instructions.md`
- `docs/release/AUREQ-0.9.0-rc1-release-notes.md`
- `docs/release/AUREQ-macos-installation-guide.md`
- `docs/release/AUREQ-release-checklist.md`
- `installer/macOS/au-first-suite/resources/README.txt`

## Validation

Required checks:

```bash
rg "../installer/macOS|installer/macOS/au-first-suite/output" docs/index.html docs/site/index.html docs/beta docs/release installer/macOS/au-first-suite/resources/README.txt
rg "releases/download/v1.0.0" docs/index.html docs/site/index.html docs/beta docs/release installer/macOS/au-first-suite/resources/README.txt
curl -I -L "https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg"
curl -I -L "https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/AUREQ-1.0.0-Windows-Setup.exe"
curl -I -L "https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/LUMINAR-1.0.0-Windows-Setup.exe"
curl -I -L "https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/GRAVITY-1.0.0-Windows-Setup.exe"
```

Validation completed:

- No remaining `../installer/macOS` or `installer/macOS/au-first-suite/output` public links in the checked docs.
- GitHub Release links are present in both public HTML files and relevant beta/release docs.
- macOS AU-only suite link returns GitHub redirect followed by `HTTP/2 200`.
- AUREQ Windows VST3 link returns GitHub redirect followed by `HTTP/2 200`.
- LUMINAR Windows VST3 link returns GitHub redirect followed by `HTTP/2 200`.
- GRAVITY Windows VST3 link returns GitHub redirect followed by `HTTP/2 200`.

## Scope Guard

No plugin code, DSP, APVTS, UI, CMake, GitHub Actions workflow, Windows installer, installer script, build artifact, cache, receipt, or system installation was changed in this phase.

No commit or push was performed.

## Status

PASS.
