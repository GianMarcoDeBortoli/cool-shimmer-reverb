#include "GranularPitchShifter.h"
#include <cmath>
#include <algorithm>

namespace DSP
{

GranularPitchShifter::GranularPitchShifter(float blockSizeMs, unsigned int numChannels)
    : blockSizeMs(blockSizeMs), numChannels(numChannels)
{
    delayBuffer.resize(numChannels);
    window.clear(); // to be generated in prepare()
}

void GranularPitchShifter::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;

    bufferSize = static_cast<unsigned int>(std::ceil(sampleRate)); // 1-second buffer
    blockSizeSamples = static_cast<unsigned int>(std::ceil(blockSizeMs * 0.001 * sampleRate));
    halfBlockSizeSamples = blockSizeSamples / 2;

    for (auto& buf : delayBuffer)
        buf.resize(bufferSize, 0.0f);

    window.resize(blockSizeSamples);
    writeIndex = 0;
}

void GranularPitchShifter::clear()
{
    for (auto& buf : delayBuffer)
        std::fill(buf.begin(), buf.end(), 0.0f);
    writeIndex = 0;
}

void GranularPitchShifter::setPitchRatio(float ratio)
{
    pitchRatio = ratio;
}

float GranularPitchShifter::readInterpolated(const std::vector<float>& buffer, float position) const
{
    int index1 = static_cast<int>(std::floor(position)) % bufferSize;
    int index2 = (index1 + 1) % bufferSize;
    float frac = position - std::floor(position);

    return (1.0f - frac) * buffer[index1] + frac * buffer[index2];
}

void GranularPitchShifter::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    // Step 1: Write input samples to delay buffer
    for (unsigned int n = 0; n < numSamples; ++n)
    {
        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            delayBuffer[ch][writeIndex] = input[ch][n];
        }

        writeIndex = (writeIndex + 1) % bufferSize;
    }

    // Step 2: Read and overlap two grains with interpolation and Hann blending
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        auto& buf = delayBuffer[ch];
        float* out = output[ch];

        // Use a safe offset from write head
        float safeDelay = static_cast<float>(2 * blockSizeSamples);
        float readBase1 = fmodf(static_cast<float>(writeIndex + bufferSize) - safeDelay, bufferSize);
        float readBase2 = fmodf(readBase1 + halfBlockSizeSamples * pitchRatio, bufferSize);

        for (unsigned int n = 0; n < numSamples; ++n)
        {
            float grainOffset = static_cast<float>(n) * pitchRatio;
            float pos1 = fmodf(readBase1 + grainOffset, bufferSize);
            float pos2 = fmodf(readBase2 + grainOffset, bufferSize);

            float phase = static_cast<float>(n) / static_cast<float>(numSamples);

            float sample1 = readInterpolated(buf, pos1);
            float sample2 = readInterpolated(buf, pos2);

            // out[n] = 0.5f * (sample1 + sample2); // Equal-blend crossfade
            // Crossfade phase in [0, 1]
            float fade1 = std::sin(halfPi * phase);
            float fade2  = std::cos(halfPi * phase);
            out[n] = 0.5 * (sample1 * fade1 + sample2 * fade2);

        }
    }
}

} // namespace DSP
