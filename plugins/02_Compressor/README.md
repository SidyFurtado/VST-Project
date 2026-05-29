# Plugin 02_Compressor: Compressor Dinâmico

Este diretório contém a estrutura e os arquivos para o desenvolvimento do Compressor Dinâmico.

## Objetivo
Desenvolver um compressor de áudio premium com controles flexíveis de Threshold, Ratio, Attack, Release, Knee, e Makeup Gain, oferecendo compressão transparente ou coloração analógica selecionável.

## Status do Desenvolvimento
*   **Fase atual**: Placeholder (Estruturação Inicial)
*   **Status do Código**: Ainda não implementado

## Observação
> [!IMPORTANT]
> **Ainda não implementar código nesta fase**. O desenvolvimento funcional deste plugin começará em fases subsequentes, após a validação e conclusão do primeiro plugin (01_EQ).

## Estrutura do Plugin
*   **`Source/`**: Ponto de entrada C++ principal e gerenciador do plugin.
*   **`DSP/`**: Detecção de nível (RMS/Peak) e algoritmos de redução de ganho.
*   **`UI/`**: Controles de compressão, agulhas de Gain Reduction, meters e design system específico.
*   **`Presets/`**: Presets de fábrica para controle dinâmico (Vocais, Bateria, Master, etc.).
*   **`Tests/`**: Casos de teste de compressão e tempos de resposta.
*   **`Docs/`**: Documentações específicas sobre o algoritmo de detecção de dinâmica deste compressor.
