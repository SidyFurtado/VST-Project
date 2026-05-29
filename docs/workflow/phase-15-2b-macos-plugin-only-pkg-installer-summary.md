# Resumo Técnico: Fase 15.2B — macOS Plugin-Only PKG Installer

## Status

Concluída.

---

## 1. Objetivo da Fase

Projetar e empacotar o primeiro instalador nativo do AUREQ para o macOS (`.pkg`), restrito estritamente à versão VST3 ("plugin-only installer"), com destinação hard-coded para garantir máxima estabilidade e mínima fricção na descoberta pelas DAWs (`/Library/Audio/Plug-Ins/VST3`).

Esta fase focou em construção e engenharia de release local, aplicando validações não-destrutivas (via descompressão de pacotes) em vez de executar instaladores locais de fato, preservando o sistema operacional de desenvolvimento.

---

## 2. Decisão Arquitetural: Plugin-Only vs. Standalone

Conforme aprovado, a versão standalone (`.app`) foi intencionalmente excluída deste `.pkg`.
**Motivo:** Garantir que o instalador cumpra fielmente sua proposta "plugin-only". O Standalone app, com sua instalação em `/Applications`, fará parte de um instalador multi-propósito (Suite Installer) ou um formato de imagem de disco (`.dmg`) em fases futuras.

---

## 3. Estratégia de Payload e Empacotamento

A síntese do pacote foi dividida em dois artefatos gerados pelo ecosistema Xcode Command Line Tools:

1. **Component Package (`pkgbuild`)**:
   - Um payload falso (`installer/.../payload/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`) foi construído a partir dos binários gerados pela etapa de CMake.
   - Usou-se `--install-location /` para forçar o desdobramento do pacote na raiz do disco alvo.
   
2. **Product Package (`productbuild`)**:
   - Para entregar experiência premium, os assets da Fase 15.1 (`background.png`) e documentação em HTML (`welcome.html` e `license.html`) foram atrelados ao instalador final via `Distribution.xml`.
   - O manifesto XML contém as travas de segurança:
     - `<options customize="never"/>`: Impede que o usuário mude componentes.
     - `<domains enable_currentUserHome="false"/>`: Impede instalações per-user que falhariam na descoberta de várias DAWs (ex: Studio One, Ableton nativo).

**Artefato Final Gerado:**
`installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg`

---

## 4. Validação Não-Destrutiva

Ao invés de clicar e instalar (o que poluiria `/Library` no macOS local e pediria `sudo`), realizamos uma extração reversa profunda:
1. `pkgutil --expand`: Extraiu o pacote product e component sem invocar o instalador.
2. `lsbom`: O "Bill of Materials" provou matematicamente que os arquivos gerados serão alocados *exclusivamente* no caminho oficial `Library/Audio/Plug-Ins/VST3/AUREQ.vst3`.
3. Validação de sistema host: `ls -l /Library/Audio/Plug-Ins/VST3/AUREQ.vst3` falhou propositadamente com "No such file or directory", comprovando que nosso ambiente host continua intocado e imaculado.

---

## 5. Build, DSP e Integridade do Repositório

O pipeline C++ rodou inteiramente antes da operação do empacotamento:
- **Build**: 100% dos targets compilados (incluindo dependências do monorepo e VST3 Helper).
- **DSP Tests (`AUREQ_FilterTests`)**: Todas as asserções em filtros biquadráticos e Dynamic EQ passaram.
- **Isolamento**: Nenhum código, APVTS, CMake ou layout de UI precisou ser tocado para viabilizar este PKG. Tudo foi resolvido nativamente no shell e com `Distribution.xml`.

---

## 6. O Que Fica para Fases Futuras

1. **Assinatura e Notarização Apple**:
   - O comando `pkgutil --check-signature` reportou (como esperado) "no signature".
   - Executar `productsign` no `.pkg` e usar `xcrun notarytool` assim que o "Developer ID Installer Certificate" for emitido e configurado no keychain.
2. **Empacotamento DMG**:
   - Envolver o `.pkg` recém-criado em um disco de imagem virtual (`.dmg`), possivelmente com uma janela de EULA integrada.
3. **Windows Installer (Fase 15.3)**:
   - Configurar o Inno Setup ou NSIS em um ambiente/VM Windows para reproduzir este mesmo "plugin-only" de forma hard-coded em `C:\Program Files\Common Files\VST3\`.
