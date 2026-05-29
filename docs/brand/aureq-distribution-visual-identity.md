# Identidade Visual de Distribuição — AUREQ / ASTRA Audio

Este documento estabelece as diretrizes e a especificação da identidade visual utilizada nos artefatos de distribuição (ícones, instaladores e materiais de release) do AUREQ e da suíte ASTRA Audio.

## 1. Direção Visual Geral

A linguagem estética segue a diretriz aprovada do projeto (Premium Glass & Cosmic Navy), focando em:
- **Temática**: Áudio premium, espacial, precisão digital.
- **Paleta**: Azul profundo cósmico (Navy) contrastando com brilhantes Cyan e Mint.
- **Formas**: Geometria curva suave e anéis orbitais limpos.
- **Textura**: Vidro líquido (glassmorphism), halos suaves de luz (bloom) sobre fundos com gradientes sem saturação exagerada.

---

## 2. Ícone do AUREQ (Plugin)

O ícone do plugin AUREQ é focado em sua função primária: **Equalização Paramétrica**.

- **Conceito "EQ Node"**: Apresenta uma curva paramétrica estilizada com um ponto de controle luminoso (nó de banda) ao centro.
- **Fundo**: Gradiente radial de `#0B0F12` a `#0D1F35`.
- **Destaque**: Cyan (`#22D3EE`). O nó brilha contra um halo suave, sendo o único elemento totalmente reconhecível em resoluções minúsculas (16x16px).
- **Uso**: Instaladores "plugin-only", atalhos de Standalone e recursos de UI do sistema.

---

## 3. Ícone da ASTRA Suite (Pacote)

O ícone da suíte completa é mais amplo, focado no ecossistema global.

- **Conceito "Orbital Constellation"**: Um sistema planetário orbitando um núcleo central brilhante.
- **Fundo**: Gradiente radial de `#08080E` a `#0B1628` (ligeiramente mais escuro e profundo que o ícone do AUREQ).
- **Destaque**: Cyan e Mint em gradiente (`#22D3EE` a `#2DD4BF`).
- **Uso**: Instaladores do pacote completo ("Full Suite Installer") e identificação da marca matriz no sistema.

---

## 4. Instaladores macOS / Windows

A estética do instalador deve ser uma extensão da interface premium do plugin.

### Header/Banner
- **Tamanho padrão macOS**: `740x90` pixels.
- **Layout**: Fundo cósmico escuro (`#0B0F12`), tipografia limpa (sem-serifa de sistema) alinhada à esquerda, linha de acento cyan na base e curva de EQ abstrata estilizada no lado direito.

### Background Welcome Screen
- **Tamanho padrão macOS**: `740x540` pixels.
- **Layout**: Deep Cosmic Background com uma nebulosa (aurora cyan/mint). Incorpora uma curva de EQ suave de fundo e um painel translúcido (glassmorphism) do lado direito com borda fina e sombra acentuada (para enquadrar o texto do instalador nativo).

---

## 5. Diretrizes de Exportação e Empacotamento

As fontes de verdade visuais são mantidas como vetores SVG escaláveis na pasta `assets/`. A rasterização ocorre apenas nas etapas finais de build.

### macOS (Fase 15.2)
Para `AUREQ.app` ou `AUREQ.vst3`, converter SVG para `.icns` via `iconutil`.
Resoluções necessárias no `iconset`:
- 16x16 (normal e @2x)
- 32x32 (normal e @2x)
- 128x128 (normal e @2x)
- 256x256 (normal e @2x)
- 512x512 (normal e @2x)

### Windows (Fase 15.3)
Para o Standalone executável e atalhos, gerar arquivo `.ico` contendo as seguintes resoluções rasterizadas:
- 16x16
- 32x32
- 48x48
- 64x64
- 128x128
- 256x256

## 6. Proibições
- Não usar logos realistas de hardware (não desenhar válvulas, transistores ou parafusos metálicos nos ícones).
- Não utilizar fontes tipográficas que exijam licenciamento externo em SVGs.
- Não exportar `.ico` ou `.icns` no repositório se eles puderem ser gerados automaticamente pelo sistema de build usando CMake/scripts.
