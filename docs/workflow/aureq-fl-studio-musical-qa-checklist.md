# AUREQ — FL Studio macOS Musical QA Checklist

Data de criacao: 2026-05-28  
Host: FL Studio macOS  
Plugin esperado: `AUREQ` como VST3  
Status permitidos: `PASS`, `PARTIAL`, `FAIL`, `BLOCKED`

## Contexto

A Fase 15.2I confirmou que o AUREQ principal aparece no FL Studio como VST3, abre corretamente, carrega a UI e mostra vendor principal `ASTRA Audio`.

O `AUREQ_2` remanescente deve ser tratado como provavel cache/entrada antiga do FL Studio, nao como duplicata fisica, ate prova contraria.

## Setup

| Item | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| Inserir AUREQ em mixer track | BLOCKED | Aguardando QA musical humano |
| Passar audio limpo pelo mixer track | BLOCKED | Aguardando QA musical humano |
| Confirmar ausencia de crash ao abrir/fechar UI | BLOCKED | Aguardando QA musical humano |
| Confirmar que o AUREQ correto, nao `AUREQ_2`, foi usado | BLOCKED | Manter favorito apenas no AUREQ correto |

## Gain Staging

| Item | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| Testar Input Gain em corte moderado | BLOCKED | Aguardando QA musical humano |
| Testar Input Gain em boost moderado | BLOCKED | Aguardando QA musical humano |
| Testar Output Gain em corte moderado | BLOCKED | Aguardando QA musical humano |
| Testar Output Gain em boost moderado | BLOCKED | Aguardando QA musical humano |
| Confirmar que nao ha clipping inesperado em ajustes normais | BLOCKED | Aguardando QA musical humano |

## Filter Types

| Tipo | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| Bell | BLOCKED | Aguardando QA musical humano |
| LC | BLOCKED | Low Cut / High-pass |
| HC | BLOCKED | High Cut / Low-pass |
| LS | BLOCKED | Low Shelf |
| HS | BLOCKED | High Shelf |
| NT | BLOCKED | Notch |
| BP | BLOCKED | Band Pass |

## Slopes

| Slope | Status | Observacoes Musicais / Tecnicas |
|-------|--------|----------------------------------|
| 12 dB/oct | BLOCKED | Aguardando QA musical humano |
| 24 dB/oct | BLOCKED | Aguardando QA musical humano |
| 48 dB/oct | BLOCKED | Aguardando QA musical humano |

## Channel Mode

| Modo | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| ST | BLOCKED | Stereo |
| M | BLOCKED | Mid |
| S | BLOCKED | Side |
| L | BLOCKED | Left |
| R | BLOCKED | Right |

## Dynamic EQ

| Item | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| Range | BLOCKED | Aguardando QA musical humano |
| Threshold | BLOCKED | Aguardando QA musical humano |
| Attack | BLOCKED | Aguardando QA musical humano |
| Release | BLOCKED | Aguardando QA musical humano |
| Confirmar resposta dinamica audivel/visual coerente | BLOCKED | Aguardando QA musical humano |
| Confirmar ausencia de pumping/artefato inesperado em ajuste moderado | BLOCKED | Aguardando QA musical humano |

## Preset Browser

| Area | Preset/Fluxo | Status | Observacoes Musicais / Tecnicas |
|------|--------------|--------|----------------------------------|
| Genres | Trap > 808 | BLOCKED | Observar grave, clipping, agressividade e utilidade musical |
| Genres | Forro > Vocal | BLOCKED | Observar clareza, nasalidade, sibilancia e ganho |
| Genres | Rock > Electric Guitar | BLOCKED | Observar medio, brilho, harshness e ganho |
| By Source | Navegar por fonte | BLOCKED | Confirmar organizacao e nomes claros |
| User Presets | Salvar preset de usuario | BLOCKED | Aguardando QA musical humano |
| User Presets | Recarregar preset de usuario | BLOCKED | Aguardando QA musical humano |
| User Presets | Confirmar persistencia apos reabrir UI | BLOCKED | Aguardando QA musical humano |

## Workflow Controls

| Item | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| A/B alterna estados sem crash | BLOCKED | Aguardando QA musical humano |
| A/B e musicalmente util para comparacao | BLOCKED | Aguardando QA musical humano |
| Undo apos mudanca de parametro | BLOCKED | Aguardando QA musical humano |
| Redo apos Undo | BLOCKED | Aguardando QA musical humano |
| Bypass global | BLOCKED | Aguardando QA musical humano |
| Salvar projeto FL Studio com AUREQ | BLOCKED | Aguardando QA musical humano |
| Reabrir projeto FL Studio e restaurar estado | BLOCKED | Aguardando QA musical humano |

## Analyzer Visual

| Item | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| Analyzer responde ao audio | BLOCKED | Aguardando QA musical humano |
| Analyzer nao trava a UI | BLOCKED | Aguardando QA musical humano |
| Analyzer permanece legivel durante mudancas de EQ | BLOCKED | Aguardando QA musical humano |

## Estabilidade

| Item | Status | Observacoes Musicais / Tecnicas |
|------|--------|----------------------------------|
| 10 minutos de uso sem crash | BLOCKED | Aguardando QA musical humano |
| Troca de presets sem crash | BLOCKED | Aguardando QA musical humano |
| Mudancas rapidas de parametros sem crash | BLOCKED | Aguardando QA musical humano |
| Abrir/fechar UI repetidamente sem crash | BLOCKED | Aguardando QA musical humano |
| Nenhum glitch severo observado | BLOCKED | Aguardando QA musical humano |

## Observacoes Musicais

Registrar qualquer comportamento inesperado:

```text
- Presets agressivos demais:
- Curvas estranhas:
- Ganho alto demais:
- Nomes confusos:
- Comportamento inesperado:
- Crash/logs:
- Observacoes gerais de mix/master:
```

## Resultado Final

Status final: `BLOCKED`

Resumo do avaliador:

```text
Aguardando QA musical humano no FL Studio macOS.
```
