# Fase 15.2D — DAW Validation Manual Checklist

Data de criacao: 2026-05-28  
Plugin instalado observado: `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`  
Receipt observado: `br.com.sidyfurtado.aureq.vst3`  
Versao do receipt: `0.9.0`

## Status Permitidos

Use apenas um dos status abaixo em cada item:

| Status | Significado |
|--------|-------------|
| PASS | Validado sem problema observado |
| FAIL | Falha reproduzivel observada |
| PARTIAL | Validado parcialmente ou com ressalvas |
| BLOCKED | Nao foi possivel validar |

## Contexto Obrigatorio

- A Fase 15.2C-R1 ficou bloqueada porque `sudo installer` nao conseguiu autenticar nesta sessao.
- O bundle `AUREQ.vst3` ja estava instalado em `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
- A validacao manual em DAW pode seguir usando o plugin atualmente instalado.
- A validacao do PKG novo deve ser repetida manualmente no Terminal com senha administrativa:

```bash
sudo installer -pkg "installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg" -target /
```

## Checklist Geral

| Area | Item | Status | Evidencia / Observacoes |
|------|------|--------|--------------------------|
| Sistema | Confirmar que `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3` existe | BLOCKED | Preencher durante QA manual |
| Sistema | Confirmar receipt `br.com.sidyfurtado.aureq.vst3` version `0.9.0` | BLOCKED | Preencher durante QA manual |
| DAW | Abrir DAW sem crash | BLOCKED | Preencher durante QA manual |
| DAW | Executar rescan de plugins | BLOCKED | Preencher durante QA manual |
| DAW | Confirmar AUREQ listado como VST3 | BLOCKED | Preencher durante QA manual |
| DAW | Confirmar fabricante/nome visivel como AUREQ / ASTRA Audio, quando a DAW exibir | BLOCKED | Preencher durante QA manual |
| Instancia | Carregar AUREQ em uma track de audio | BLOCKED | Preencher durante QA manual |
| Instancia | Abrir UI do plugin | BLOCKED | Preencher durante QA manual |
| Audio | Passar audio pela track com AUREQ ativo | BLOCKED | Preencher durante QA manual |
| Audio | Confirmar bypass global sem crash e sem comportamento inesperado | BLOCKED | Preencher durante QA manual |
| Presets | Testar preset browser de fabrica | BLOCKED | Preencher durante QA manual |
| Presets | Testar navegacao previous/next de presets | BLOCKED | Preencher durante QA manual |
| Presets | Testar user presets: salvar preset | BLOCKED | Preencher durante QA manual |
| Presets | Testar user presets: recarregar preset salvo | BLOCKED | Preencher durante QA manual |
| A/B | Alternar A/B | BLOCKED | Preencher durante QA manual |
| A/B | Testar copy/reset actions de A/B, se acessiveis na DAW | BLOCKED | Preencher durante QA manual |
| Host | Testar Undo/Redo apos mudanca de parametro | BLOCKED | Preencher durante QA manual |
| Dynamic EQ | Ativar Dynamic EQ em uma banda | BLOCKED | Preencher durante QA manual |
| Dynamic EQ | Ajustar threshold/range/attack/release e confirmar resposta visual/sonora | BLOCKED | Preencher durante QA manual |
| Channel Mode | Testar Stereo | BLOCKED | Preencher durante QA manual |
| Channel Mode | Testar Left/Right ou Mid/Side, conforme disponivel na UI | BLOCKED | Preencher durante QA manual |
| Sessao | Salvar sessao/projeto com AUREQ carregado | BLOCKED | Preencher durante QA manual |
| Sessao | Fechar e reabrir sessao/projeto | BLOCKED | Preencher durante QA manual |
| Sessao | Confirmar restauracao de estado: parametros, preset atual, A/B quando aplicavel | BLOCKED | Preencher durante QA manual |
| Estabilidade | Observar crashes, hangs, glitches severos ou UI travada | BLOCKED | Preencher durante QA manual |
| Logs | Verificar logs/relatorios da DAW ou macOS se houver crash | BLOCKED | Preencher durante QA manual |

## Ableton Live

| Item | Status | Evidencia / Observacoes |
|------|--------|--------------------------|
| Abrir Ableton Live | BLOCKED | Preencher durante QA manual |
| Preferences > Plug-Ins > Rescan | BLOCKED | Preencher durante QA manual |
| Confirmar AUREQ como VST3 | BLOCKED | Preencher durante QA manual |
| Carregar AUREQ em Audio Track | BLOCKED | Preencher durante QA manual |
| Abrir UI | BLOCKED | Preencher durante QA manual |
| Passar audio | BLOCKED | Preencher durante QA manual |
| Testar preset browser | BLOCKED | Preencher durante QA manual |
| Testar user presets | BLOCKED | Preencher durante QA manual |
| Testar A/B | BLOCKED | Preencher durante QA manual |
| Testar Undo/Redo do host | BLOCKED | Preencher durante QA manual |
| Testar Dynamic EQ | BLOCKED | Preencher durante QA manual |
| Testar Channel Mode | BLOCKED | Preencher durante QA manual |
| Salvar/reabrir set | BLOCKED | Preencher durante QA manual |
| Observar crash/logs | BLOCKED | Preencher durante QA manual |

## Reaper

| Item | Status | Evidencia / Observacoes |
|------|--------|--------------------------|
| Abrir Reaper | BLOCKED | Preencher durante QA manual |
| Options > Preferences > Plug-ins > VST > Re-scan | BLOCKED | Preencher durante QA manual |
| Confirmar AUREQ como VST3 | BLOCKED | Preencher durante QA manual |
| Carregar AUREQ em track | BLOCKED | Preencher durante QA manual |
| Abrir UI | BLOCKED | Preencher durante QA manual |
| Passar audio | BLOCKED | Preencher durante QA manual |
| Testar preset browser | BLOCKED | Preencher durante QA manual |
| Testar user presets | BLOCKED | Preencher durante QA manual |
| Testar A/B | BLOCKED | Preencher durante QA manual |
| Testar Undo/Redo do host | BLOCKED | Preencher durante QA manual |
| Testar Dynamic EQ | BLOCKED | Preencher durante QA manual |
| Testar Channel Mode | BLOCKED | Preencher durante QA manual |
| Salvar/reabrir projeto | BLOCKED | Preencher durante QA manual |
| Observar crash/logs | BLOCKED | Preencher durante QA manual |

## Qualquer DAW Disponivel

| DAW | Versao | Item | Status | Evidencia / Observacoes |
|-----|-------|------|--------|--------------------------|
| Preencher | Preencher | Rescan de plugins | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Confirmar AUREQ como VST3 | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Carregar em track | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Abrir UI | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Passar audio | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Testar preset browser | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Testar user presets | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Testar A/B | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Testar Undo/Redo | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Testar Dynamic EQ | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Testar Channel Mode | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Salvar/reabrir sessao | BLOCKED | Preencher durante QA manual |
| Preencher | Preencher | Observar crash/logs | BLOCKED | Preencher durante QA manual |

## Resultado Final da Validacao Manual

Status final: `BLOCKED`

Resumo do avaliador:

```text
Preencher apos validacao manual em DAW.
```

Bloqueadores:

```text
Preencher se houver.
```
