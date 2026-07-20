# ASTRA Suite — Phase 23.5E: Download Page & Beta Docs Update Summary

Este documento resume a execução das atualizações de documentação, manuais de instalação, guias de beta e da página oficial de download, consolidando a estratégia **macOS AU-First** e **Windows VST3-First** para os plugins **AUREQ, LUMINAR e GRAVITY**.

---

## Ações Executadas

### 1. Atualização da Landing Page de Download
Modificamos em sincronia os arquivos:
* [docs/index.html](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/index.html)
* [docs/site/index.html](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/site/index.html)

**Principais alterações:**
* **Links de macOS:** Todos os botões de download de macOS (`AUREQ`, `LUMINAR` e `GRAVITY`) foram alterados do formato antigo plugin-only individual no GitHub Releases para apontar para o instalador unificado da suite AU-first:
  `../installer/macOS/au-first-suite/output/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`
* **Metadados de Formatos (macOS AU / Windows VST3):**
  * Atualização dos textos dos botões de macOS de `macOS — .pkg` / `macOS .pkg` para `macOS (Audio Unit)`.
  * Atualização do rótulo de especificações de formato de `AU/VST3` para `AU-Only Suite` sob os botões macOS.
  * Atualização do cabeçalho de especificações de cards de `AU / VST3` para `AU (Mac) &middot; VST3 (Win)`.
  * Atualização do rótulo descritivo geral (`download-subtext`) para:
    `Versão beta privada. Disponível para macOS (Audio Unit) e Windows (VST3), prontos para a sua DAW.`
* **Painel de Avisos Expandido:**
  Substituição da div simples de Gatekeeper (`download-warning`) por uma caixa de aviso estruturada (`warnings-container`) com borda colorida e flexbox contendo:
  * **Nota Beta:** Aviso de beta privada unsigned/not notarized.
  * **Nota macOS:** Esclarecimento de que a suite AU é recomendada para Logic Pro, Final Cut, GarageBand e DAWs com suporte AU, e instrução curta de clique direito -> Abrir para bypassar o Gatekeeper, juntamente com o lembrete de limpeza de cache de componentes duplicados antigos (`Manu`).
  * **Nota Windows:** Esclarecimento de que o formato VST3 é recomendado para FL Studio, Ableton, Reaper, Studio One, etc., e instrução sobre o aviso do SmartScreen ("Mais Informações" -> "Executar assim mesmo").

### 2. Refatoração dos Documentos Beta
* [docs/beta/AUREQ-0.9.0-rc1-private-beta-instructions.md](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/beta/AUREQ-0.9.0-rc1-private-beta-instructions.md):
  * Focado inteiramente na instalação do instalador da suite Audio Unit (`ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`) no caminho global `/Library/Audio/Plug-Ins/Components/`.
  * Adicionada seção de instalação limpa com os comandos exatos de remoção de duplicatas locais (`~/Library`) e limpeza de cache AU.
  * Atualizada a checklist de QA para testar como Audio Unit, com suporte ao Logic Pro.
* [docs/beta/AUREQ-0.9.0-rc1-windows-beta-instructions.md](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/beta/AUREQ-0.9.0-rc1-windows-beta-instructions.md):
  * Atualizada para remover a indicação de bloqueio, apontando para o setup EXE oficial gerado pelo pipeline de build em CI (`AUREQ-0.9.0-rc1-Windows-plugin-only-setup.exe`).
  * Adicionado aviso de segurança sobre SmartScreen.

### 3. Atualização dos Manuais de Instalação e READMEs
* [docs/release/AUREQ-macos-installation-guide.md](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/release/AUREQ-macos-installation-guide.md):
  * Reescreveu o manual focando inteiramente no pacote AU-first global `/Library/Audio/Plug-Ins/Components/`.
  * Adicionados comandos e explicações claras para remover as duplicatas de usuário antigas e resetar o cache do AudioUnit.
* [installer/macOS/au-first-suite/resources/README.txt](file:///Users/sidyziin/Documents/VST%20-%20Project/installer/macOS/au-first-suite/resources/README.txt):
  * Incluídos os passos recomendados de limpeza de cache de Audio Units e remoção de duplicatas da pasta do usuário para que os testers da suite não sofram com conflitos de cache na validação da DAW.

### 4. Gestão de Planos de Desenvolvimento e Checklist
* [docs/release/AUREQ-release-checklist.md](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/release/AUREQ-release-checklist.md):
  * Registrada a conclusão da Fase 23.5E.
* [plugins/01_EQ/Docs/implementation-plan.md](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/Docs/implementation-plan.md), [plugins/02_AIR/Docs/implementation-plan.md](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/02_AIR/Docs/implementation-plan.md) e [plugins/03_GRAVITY/Docs/implementation-plan.md](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/03_GRAVITY/Docs/implementation-plan.md):
  * Registrado o escopo e a conclusão da Fase 23.5E como concluída (`[CONCLUÍDA]` / `[x]` / `✅`).

---

## Validação e Conformidade

### 1. Integridade de Links
Confirmamos que todos os 6 links do macOS nas páginas principais apontam para a suite AU:
* **AUREQ:** Card e Box de download apontam para `../installer/macOS/au-first-suite/output/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.
* **LUMINAR:** Card e Box de download apontam para `../installer/macOS/au-first-suite/output/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.
* **GRAVITY:** Card e Box de download apontam para `../installer/macOS/au-first-suite/output/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.

### 2. Validação HTML local
A sintaxe estrutural do HTML foi mantida válida, sem quebras de tags ou de layout do display cartesiano.

### 3. Preservação de Escopo
Garantido que **absolutamente nenhum código C++, script CMake, installer binário (PKG ou EXE), APVTS, DSP ou arquivos PDF** tenha sido alterado durante esta fase de documentação.

---

## Risco de Caminho Relativo no GitHub Pages (Scope Guard / Warning)

> [!WARNING]
> **Risco Importante de Deploy Web:**
> Os botões de download utilizam links relativos a partir das pastas `docs/index.html` e `docs/site/index.html` (ex: `../installer/macOS/...`).
>
> Se o GitHub Pages estiver configurado para servir a página estática a partir da pasta `/docs`, o servidor web tratará `/docs` como a raiz `/`. Logo, caminhos de link que tentem subir um nível (`../`) para alcançar a pasta `installer` (que está fora de `/docs`) **retornarão erro 404 (Not Found)** para os usuários finais.
>
> **Solução Recomendada:**
> Para testes locais em disco, o link relativo funciona perfeitamente. No deploy de produção final na web:
> 1. Fazer o upload do pacote `.pkg` de suite para o GitHub Releases e substituir o link relativo por uma URL absoluta de release (ex: `https://github.com/.../releases/download/.../ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`).
> 2. Alternativamente, mover/copiar o instalador para uma subpasta em `docs/assets/` e linká-lo diretamente como `assets/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg`.
