# Fase 16.3 — Refresh macOS Beta Installer With Latest Build

Data: 2026-05-29

## Objetivo

Atualizar os artefatos beta macOS (`.pkg` e `.dmg`) usando o VST3 mais recente gerado pelo build atual, sem alterar codigo, APVTS, IDs, DSP, analyzer, presets, UI, CMake, installers de sistema ou Livros/PDFs.

Esta fase nao usa `sudo`, nao executa o PKG, nao instala nada em `/Library`, nao assina e nao notariza.

## Build E Testes

```text
cmake --build build — PASS
./build/tests/AUREQ_FilterTests — PASS
```

## VST3 Usado

Origem:

```text
build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3
```

Binario usado:

```text
build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3/Contents/MacOS/AUREQ
```

Timestamp do binario:

```text
May 29 18:45:34 2026
```

O payload local foi atualizado em:

```text
installer/macOS/plugin-only/payload/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

## PKG Regenerado

Component package:

```text
installer/macOS/plugin-only/output/AUREQ-VST3-component.pkg
```

Timestamp:

```text
May 29 19:16:21 2026
```

Product package:

```text
installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg
```

Timestamp:

```text
May 29 19:16:26 2026
```

Inspecao nao destrutiva:

```text
pkgutil --expand — PASS
lsbom — PASS
```

O BOM confirmou payload em:

```text
Library/Audio/Plug-Ins/VST3/AUREQ.vst3
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/MacOS/AUREQ
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Resources/moduleinfo.json
```

## DMG Regenerado

DMG final:

```text
installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg
```

Timestamp:

```text
May 29 19:17:26 2026
```

O PKG do staging:

```text
installer/macOS/dmg/staging/AUREQ Installer.pkg
```

foi confirmado identico ao product package regenerado.

## DMG Validation

Montagem:

```text
/Volumes/AUREQ 0.9.0-rc1 Installer
```

Conteudo confirmado:

```text
AUREQ Installer.pkg
README - Install AUREQ.txt
AUREQ.icns
.background/aureq-dmg-background.png
```

Desmontagem:

```text
hdiutil detach — PASS
```

Finder polish:

```text
Finder polish failed; continuing with functional DMG.
```

Resultado: DMG funcional regenerado. O polish visual avancado da janela Finder continua best effort e nao bloqueia esta fase.

## Beta Status

O pacote continua:

```text
unsigned / not notarized
```

Isto e esperado para beta local. Release publica permanece bloqueada por Apple Developer ID + notarizacao.

## Integridade De Escopo

Nao foi feito:

- Nenhuma alteracao de codigo C++.
- Nenhuma alteracao de APVTS, IDs, DSP, analyzer logic, presets ou UI.
- Nenhuma alteracao de CMake.
- Nenhuma alteracao em Livros/PDFs.
- Nenhuma execucao do PKG.
- Nenhuma instalacao em `/Library`.
- Nenhum `sudo`.
- Nenhuma assinatura/notarizacao.
- Nenhum Windows installer.
