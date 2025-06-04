#include "Shimmer.h"

#include <cmath>

namespace DSP
{

Shimmer::Shimmer(float maxTimeMs, float blockSizeMS, unsigned int numChannels) :
    delayLine(static_cast<unsigned int>(std::ceil(std::fmax(maxTimeMs, 1.f) * static_cast<float>(0.001 * sampleRate))), numChannels),
    buildupRamp(0.5f),
    shift1(static_cast<float>(std::fmax(blockSizeMS, 20.0f)), numChannels),
    shift2(static_cast<float>(std::fmax(blockSizeMS, 20.0f)), numChannels)
{
}

Shimmer::~Shimmer()
{
}

void Shimmer::prepare(double newSampleRate, float maxTimeMs, unsigned int numChannels, unsigned int numSamples)
{
    sampleRate = newSampleRate;

    delayLine.prepare(static_cast<unsigned int>(std::round(maxTimeMs * static_cast<float>(0.001 * sampleRate))), MaxChannels);
    delayLine.setDelaySamples(1); // Keep at least 1 sample minimum fixed delay

    shift1.prepare(sampleRate);
    shift1.setPitchRatio(2.f);

    shift2.prepare(sampleRate);
    shift2.setPitchRatio(0.5f); // initial values

    buildupRamp.prepare(sampleRate, true, buildupMs * static_cast<float>(sampleRate * 0.001));

    // Resize buffers to match channel count and buffer size
    delayOut.resize(numChannels);
    shifted1.resize(numChannels);
    shifted2.resize(numChannels);

    delayOutPtrs.resize(numChannels);
    shifted1Ptrs.resize(numChannels);
    shifted2Ptrs.resize(numChannels);

    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        delayOut[ch].resize(numSamples, 0.0f);
        shifted1[ch].resize(numSamples, 0.0f);
        shifted2[ch].resize(numSamples, 0.0f);

        delayOutPtrs[ch] = delayOut[ch].data();
        shifted1Ptrs[ch] = shifted1[ch].data();
        shifted2Ptrs[ch] = shifted2[ch].data();
    }

}

void Shimmer::clear()
{
    delayLine.clear();
    shift1.clear();
    shift2.clear();
}

void Shimmer::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    // Apply delay
    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Process LFO no modulation, just add zeros for delay/buildup ramp
        float lfo[2] { 0.f, 0.f };
        // Apply buildup ramp
        buildupRamp.applySum(lfo, numChannels);

        // Delay in/out
        float x[2];
        float y[2];

        for (unsigned int ch = 0; ch < numChannels; ++ch)
            x[ch] = input[ch][n];

        // Process delay
        delayLine.process(y, x, lfo, numChannels);

        // Write to output buffers
        for (unsigned int ch = 0; ch < numChannels; ++ch)
            delayOutPtrs[ch][n] = y[ch];
    }

    //Apply pitch shifting to delayed signal
    shift1.process(shifted1Ptrs.data(), const_cast<const float* const*>(delayOutPtrs.data()), numChannels, numSamples);
    shift2.process(shifted2Ptrs.data(), const_cast<const float* const*>(delayOutPtrs.data()), numChannels, numSamples);

    // Add pitch shifted signals
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        for (unsigned int n = 0; n < numSamples; ++n)
        {
            output[ch][n] = 0.5f * (shifted1[ch][n] + shifted2[ch][n]);
        }
    }
}

void Shimmer::setBuildup(float newBuildupMs)
{
    buildupMs = std::fmax(newBuildupMs, 1.f);
    buildupRamp.setTarget(buildupMs * static_cast<float>(sampleRate * 0.001));
}

void Shimmer::setRatio1(float newRatio1)
{
    ratio1 = std::clamp(newRatio1, 0.25f, 2.f);
    shift1.setPitchRatio(static_cast<float>(ratio1));
}

void Shimmer::setRatio2(float newRatio2)
{
    ratio2 = std::clamp(newRatio2, 0.25f, 2.f);
    shift2.setPitchRatio(static_cast<float>(ratio2));
}


}