# Fase 20.0 — Migração Universal para Audio Unit (AU) no macOS e Atualização de Site
## Resumo do Diagnóstico e Plano de Implementação

Este documento detalha o planejamento para adicionar o formato nativo da Apple (Audio Unit - AU) aos três plugins da suíte ASTRA (AUREQ, LUMINAR e GRAVITY) no macOS, além de adaptar o CI/CD (GitHub Actions), os instaladores `.pkg` e o website do projeto.

---

### 1. CMakeLists.txt dos Plugins
Precisamos atualizar o parâmetro `FORMATS` na macro `juce_add_plugin` para incluir `AU`.

- **AUREQ** ([plugins/01_EQ/CMakeLists.txt](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/01_EQ/CMakeLists.txt#L15)):
  - *Atual:* `FORMATS VST3 Standalone`
  - *Novo:* `FORMATS VST3 AU Standalone`
- **LUMINAR** ([plugins/02_AIR/CMakeLists.txt](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/02_AIR/CMakeLists.txt#L24)):
  - *Atual:* `FORMATS          VST3 Standalone`
  - *Novo:* `FORMATS          VST3 AU Standalone`
- **GRAVITY** ([plugins/03_GRAVITY/CMakeLists.txt](file:///Users/sidyziin/Documents/VST%20-%20Project/plugins/03_GRAVITY/CMakeLists.txt#L24)):
  - *Atual:* `FORMATS          VST3 Standalone`
  - *Novo:* `FORMATS          VST3 AU Standalone`

---

### 2. CI/CD Workflow (`.github/workflows/astra-suite-build.yml`)
No workflow do GitHub Actions ([astra-suite-build.yml](file:///Users/sidyziin/Documents/VST%20-%20Project/.github/workflows/astra-suite-build.yml)), as seguintes etapas no job `build-macos` precisam de alteração:

- **Etapa `Stage Payloads`**:
  - Criar diretórios de staging para os componentes AU (`.component`):
    ```bash
    mkdir -p installer/macOS/aureq/staging/AU
    mkdir -p installer/macOS/luminar/staging/AU
    mkdir -p installer/macOS/gravity/staging/AU
    ```
  - Copiar os arquivos `.component` compilados:
    ```bash
    # Exemplo para AUREQ:
    AUREQ_AU=$(find build -type d -name "AUREQ.component" | head -n 1)
    if [ -n "$AUREQ_AU" ]; then
      cp -R "$AUREQ_AU" installer/macOS/aureq/staging/AU/
    else
      echo "AUREQ.component not found!" && exit 1
    fi
    ```

- **Etapa `Build macOS PKG Installers`**:
  - Compilar pacotes de componentes para os arquivos `.component` (com destino em `/Library/Audio/Plug-Ins/Components`):
    ```bash
    # Exemplo para AUREQ:
    pkgbuild --identifier "br.com.sidyfurtado.aureq.au" \
      --version "${{ env.VERSION }}" \
      --install-location "/Library/Audio/Plug-Ins/Components" \
      --component "installer/macOS/aureq/staging/AU/AUREQ.component" \
      "installer/macOS/aureq/output/AUREQ-AU-component.pkg"
    ```

---

### 3. XMLs de Distribuição do macOS (`Distribution.xml`)
Os arquivos de distribuição do macOS devem incluir referências ao novo componente AU.

- **AUREQ** ([Distribution.xml](file:///Users/sidyziin/Documents/VST%20-%20Project/installer/macOS/aureq/distribution/Distribution.xml)):
  - Adicionar `<pkg-ref id="br.com.sidyfurtado.aureq.au"/>` no cabeçalho.
  - Incluir `<line choice="br.com.sidyfurtado.aureq.au"/>` sob `<choices-outline>`.
  - Definir a escolha de instalação:
    ```xml
    <choice id="br.com.sidyfurtado.aureq.au" title="AUREQ Audio Unit (AU) Plug-in" description="Installs the Audio Unit plug-in to /Library/Audio/Plug-Ins/Components" start_selected="true" required="true">
        <pkg-ref id="br.com.sidyfurtado.aureq.au"/>
    </choice>
    ```
  - Adicionar o binding de referência do pacote no rodapé:
    ```xml
    <pkg-ref id="br.com.sidyfurtado.aureq.au" version="VERSION_PLACEHOLDER" onConclusion="none">AUREQ-AU-component.pkg</pkg-ref>
    ```
- **LUMINAR & GRAVITY** ([LUMINAR Distribution](file:///Users/sidyziin/Documents/VST%20-%20Project/installer/macOS/luminar/distribution/Distribution.xml), [GRAVITY Distribution](file:///Users/sidyziin/Documents/VST%20-%20Project/installer/macOS/gravity/distribution/Distribution.xml)):
  - Aplicar as mesmas alterações equivalentes usando seus respectivos identificadores (`br.com.sidyfurtado.luminar.au` e `br.com.sidyfurtado.gravity.au`) e arquivos (`LUMINAR-AU-component.pkg` e `GRAVITY-AU-component.pkg`).

---

### 4. Website da Suíte (`docs/site/index.html` e `docs/index.html`)
Para refletir as atualizações na landing page de downloads:

- **Seção de Downloads Individuais (Cards)**:
  - Botão de macOS do AUREQ ([docs/site/index.html:L862](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/site/index.html#L862)):
    - Alterar o texto de `macOS — .pkg` para `macOS — Audio Unit (AU) / VST3 / App`.
    - Atualizar a especificação do formato ([docs/site/index.html:L872](file:///Users/sidyziin/Documents/VST%20-%20Project/docs/site/index.html#L872)): `<strong>Formato</strong>VST3, AU, APP`.
  - Aplicar modificações equivalentes para LUMINAR (L929/L939) e GRAVITY (L998/L1008).

- **Seção Geral de Downloads (Download Boxes)**:
  - Legenda e Subtítulo principal: Incluir menção clara a "Audio Unit (AU)".
  - Atualizar os `<small>` tags dos botões de macOS dos três plugins para: `macOS 10.15+ &middot; Apple Silicon &amp; Intel &middot; Audio Unit (AU) / VST3 / App`.
  - Atualizar os `<small>` tags dos botões de Windows para: `Windows 10+ 64-bit &middot; VST3 / App &middot; Instalador EXE` (destacando que no Windows o formato é VST3).
  - Na grade de especificações da DAW, destacar que Logic Pro agora é totalmente compatível em virtude do formato AU nativo.
