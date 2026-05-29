# Fase 15.2H — macOS Release Hardening & DAW Validation Pass

Data: 2026-05-28

## Objetivo

Consolidar o estado macOS do AUREQ antes de avancar para Windows, cobrindo instalacao, duplicatas, metadata, Gatekeeper, DMG UX e checklist real de validacao em DAWs.

Esta fase nao altera codigo, CMake, APVTS, IDs, DSP, analyzer FFT, presets, UI interna, PKG, plugin binary, Livros ou PDFs. Nao usa `sudo`, nao executa o PKG e nao instala nada.

## Duplicatas

Comando executado:

```bash
find "/Library/Audio/Plug-Ins/VST3" "$HOME/Library/Audio/Plug-Ins/VST3" -maxdepth 1 \( -name "AUREQ.vst3" -o -name "AUREQ_2.vst3" \) -print
```

Resultado:

```text
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

Status: **PASS**.

Interpretacao:

- Existe apenas o bundle system-level esperado.
- Nao foi encontrada duplicata fisica em `~/Library/Audio/Plug-Ins/VST3`.
- Nao foi encontrado `AUREQ_2.vst3` em `/Library` ou `~/Library`.
- Se FL Studio ainda mostrar `AUREQ_2`, tratar como possivel cache/registro interno da DAW ate prova contraria.

## Metadata

`Info.plist`:

```text
CFBundleIdentifier = br.com.sidyfurtado.aureq
CFBundleDisplayName = AUREQ
CFBundleName = AUREQ
CFBundleShortVersionString = 0.9.0
CFBundleVersion = 0.9.0
```

Status: **PASS**.

`moduleinfo.json`:

```text
Name = AUREQ
Version = 0.9.0
Factory Info / Vendor = ASTRA Audio
Classes[0] / Vendor = ASTRA Audio
Classes[1] / Vendor = ASTRA Audio
```

Status: **PASS**.

Ocorrencia de `Apple`:

```text
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
```

Interpretacao: **PASS**. A ocorrencia de `Apple` e apenas o DTD padrao do plist, nao vendor do plugin.

## Gatekeeper / Signing

`codesign -dv --verbose=4`:

```text
Identifier=br.com.sidyfurtado.aureq
Format=bundle with Mach-O thin (arm64)
Signature=adhoc
TeamIdentifier=not set
```

`spctl -a -vvv`:

```text
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3: internal error in Code Signing subsystem
```

`xattr -l`:

```text
com.apple.provenance
```

Status: **BLOCKED** ate Apple Developer ID + notarizacao.

Interpretacao:

- A build atual usa assinatura ad-hoc e nao possui TeamIdentifier.
- O estado e esperado para RC interno nao assinado/notarizado.
- `xattr` pode ser workaround local de beta, mas nao e solucao de produto final.
- Release publico precisa Developer ID, notarizacao e validacao Gatekeeper final.

## DMG UX

DMG validado:

```text
installer/macOS/dmg/output/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg
```

Montagem:

```text
/Volumes/AUREQ 0.9.0-rc1 Installer
```

Conteudo verificado:

```text
AUREQ Installer.pkg
README - Install AUREQ.txt
AUREQ.icns
.background/aureq-dmg-background.png
```

Status: **PASS funcional / PARTIAL visual polish**.

Interpretacao:

- O DMG monta e contem os artefatos esperados.
- O PKG nao foi executado.
- Nada foi instalado no sistema.
- O background asset esta presente, mas o polish de janela Finder/background automatico permanece parcial conforme Fase 15.2G.

## DAW Validation

Checklist criado:

```text
docs/workflow/aureq-macos-daw-validation-checklist.md
```

Status FL Studio: **BLOCKED / aguardando validacao humana**.

Observacao FL Studio:

- Como nao ha duplicata fisica no filesystem, qualquer `AUREQ_2` visto no FL Studio deve ser tratado primeiro como possivel cache/registro interno da DAW.
- Nao remover arquivos sem aprovacao explicita.

Status Ableton Live: **BLOCKED / aguardando validacao humana**.

Status Reaper: **BLOCKED / aguardando validacao humana**.

Status Logic Pro: **BLOCKED / futuro AU ou wrapper**.

Status Studio One / Cubase / Bitwig: **BLOCKED / validacao futura VST3**.

## Integridade de Escopo

Nao foi feito:

- Nenhuma edicao de codigo.
- Nenhuma edicao de CMake.
- Nenhuma alteracao de APVTS ou IDs.
- Nenhuma alteracao de DSP, analyzer FFT, presets ou UI interna.
- Nenhuma remocao em `/Library`.
- Nenhuma remocao em `~/Library`.
- Nenhum `sudo`.
- Nenhuma execucao do PKG.
- Nenhuma instalacao.
- Nenhuma assinatura/notarizacao.
- Nenhum Windows installer.
- Nenhum build.

## Proximos Passos

1. Completar validacao humana no FL Studio usando o checklist macOS.
2. Se FL Studio mostrar duplicata sem duplicata fisica, limpar/rescan/verify pelo Plugin Manager antes de tocar em arquivos.
3. Validar Ableton Live e Reaper quando disponiveis.
4. Resolver assinatura/notarizacao quando houver Apple Developer ID.
5. Adiar Windows installer ate macOS estar mais firme.
