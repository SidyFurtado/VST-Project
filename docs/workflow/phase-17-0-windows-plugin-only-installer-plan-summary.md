# Fase 17.0 — Windows Plugin-Only Installer Plan

Data: 2026-05-29

## Objetivo

Preparar a estrutura inicial do instalador Windows VST3-only do AUREQ 0.9.0-rc1, usando Inno Setup como ferramenta principal, sem gerar `.exe` final e sem usar artefato macOS como payload Windows.

## Estrategia Windows

- O build Windows deve ser feito em Windows real, VM aprovada ou CI runner Windows.
- O payload precisa ser um `AUREQ.vst3` Windows real.
- O artefato macOS `AUREQ.vst3` nao deve ser usado como payload Windows.
- O instalador final `.exe` fica bloqueado ate existir build Windows real e ambiente Inno Setup.
- O release publico Windows fica bloqueado ate code signing Windows para reduzir alertas SmartScreen.

## Estrutura Criada

```text
installer/Windows/plugin-only/
  scripts/AUREQ-plugin-only.iss
  resources/README-Windows.txt
  staging/.gitkeep
  staging/VST3/
  output/.gitkeep
```

## Script Inno Setup

O script `installer/Windows/plugin-only/scripts/AUREQ-plugin-only.iss` preve:

- `AppName=AUREQ`
- `AppVersion=0.9.0`
- `AppPublisher=ASTRA Audio`
- `DefaultDirName={commoncf}\VST3`
- `DisableDirPage=yes`
- `OutputDir=..\output`
- `OutputBaseFilename=AUREQ-0.9.0-rc1-Windows-plugin-only-setup`
- `SetupIconFile=..\resources\AUREQ.ico` somente se o `.ico` existir.
- `Source: "..\staging\VST3\AUREQ.vst3"`
- `DestDir: "{commoncf}\VST3"`
- Uninstaller padrao do Inno Setup.
- Mensagens informando que o VST3 sera instalado no caminho padrao.

## Destino Obrigatorio

```text
C:\Program Files\Common Files\VST3\AUREQ.vst3
```

No Inno Setup:

```text
{commoncf}\VST3\AUREQ.vst3
```

## Icone

`installer/Windows/plugin-only/resources/AUREQ.ico` ficou pendente nesta fase.

Motivo: nao foi forcada geracao de `.ico` sem validacao confiavel. O script Inno Setup usa `SetupIconFile` de forma condicional se o arquivo existir.

## Payload Windows

Nao ha payload Windows nesta fase.

`installer/Windows/plugin-only/staging/VST3/` permanece vazio ate existir um `AUREQ.vst3` Windows real.

## EXE Final

Nao foi gerado `.exe` final nesta fase.

O output esperado futuro sera:

```text
installer/Windows/plugin-only/output/AUREQ-0.9.0-rc1-Windows-plugin-only-setup.exe
```

## Uninstaller

O uninstaller padrao do Inno Setup esta previsto.

Ele deve remover o VST3 instalado em:

```text
C:\Program Files\Common Files\VST3\AUREQ.vst3
```

User presets nao devem ser removidos automaticamente nesta fase.

## Validacao Estatica

- Estrutura Windows plugin-only criada.
- Script `.iss` criado.
- Destino `{commoncf}\VST3` registrado.
- `DisableDirPage=yes` registrado.
- Uninstaller previsto pelo Inno Setup.
- Nenhum `.exe` final criado.
- Nenhum payload macOS copiado para staging Windows.
- Bloqueios de Windows build, Inno Setup real e code signing documentados.

## Escopo Preservado

Nao foram alterados:

- Codigo C++.
- CMake.
- APVTS.
- IDs.
- DSP.
- Analyzer.
- Presets.
- UI interna.
- macOS PKG/DMG.
- Livros/PDFs.
