# Compartilhado - Utilitários Diversos (shared/utilities)

Esta pasta conterá funções puras e classes de utilidades de programação geral C++.

## Responsabilidades
*   **Conversões Matemáticas de Áudio**: Conversões entre decibéis e ganho linear (`Decibels::decibelsToGain` wrappers), conversão de notas MIDI para Hz, etc.
*   **Segurança de Concorrência**: Classes e locks para compartilhamento seguro de estruturas de dados entre a thread de processamento em tempo real de áudio (Audio Thread) e a thread gráfica (Message/UI Thread), evitando alocações e travamentos indesejados.
*   **Formatação de Texto**: Utilitários para converter floats em strings formatadas de forma cirúrgica (ex: truncar casas decimais ou lidar com sufixos dinâmicos como "1.2k Hz" em vez de "1200 Hz").
*   **Lógicas de Depuração (Debugging)**: Logs técnicos de console inteligentes com controle de verbosidade.
