#pragma once

#include <juce_core/juce_core.h>

/**
 * AnalyzerFIFO — Lock-Free Audio → UI Transport
 *
 * Propósito:
 *   Transferir amostras de áudio (pós-EQ) da thread de áudio de alta prioridade
 *   para a thread de UI de forma completamente livre de travas (lock-free),
 *   usando juce::AbstractFifo como mecanismo de indexação atômico.
 *
 * Regras de uso:
 *   - push() é chamado APENAS na audio thread (dentro de processBlock)
 *   - pop()  é chamado APENAS na UI/message thread (dentro de timerCallback)
 *   - Nenhuma alocação dinâmica ocorre após a construção do objeto
 *   - O buffer interno é pré-alocado com tamanho fixo (FIFO_SIZE)
 */
class AnalyzerFIFO
{
public:
    static constexpr int FIFO_SIZE = 8192;

    AnalyzerFIFO();

    /**
     * Push amostras da thread de áudio para a FIFO.
     * Chamado EXCLUSIVAMENTE dentro de processBlock().
     * Nunca aloca memória nem usa locks.
     */
    void push (const float* data, int numSamples) noexcept;

    /**
     * Pop até maxSamples amostras da FIFO para o buffer de destino.
     * Chamado EXCLUSIVAMENTE na UI/message thread.
     * Retorna o número real de amostras lidas.
     */
    int pop (float* dest, int maxSamples) noexcept;

private:
    juce::AbstractFifo abstractFifo { FIFO_SIZE };
    float buffer[FIFO_SIZE];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyzerFIFO)
};
