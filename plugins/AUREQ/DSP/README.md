# Clean EQ - Motor de Processamento (plugins/01_EQ/DSP)

Esta pasta conterá as classes que executam os cálculos matemáticos dos filtros e do processamento de áudio digital do equalizador.

## Responsabilidades
*   **Engine de Equalização (`EqualizerEngine`)**:
    *   Gerencia e processa o áudio multicanal (Estéreo) recebido pelo buffer do plugin.
    *   Lida com o processamento das 5 bandas selecionáveis de filtros recursivos.
*   **Filtros Biquad IIR**:
    *   Cálculo dos coeficientes e processamento recursivo das equações matemáticas baseadas na teoria estudada (Fase 2) para os tipos:
        *   High-Pass Filter (HPF)
        *   Low-Shelf Filter
        *   Peak Filter (Banda paramétrica média)
        *   High-Shelf Filter
        *   Low-Pass Filter (LPF)
*   **Suavização de Coeficientes**:
    *   Garante que mudanças nos knobs de ganho, frequência ou Q não gerem zipper noise ou estalos de fase por meio de interpolação linear ou exponencial dos coeficientes do filtro.
*   **Proteção contra Erros**:
    *   Garante que o áudio seja protegido contra NaNs, denormais ou transientes violentos de clipping interno.
