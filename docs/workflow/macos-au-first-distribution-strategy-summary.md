# macOS AU-First Distribution Strategy — ASTRA Audio Suite

Data: 2026-06-04

## Objetivo

Registrar a decisao estrategica de distribuicao da ASTRA Audio Suite no macOS:

- **macOS:** Audio Unit (AU) passa a ser o formato principal de distribuicao publica.
- **Windows:** VST3 continua sendo o formato principal de distribuicao publica.

Esta decisao e documental/estrategica. Nenhum codigo, DSP, APVTS, preset, UI ou installer foi alterado nesta etapa.

## Motivos

- Logic Pro nao carrega VST3 nativamente; a validacao e distribuicao para Logic devem ser feitas via AU.
- VST3 no macOS apresentou travamento/incompatibilidade em programa de edicao de video.
- AU e o formato nativo/principal no ecossistema Apple para DAWs como Logic Pro e GarageBand.
- VST3 continua importante para desenvolvimento, testes e DAWs macOS compativeis, mas nao deve ser o foco do installer publico no macOS.

## Estado Confirmado No CMake

Os plugins principais da suite ja declaram suporte a AU no CMake:

- AUREQ: `plugins/01_EQ/CMakeLists.txt`
  - `FORMATS VST3 AU Standalone`
- LUMINAR: `plugins/02_AIR/CMakeLists.txt`
  - `FORMATS VST3 AU Standalone`

Conclusao: nao ha necessidade imediata de alterar o CMake de AUREQ ou LUMINAR para habilitar AU. A mudanca necessaria e de estrategia de release, documentacao, staging e packaging.

## Nova Estrategia Por Plataforma

### macOS

Formato principal:

```text
Audio Unit (AU)
```

Destino system-level recomendado para installer publico:

```text
/Library/Audio/Plug-Ins/Components/
```

Artefatos esperados:

```text
/Library/Audio/Plug-Ins/Components/AUREQ.component
/Library/Audio/Plug-Ins/Components/LUMINAR.component
```

Destino user-level alternativo para instalacao manual:

```text
~/Library/Audio/Plug-Ins/Components/
```

VST3 macOS pode continuar existindo para desenvolvimento/testes:

```text
/Library/Audio/Plug-Ins/VST3/
```

Mas o instalador publico macOS deve priorizar AU.

### Windows

Formato principal:

```text
VST3
```

Destino do installer Windows:

```text
C:\Program Files\Common Files\VST3\
```

Nao ha mudanca proposta para o installer Windows nesta decisao.

## Impacto Em Produtos

### AUREQ

- macOS: `AUREQ.component` deve virar o payload principal do installer publico.
- macOS VST3: manter apenas como formato secundario/dev/test ate nova aprovacao.
- Windows: `AUREQ.vst3` continua formato principal.

### LUMINAR

- macOS: `LUMINAR.component` deve virar o payload principal do installer publico futuro.
- macOS VST3: manter apenas como formato secundario/dev/test ate nova aprovacao.
- Windows: `LUMINAR.vst3` continua formato principal.

## Documentos Que Devem Ser Atualizados Com Aprovacao

### `docs/release/AUREQ-macos-installation-guide.md`

Trocar o foco de instalacao macOS de VST3 para AU:

- Pre-requisitos devem citar Logic Pro/GarageBand via AU.
- Secao principal deve ser "Instalacao do Plugin AU".
- Caminho principal deve virar `/Library/Audio/Plug-Ins/Components/AUREQ.component`.
- Caminho user-level alternativo deve virar `~/Library/Audio/Plug-Ins/Components/AUREQ.component`.
- VST3 deve ser movido para secao secundaria: desenvolvimento/testes/DAWs que aceitam VST3.
- A nota sobre Logic deve mudar de "validacao futura via AU" para "Logic Pro exige AU; use AUREQ.component".

### `docs/release/AUREQ-release-checklist.md`

Atualizar os blocos de release macOS:

- Build & QA deve incluir artefato AU: `build/.../AU/AUREQ.component`.
- Staging macOS deve incluir pasta `macOS/AU/`.
- Installer macOS deve trocar payload principal para `/Library/Audio/Plug-Ins/Components/AUREQ.component`.
- Signing/notarizacao deve incluir o `.component`.
- VST3 macOS deve ser reclassificado como secundario/dev/test, nao como foco publico.
- Windows deve permanecer VST3-first sem alteracao de destino.

### `plugins/01_EQ/Docs/implementation-plan.md`

Adicionar uma fase documental/estrategica apos a fase AU existente:

- Registrar que AUREQ no macOS passa a ser AU-first.
- Registrar que VST3 macOS permanece disponivel, mas secundario.
- Registrar que nenhuma alteracao de DSP/APVTS/presets/UI foi feita.
- Registrar proximas acoes: staging AU, PKG AU, DMG AU-first, validacao em Logic Pro.

### `plugins/02_AIR/Docs/implementation-plan.md`

Atualizar a direcao de distribuicao do LUMINAR:

- Remover/aposentar a premissa de "produto principal continua sendo VST3" para macOS.
- Registrar LUMINAR macOS AU-first.
- Manter Windows VST3-first.
- Registrar que nenhum DSP/APVTS/UI/installer foi alterado nesta etapa.

## Packaging macOS Futuro

Installer publico macOS deve instalar AU em:

```text
/Library/Audio/Plug-Ins/Components/
```

Payload AUREQ:

```text
AUREQ.component
```

Payload LUMINAR futuro:

```text
LUMINAR.component
```

Receipts/identificadores sugeridos:

```text
br.com.sidyfurtado.aureq.au
br.com.sidyfurtado.luminar.au
```

O VST3 macOS nao deve ser removido sem aprovacao explicita. Se incluido no installer futuro, deve ser opcional/secundario; se o objetivo for reduzir risco publico, o pacote publico pode ser AU-only e manter VST3 apenas nos builds internos.

## Validacao Recomendada

Antes de qualquer release AU-first:

```bash
cmake --build build
./build/tests/AUREQ_FilterTests
```

Validacoes macOS AU:

- Confirmar existencia de `AUREQ.component` no build.
- Instalar/copy em `/Library/Audio/Plug-Ins/Components/`.
- Validar scan em Logic Pro.
- Validar scan em GarageBand, se disponivel.
- Validar que AUREQ carrega, abre UI e passa audio.
- Validar presets, A/B, analyzer, Dynamic EQ e automacao basica.
- Confirmar ausencia de crash no host que teve incompatibilidade com VST3, quando AU for suportado por ele.

Validacoes Windows:

- Confirmar que o installer Windows continua usando `C:\Program Files\Common Files\VST3\`.
- Confirmar que nenhuma mudanca AU macOS afetou o fluxo Windows.

## Fora De Escopo Nesta Etapa

- Remover VST3 macOS.
- Alterar CMake.
- Alterar DSP.
- Alterar APVTS ou IDs de parametros.
- Alterar presets.
- Alterar UI.
- Alterar installers existentes.
- Alterar installer Windows.
- Mexer em payloads instalados no sistema.

## Decisao Registrada

AU passa a ser o formato principal da ASTRA Audio Suite no macOS.

VST3 continua sendo o formato principal no Windows.

VST3 macOS permanece permitido para desenvolvimento/testes, mas deixa de ser o foco do installer publico macOS.
