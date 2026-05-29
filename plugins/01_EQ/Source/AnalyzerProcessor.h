#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>

/**
 * AnalyzerProcessor — FFT Spectrum Analysis (UI Thread Only)
 *
 * Propósito:
 *   Acumular amostras recebidas via AnalyzerFIFO, aplicar janelamento Hann,
 *   calcular a FFT (juce::dsp::FFT, 2048 pontos), converter magnitudes para dBFS
 *   e aplicar suavização temporal com decaimento de pico (ballistics).
 *
 * Regras de uso:
 *   - pushSamples() é chamado APENAS na UI/message thread (dentro de timerCallback)
 *   - Nenhuma alocação dinâmica após a construção
 *   - getNumBins() retorna fftSize/2 + 1 bins úteis (freqüências de DC a Nyquist)
 *
 * Estratégia de overlap:
 *   Após cada frame FFT processado, os últimos fftSize/2 samples do buffer interno
 *   são mantidos (50% overlap), melhorando a continuidade temporal do espectro.
 */
class AnalyzerProcessor
{
public:
    // FFT order 11 → 2^11 = 2048 amostras por frame
    static constexpr int FFT_ORDER = 11;
    static constexpr int FFT_SIZE  = 1 << FFT_ORDER; // 2048
    static constexpr int NUM_BINS  = FFT_SIZE / 2 + 1; // 1025 bins DC→Nyquist

    AnalyzerProcessor();

    /**
     * Acumula amostras no buffer interno.
     * Quando o buffer atinge FFT_SIZE amostras, processa automaticamente
     * e mantém FFT_SIZE/2 amostras de sobreposição (50% overlap).
     * Chamado APENAS na UI/message thread.
     */
    void pushSamples (const float* data, int numSamples);

    /**
     * Retorna o array de magnitudes espectrais suavizadas em dBFS.
     * Indexado de 0 (DC, 0 Hz) até NUM_BINS-1 (Nyquist).
     * Válido após pelo menos um frame ter sido processado.
     */
    const float* getBins() const noexcept { return spectrumBins.data(); }

    /**
     * Retorna o número de bins disponíveis (FFT_SIZE/2 + 1 = 1025).
     */
    int getNumBins() const noexcept { return NUM_BINS; }

private:
    /**
     * Executa o pipeline completo de análise:
     * 1. Copia fifoBuffer → fftData
     * 2. Aplica janela Hann elemento a elemento
     * 3. Calcula FFT usando performFrequencyOnlyForwardTransform
     * 4. Converte magnitudes para dBFS e aplica ballistics de decaimento
     * 5. Desloca fifoBuffer em FFT_SIZE/2 amostras (50% overlap)
     */
    void process();

    juce::dsp::FFT fft;

    // Buffer de acumulação de amostras
    std::array<float, FFT_SIZE> fifoBuffer;
    int fifoFill = 0;

    // Buffer de trabalho para a FFT (tamanho duplo: N complexos)
    std::array<float, FFT_SIZE * 2> fftData;

    // Coeficientes da janela de Hann pré-calculados
    std::array<float, FFT_SIZE> hannWindow;

    // Bins de saída com magnitude suavizada (dBFS)
    std::array<float, NUM_BINS> spectrumBins;

    // Constante de decaimento temporal (ballistics)
    // 0.78f por frame a 30Hz ≈ ~135ms de decaimento visual
    static constexpr float DECAY_FACTOR = 0.78f;

    // Nível mínimo de silêncio em dBFS para o floor do espectro
    static constexpr float FLOOR_DB = -90.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyzerProcessor)
};
