# AUREQ — macOS DAW Validation Checklist

Data de criacao: 2026-05-28  
Plugin instalado esperado: `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`  
Status permitidos: `PASS`, `PARTIAL`, `BLOCKED`, `FAIL`

## Estado Base

| Area | Item | Status | Evidencia / Observacoes |
|------|------|--------|--------------------------|
| Instalacao | Existe `/Library/Audio/Plug-Ins/VST3/AUREQ.vst3` | PASS | Confirmado por `find` |
| Duplicatas | Nao existe `~/Library/Audio/Plug-Ins/VST3/AUREQ.vst3` | PASS | `find` retornou apenas o bundle system-level |
| Duplicatas | Nao existe `/Library/Audio/Plug-Ins/VST3/AUREQ_2.vst3` | PASS | `find` retornou apenas `AUREQ.vst3` |
| Duplicatas | Nao existe `~/Library/Audio/Plug-Ins/VST3/AUREQ_2.vst3` | PASS | `find` retornou apenas `AUREQ.vst3` |
| Metadata | Bundle ID `br.com.sidyfurtado.aureq` | PASS | Confirmado via `plutil` |
| Metadata | Nome/display name `AUREQ` | PASS | Confirmado via `plutil` |
| Metadata | Versao `0.9.0` | PASS | Confirmado via `plutil` e `moduleinfo.json` |
| Metadata | Vendor `ASTRA Audio` | PASS | Confirmado via `moduleinfo.json` |
| Gatekeeper | Developer ID / notarizacao | BLOCKED | Assinatura ad-hoc, sem TeamIdentifier |

## FL Studio macOS

| Item | Status | Evidencia / Observacoes |
|------|--------|--------------------------|
| Abrir FL Studio sem crash | PASS | Usuario testou AUREQ no FL Studio macOS |
| Abrir Plugin Manager | PASS | Usuario observou AUREQ e AUREQ_2 no FL Studio |
| Confirmar path `/Library/Audio/Plug-Ins/VST3` habilitado | PASS | AUREQ principal detectado como VST3 a partir da instalacao system-level |
| Verificar se `~/Library/Audio/Plug-Ins/VST3` causa duplicata | PASS | Duplicata fisica user-level resolvida; `AUREQ_2` tratado como cache/entrada antiga |
| Executar `Find installed plugins` | PASS | FL Studio detectou AUREQ como VST3 |
| Ativar `Rescan previously verified plugins`, se necessario | BLOCKED | Aguardando validacao humana |
| Ativar `Verify plugins`, se necessario | BLOCKED | Aguardando validacao humana |
| Confirmar AUREQ listado uma unica vez | PARTIAL | AUREQ principal esta correto; `AUREQ_2` ainda aparece como provavel cache antigo |
| Confirmar AUREQ como VST3 | PASS | Usuario confirmou AUREQ como VST3 |
| Confirmar Vendor principal `ASTRA Audio` | PASS | Usuario confirmou vendor correto no AUREQ principal |
| Carregar AUREQ em mixer/track | PASS | AUREQ abriu dentro do FL Studio |
| Abrir UI do plugin | PASS | UI carregou corretamente |
| Passar audio pelo plugin | BLOCKED | Aguardando validacao humana |
| Testar preset browser | BLOCKED | Coberto em detalhe por `aureq-fl-studio-musical-qa-checklist.md` |
| Testar user presets | BLOCKED | Coberto em detalhe por `aureq-fl-studio-musical-qa-checklist.md` |
| Testar A/B | BLOCKED | Coberto em detalhe por `aureq-fl-studio-musical-qa-checklist.md` |
| Testar Undo/Redo | BLOCKED | Coberto em detalhe por `aureq-fl-studio-musical-qa-checklist.md` |
| Testar Dynamic EQ | BLOCKED | Coberto em detalhe por `aureq-fl-studio-musical-qa-checklist.md` |
| Testar Channel Mode | BLOCKED | Coberto em detalhe por `aureq-fl-studio-musical-qa-checklist.md` |
| Salvar e reabrir projeto | BLOCKED | Aguardando validacao humana |
| Observar crash/logs | BLOCKED | Aguardando validacao humana |

## Ableton Live macOS

| Item | Status | Evidencia / Observacoes |
|------|--------|--------------------------|
| Preferences > Plug-Ins > Rescan | BLOCKED | Aguardando DAW/validacao humana |
| Confirmar AUREQ listado como VST3 | BLOCKED | Aguardando DAW/validacao humana |
| Carregar em Audio Track | BLOCKED | Aguardando DAW/validacao humana |
| Abrir UI | BLOCKED | Aguardando DAW/validacao humana |
| Passar audio | BLOCKED | Aguardando DAW/validacao humana |
| Testar presets/user presets/A-B/Undo-Redo/Dynamic EQ/Channel Mode | BLOCKED | Aguardando DAW/validacao humana |
| Salvar e reabrir set | BLOCKED | Aguardando DAW/validacao humana |

## Reaper macOS

| Item | Status | Evidencia / Observacoes |
|------|--------|--------------------------|
| Options > Preferences > Plug-ins > VST > Re-scan | BLOCKED | Aguardando DAW/validacao humana |
| Confirmar AUREQ listado como VST3 | BLOCKED | Aguardando DAW/validacao humana |
| Carregar em track | BLOCKED | Aguardando DAW/validacao humana |
| Abrir UI | BLOCKED | Aguardando DAW/validacao humana |
| Passar audio | BLOCKED | Aguardando DAW/validacao humana |
| Testar presets/user presets/A-B/Undo-Redo/Dynamic EQ/Channel Mode | BLOCKED | Aguardando DAW/validacao humana |
| Salvar e reabrir projeto | BLOCKED | Aguardando DAW/validacao humana |

## Hosts Futuros

| Host | Status | Observacoes |
|------|--------|-------------|
| Logic Pro | BLOCKED | Logic Pro nao carrega VST3 nativamente; validar em fase futura via AU ou wrapper, nao como falha VST3 |
| Studio One | BLOCKED | Validacao futura VST3 |
| Cubase | BLOCKED | Validacao futura VST3 |
| Bitwig Studio | BLOCKED | Validacao futura VST3 |

## Politica de Duplicatas

Se uma duplicata fisica aparecer, nao remover automaticamente. Documentar o caminho e pedir aprovacao explicita antes de qualquer acao destrutiva.

Comandos de diagnostico seguros:

```bash
find "/Library/Audio/Plug-Ins/VST3" "$HOME/Library/Audio/Plug-Ins/VST3" -maxdepth 1 \( -name "AUREQ.vst3" -o -name "AUREQ_2.vst3" \) -print
```

Se FL Studio ainda mostrar `AUREQ_2` sem duplicata fisica, tratar primeiro como cache/registro interno da DAW e repetir scan/verify antes de mexer em arquivos.

Nota da Fase 15.2I:

- O AUREQ principal foi validado no FL Studio macOS como VST3, abrindo UI corretamente e com vendor `ASTRA Audio`.
- O `AUREQ_2` remanescente aparece com vendor `ASTRA Audio Suite`, coerente com cache/entrada antiga de build anterior.
- Recomendacao: manter favorito apenas no AUREQ correto e remover/desfavoritar `AUREQ_2` no Plugin Manager.

Nota da Fase 15.2J:

- O QA musical real do FL Studio foi preparado em `docs/workflow/aureq-fl-studio-musical-qa-checklist.md`.
- O resultado final permanece `BLOCKED` ate validacao humana de audio, filtros, presets, A/B, Undo/Redo, Dynamic EQ, Channel Mode, bypass, analyzer e salvar/reabrir projeto.
