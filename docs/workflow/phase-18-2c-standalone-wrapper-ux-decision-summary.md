# Phase 18.2C — Standalone Wrapper UX Decision

## Status

**NO ACTION**

---

## 1. Contexto

Durante a validacao visual do AUREQ e do LUMINAR em modo Standalone, foi observado um topo padrao com:

- `Options`
- `Audio input is muted to avoid feedback loop`
- `Settings...`
- botao de fechar da janela nativa

Esse topo quebra a identidade visual premium quando o plugin e aberto como aplicativo Standalone.

---

## 2. Origem Tecnica

A barra vem do `StandaloneFilterWindow` padrao do JUCE, usado pelo wrapper Standalone gerado por `juce_audio_plugin_client`.

Ela nao faz parte da UI interna do AUREQ nem do LUMINAR. A UI propria dos plugins comeca dentro do editor criado pelo `AudioProcessorEditor`; o topo pertence ao aplicativo Standalone que hospeda esse editor.

---

## 3. Impacto No VST3

O VST3 dentro das DAWs nao usa o `StandaloneFilterWindow` padrao do JUCE.

Portanto:

- A barra `Options` / `Settings...` / `Audio input is muted...` nao aparece no VST3 dentro da DAW.
- O produto principal nao sofre esse impacto visual.
- Qualquer chrome externo visto em uma DAW pertence ao host, nao ao wrapper Standalone do JUCE.

---

## 4. Decisao De Produto

- Produto principal: **VST3**.
- Standalone: **ferramenta interna de desenvolvimento/QA**.
- Installers beta: continuam **plugin-only VST3**.
- Nao ajustar a UI interna do AUREQ/LUMINAR por causa da barra externa do Standalone.
- Nao investir agora em custom Standalone.
- `Custom Standalone App Wrapper` fica para fase futura, caso o Standalone vire produto distribuido.

---

## 5. Opcoes Avaliadas

| Opcao | Decisao |
|---|---|
| Manter Standalone padrao JUCE | Aprovado como ferramenta interna |
| Remover Standalone do build | Nao necessario agora |
| Polir visual interno para compensar a barra | Rejeitado; problema e externo ao editor |
| Criar custom Standalone wrapper | Adiar para fase futura |
| Alterar installers beta | Rejeitado; manter plugin-only VST3 |

---

## 6. Acao Recomendada

Nenhuma alteracao agora.

Nao alterar:

- Codigo.
- CMake.
- DSP.
- APVTS.
- UI.
- AUREQ.
- LUMINAR.
- Installers.
- Livros/PDFs.

Status final: **NO ACTION**.
