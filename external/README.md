# Pasta de Dependências Externas (external)

Esta pasta é reservada para armazenar e referenciar bibliotecas e frameworks externos de terceiros que não fazem parte do código proprietário do projeto, como o **JUCE Framework**.

## Integração do JUCE

O JUCE poderá ser adicionado e integrado ao projeto futuramente por meio de uma das seguintes abordagens:

1. **Git Submodule (Recomendado)**:
   Adicionar o JUCE como um submódulo do git apontando para a tag de versão estável desejada (ex: `7.0.x` ou `8.x`):
   ```bash
   git submodule add https://github.com/juce-framework/JUCE.git external/JUCE
   ```

2. **Cópia Local (Local Vendor)**:
   Baixar os arquivos do framework diretamente da página oficial e descompactá-los na pasta `external/JUCE`. O diretório correspondente deve ser adicionado ao arquivo `.gitignore` para evitar o commit de arquivos massivos no repositório.

3. **Instalação Externa referenciada via CMake (System Package)**:
   Manter o JUCE instalado no sistema operacional do desenvolvedor e apenas configurar as variáveis de caminho do CMake no `CMakeLists.txt` raiz para apontar para a instalação local do framework (ex: usando `find_package(JUCE CONFIG REQUIRED)` ou fornecendo `JUCE_DIR`).

*Nota: Não baixe o JUCE automaticamente sem autorização prévia e documentada.*
