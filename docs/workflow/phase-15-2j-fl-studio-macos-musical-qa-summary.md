# Fase 15.2J — FL Studio macOS Musical QA Pass

Data: 2026-05-28

## Objetivo

Preparar o passe de QA musical real do AUREQ no FL Studio macOS, sem alterar codigo, CMake, APVTS, IDs, DSP, analyzer FFT, presets, UI interna, PKG, DMG ou Livros/PDFs.

Esta fase e de QA manual/musical. Ela nao implementa feature nova, nao roda build, nao usa `sudo`, nao instala nada e nao limpa cache automaticamente.

## Contexto

A Fase 15.2I confirmou:

- AUREQ principal aparece no FL Studio como VST3.
- A UI abre corretamente.
- Vendor principal: `ASTRA Audio`.
- Duplicata fisica foi resolvida.
- `AUREQ_2` e provavel cache/entrada antiga do FL Studio.
- Gatekeeper segue `BLOCKED` ate Apple Developer ID + notarizacao.

## Checklist Criado

Arquivo:

```text
docs/workflow/aureq-fl-studio-musical-qa-checklist.md
```

O checklist cobre:

- Inserir AUREQ em mixer track.
- Passar audio limpo.
- Input/Output Gain.
- Tipos de filtro: Bell, LC, HC, LS, HS, NT, BP.
- Slopes 12/24/48.
- Channel Mode: ST, M, S, L, R.
- Dynamic EQ: Range, Threshold, Attack, Release.
- Preset Browser: `Genres > Trap > 808`, `Genres > Forro > Vocal`, `Genres > Rock > Electric Guitar`, `By Source`, `User Presets`.
- A/B.
- Undo/Redo.
- Salvar e reabrir projeto FL Studio.
- Bypass.
- Analyzer visual.
- Estabilidade sem crash.
- Observacoes musicais: presets agressivos, curvas estranhas, ganho alto, nomes confusos e comportamentos inesperados.

## Status

Status documental: **CONCLUIDA**.

Status do QA musical final: **BLOCKED / aguardando validacao humana**.

## Politica Sobre AUREQ_2

Nao limpar cache automaticamente.

Enquanto nao houver duplicata fisica, `AUREQ_2` deve ser tratado como cache/entrada antiga do FL Studio. A recomendacao continua sendo manter favorito apenas no AUREQ correto e remover/desfavoritar `AUREQ_2` no Plugin Manager.

## Integridade de Escopo

Nao foi feito:

- Nenhuma edicao de codigo.
- Nenhuma edicao de CMake.
- Nenhuma alteracao de APVTS, IDs, DSP, analyzer FFT, presets ou UI interna.
- Nenhuma alteracao de PKG ou DMG.
- Nenhuma alteracao em Livros/PDFs.
- Nenhum build.
- Nenhum `sudo`.
- Nenhuma instalacao.
- Nenhuma remocao.
- Nenhuma limpeza automatica de cache.

## Proximo Passo

Executar o checklist musical no FL Studio macOS e preencher os status `PASS`, `PARTIAL`, `FAIL` ou `BLOCKED` com observacoes musicais.
