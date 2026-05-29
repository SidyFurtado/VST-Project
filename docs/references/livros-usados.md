# Referências Técnicas e Livros de DSP

Este arquivo documenta o propósito dos livros e materiais em PDF localizados no diretório `/Livros` na raiz deste projeto. Esses materiais servem como referências teóricas primárias para a engenharia de áudio e programação do monorepo.

## Conteúdo Técnico de Referência

Os PDFs presentes na pasta `Livros/` (incluindo `Practical Digital Signal Processing.pdf` e `book.pdf`) abordam conceitos avançados e fundamentais de:

*   **Processamento Digital de Sinais (DSP)**: Amostragem, quantização e representações no domínio do tempo e da frequência.
*   **Áudio Digital**: Teorema de Nyquist-Shannon, jitter, dither e conversão AD/DA.
*   **Filtros Digitais**: Filtros FIR (Finite Impulse Response), IIR (Infinite Impulse Response), equalização paramétrica, shelving e filtros passa-alta/passa-baixa.
*   **Transformada de Fourier / FFT**: Análise espectral, janelamento (windowing) e processamento em frequência.
*   **Processores de Dinâmica**: Algoritmos de detecção (RMS, Peak), tempos de attack e release, joelho (knee) e curvas de transferência para compressores, expanders e limitadores.
*   **Processamento por Blocos (Block-based Processing)**: Gerenciamento de buffers de áudio do host e latência.
*   **Processamento Amostra por Amostra (Sample-by-sample Processing)**: Filtragem recursiva em tempo real e estruturas de atraso (delay lines).
*   **Fundamentos para Desenvolvimento de Plugins VST**: Arquitetura de plugins, sincronização com a DAW e thread de áudio de alta prioridade.

## Diretiva Obrigatória para o Agente

> [!IMPORTANT]
> **Instrução para o Agente:**
> Sempre que for implementar DSP, filtros, análise espectral, dinâmica, equalização, compressão, saturação, limitação ou qualquer processamento de áudio, consulte primeiro os materiais de referência presentes na pasta de livros do projeto (`Livros/`).

## Resumo da Fase 2

Durante a Fase 2, os livros e PDFs originais contidos na pasta `Livros/` foram rigorosamente lidos, analisados e sumarizados no diretório `docs/dsp-notes/`. 

Esses documentos (01 a 09) representam o extrato prático da teoria, traduzindo conceitos complexos de processamento digital de sinais em guias aplicados exclusivamente à construção dos nossos VSTs de mixagem e masterização. Eles servem agora como a principal base de conhecimento de trabalho rápido e regras de segurança para a escrita de código nas fases seguintes do monorepo, sem a necessidade constante de buscas teóricas pesadas na base de literatura completa.
