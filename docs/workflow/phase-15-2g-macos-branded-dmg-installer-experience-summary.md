# Fase 15.2G — macOS Branded DMG Installer Experience

Data: 2026-05-28

## Objetivo

Criar uma embalagem visual premium em formato `.dmg` para distribuir o instalador plugin-only do AUREQ no macOS, mantendo o `.pkg` como motor real de instalacao.

Esta fase e apenas de distribuicao visual. Ela nao executa o `.pkg`, nao instala nada no sistema, nao usa `sudo`, nao assina e nao notariza.

## Artefato Gerado

DMG final:

```text
installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg
```

Volume planejado:

```text
AUREQ 0.9.0-rc1 Installer
```

## Conteudo do DMG

```text
AUREQ Installer.pkg
README - Install AUREQ.txt
AUREQ.icns
.background/aureq-dmg-background.png
```

O arquivo `AUREQ Installer.pkg` e uma copia renomeada de:

```text
installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg
```

O PKG fonte nao foi alterado.

## Ferramenta

Ferramenta principal:

```text
hdiutil
```

Polimento visual best effort:

```text
osascript / Finder AppleScript
```

O script de geracao fica em:

```text
installer/macOS/dmg/scripts/create-dmg.sh
```

Resultado do polish Finder: **best effort parcial**.

O DMG read-write foi montado e o AppleScript tentou configurar a janela Finder, mas falhou ao aplicar o background como `background picture`:

```text
Finder polish failed; continuing with functional DMG.
```

O DMG funcional foi gerado corretamente e contem o background asset em `.background/aureq-dmg-background.png`. O polish avancado da janela Finder fica para fase futura caso seja necessario gerar uma arte dedicada de DMG e ajustar metadados Finder com mais controle.

## README Incluido

O README e bilingue PT-BR/EN e informa que o instalador coloca o VST3 em:

```text
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

Tambem informa que esta build RC ainda nao esta assinada nem notarizada.

## Validacao

Validacao executada sem instalar:

```bash
hdiutil attach "installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg"
ls -la "/Volumes/AUREQ 0.9.0-rc1 Installer"
ls -la "/Volumes/AUREQ 0.9.0-rc1 Installer/.background"
hdiutil detach "/Volumes/AUREQ 0.9.0-rc1 Installer"
```

Resultado da montagem:

```text
/Volumes/AUREQ 0.9.0-rc1 Installer
```

Conteudo verificado no volume:

```text
AUREQ Installer.pkg
AUREQ.icns
README - Install AUREQ.txt
.background/aureq-dmg-background.png
```

Tamanho do DMG final:

```text
9.4M
```

Formato do DMG final:

```text
UDIF somente leitura comprimido (zlib)
```

Integridade do PKG:

```text
PKG source and staged copy match
```

Nao executar:

```bash
installer -pkg ...
```

## Integridade de Escopo

Nao foram alterados:

- Codigo C++.
- CMake.
- APVTS.
- IDs de parametros.
- DSP.
- Analyzer FFT.
- Presets.
- UI interna do plugin.
- PKG fonte.
- Livros/PDFs.

Nao foi usado `sudo`.
Nao foi executado installer.
Nao houve instalacao em `/Library`.
Nao houve instalacao em `/Applications`.

## Fora de Escopo

Assinatura/notarizacao ficam para fase futura:

- Assinar o PKG com Developer ID Installer.
- Assinar o DMG, se adotado no fluxo final.
- Submeter com `xcrun notarytool`.
- Aplicar `xcrun stapler`.
- Validar Gatekeeper final.

Windows installer fica para fase futura:

- Inno Setup ou NSIS.
- Destino `C:\Program Files\Common Files\VST3\AUREQ.vst3`.
- Uninstaller Windows.
- Code signing Windows.
