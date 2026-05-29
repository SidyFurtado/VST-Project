# Fase 15.2I — FL Studio macOS Validation Sign-off / Cache Note

Data: 2026-05-28

## Objetivo

Registrar o resultado da validacao manual do AUREQ no FL Studio macOS e documentar a situacao residual do `AUREQ_2` como provavel cache/entrada antiga da DAW, sem limpar cache automaticamente e sem remover arquivos.

## Resultado FL Studio macOS

Status: **PARTIAL / PASS funcional principal**.

Resultados observados pelo usuario:

- O AUREQ aparece como VST3 no FL Studio macOS.
- O AUREQ abre dentro do FL Studio.
- A UI carregou corretamente.
- O Vendor do AUREQ principal aparece corretamente como `ASTRA Audio`.
- O plugin fisico instalado existe somente em:

```text
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

- Nao existe mais duplicata fisica em:

```text
~/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

## Nota Sobre AUREQ_2

O FL Studio ainda mostra `AUREQ_2`, mas o estado atual aponta para cache interno/entrada antiga da DAW:

- Nao ha duplicata fisica correspondente no filesystem.
- O `AUREQ_2` aparece com vendor `ASTRA Audio Suite`, indicando scan/cache antigo de build anterior.
- O `moduleinfo.json` atual confirma vendor `ASTRA Audio`.

Recomendacao:

- Manter como favorito apenas o AUREQ correto.
- Remover/desfavoritar `AUREQ_2` no Plugin Manager do FL Studio.
- Nao apagar arquivos em `/Library` ou `~/Library` sem aprovacao explicita.
- Nao limpar cache automaticamente nesta fase.

## Gatekeeper

Status: **BLOCKED** ate Apple Developer ID + notarizacao.

A build atual continua sendo RC interno sem assinatura/notarizacao final. Alertas do Gatekeeper continuam esperados ate que exista assinatura Developer ID e notarizacao Apple.

## Integridade de Escopo

Nao foi feito:

- Nenhuma edicao de codigo.
- Nenhuma edicao de CMake.
- Nenhuma alteracao de APVTS, IDs, DSP, analyzer FFT, presets ou UI interna.
- Nenhuma alteracao de PKG ou DMG.
- Nenhuma alteracao em Livros/PDFs.
- Nenhum build.
- Nenhum `sudo`.
- Nenhuma remocao de arquivos.
- Nenhuma limpeza automatica de cache.
- Nenhuma instalacao.

## Proximo Passo

Continuar priorizando macOS antes do Windows:

1. Finalizar validacao manual no FL Studio com foco em audio, presets, user presets, A/B, Undo/Redo, Dynamic EQ, Channel Mode e salvar/reabrir projeto.
2. Validar Ableton Live e Reaper quando disponiveis.
3. Resolver assinatura/notarizacao quando houver Apple Developer ID.
4. So depois avancar para Windows installer.
