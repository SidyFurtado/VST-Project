# Plugin 04_Limiter: Limitador de Pico (Limiter)

Este diretório contém a estrutura e os arquivos para o desenvolvimento do Limitador de Pico.

## Objetivo
Desenvolver um brickwall limiter de alta performance para masterização com look-ahead inteligente, controle de teto (ceiling) e detecção de pico real (Inter-sample Peak / True Peak detection) a fim de evitar distorções na conversão analógica.

## Status do Desenvolvimento
*   **Fase atual**: Placeholder (Estruturação Inicial)
*   **Status do Código**: Ainda não implementado

## Observação
> [!IMPORTANT]
> **Ainda não implementar código nesta fase**. O desenvolvimento funcional deste plugin começará em fases subsequentes, após a validação e conclusão dos plugins anteriores.

## Estrutura do Plugin
*   **`Source/`**: Ponto de entrada C++ principal e gerenciador do plugin.
*   **`DSP/`**: Algoritmo de atenuação por look-ahead, cálculo de True Peak e controle de ganho de saída.
*   **`UI/`**: Visualizador de forma de onda em tempo real com redução de ganho (Gain Reduction graph), controles de teto/release e design system específico.
*   **`Presets/`**: Presets de masterização para diversos gêneros musicais (Loudness Maximizer, Safe Clean Master, etc.).
*   **`Tests/`**: Casos de teste de True Peak e prevenção de clipping interamostra.
*   **`Docs/`**: Documentações específicas sobre look-ahead e detecção de True Peak deste limiter.
