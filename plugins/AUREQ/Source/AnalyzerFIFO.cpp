#include "AnalyzerFIFO.h"

AnalyzerFIFO::AnalyzerFIFO()
{
    // Zero-initialize the internal buffer at construction time.
    // No further dynamic allocation ever occurs.
    std::memset (buffer, 0, sizeof (buffer));
}

void AnalyzerFIFO::push (const float* data, int numSamples) noexcept
{
    // AbstractFifo::prepareToWrite gives us two contiguous write regions
    // (due to the circular nature of the ring buffer) without using any locks.
    int start1, size1, start2, size2;
    abstractFifo.prepareToWrite (numSamples, start1, size1, start2, size2);

    if (size1 > 0)
        std::memcpy (buffer + start1, data, (size_t) size1 * sizeof (float));

    if (size2 > 0)
        std::memcpy (buffer + start2, data + size1, (size_t) size2 * sizeof (float));

    abstractFifo.finishedWrite (size1 + size2);
}

int AnalyzerFIFO::pop (float* dest, int maxSamples) noexcept
{
    int start1, size1, start2, size2;
    abstractFifo.prepareToRead (maxSamples, start1, size1, start2, size2);

    if (size1 > 0)
        std::memcpy (dest, buffer + start1, (size_t) size1 * sizeof (float));

    if (size2 > 0)
        std::memcpy (dest + size1, buffer + start2, (size_t) size2 * sizeof (float));

    abstractFifo.finishedRead (size1 + size2);

    return size1 + size2;
}
