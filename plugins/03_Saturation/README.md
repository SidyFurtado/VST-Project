# Plugin 03_Saturation: Saturação Harmônica

Este diretório contém a estrutura e os arquivos para o desenvolvimento do processador de Saturação Harmônica.

## Objetivo
Desenvolver um plugin de saturação analógica virtual para adição de harmônicos pares/ímpares, simulando fita (Tape), válvulas (Tube) e circuitos de estado sólido (Transistor), com controle de ganho compensado de entrada e saída.

## Status do Desenvolvimento
*   **Fase atual**: Placeholder (Estruturação Inicial)
*   **Status do Código**: Ainda não implementado

## Observação
> [!IMPORTANT]
> **Ainda não implementar código nesta fase**. O desenvolvimento funcional deste plugin começará em fases subsequentes, após a validação e conclusão dos plugins anteriores.

## Estrutura do Plugin
*   **`Source/`**: Ponto de entrada C++ principal e gerenciador do plugin.
*   **`DSP/`**: Algoritmos de waveshaping, sobreamostragem (oversampling) para prevenção de aliasing e geração harmônica.
*   **`UI/`**: Controles de drive, mix, seleção de algoritmos, meters de distorção e design system específico.
*   **`Presets/`**: Presets de fábrica para calor harmônico e excitação excêntrica.
*   **`Tests/`**: Casos de teste de distorção harmônica total (THD) e oversampling.
*   **`Docs/`**: Documentações específicas sobre as curvas de waveshaping deste saturador.
