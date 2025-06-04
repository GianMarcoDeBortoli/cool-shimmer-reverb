#pragma once

#include "DelayLine.h"
#include "Ramp.h"
#include "GranularPitchShifter.h"

namespace DSP
{

class Shimmer
{
public:

    Shimmer(float maxTimeMs, float blockSizeMS, unsigned int numChannels);
    ~Shimmer();

    // No default ctor
    Shimmer() = delete;

    // No copy semantics
    Shimmer(const Shimmer&) = delete;
    const Shimmer& operator=(const Shimmer&) = delete;

    // No move semantics
    Shimmer(Shimmer&&) = delete;
    const Shimmer& operator=(Shimmer&&) = delete;

    // Update sample rate, reallocates and clear internal buffers
    void prepare(double sampleRate, float maxTimeMs, unsigned int numChannels, unsigned int numSamples);

    // Clear contents of internal buffer
    void clear();

    // Process audio
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Set delay/buildup offset in ms
    void setBuildup(float newBuildupMs);

    // Set pitch shift ratios
    void setRatio1(float newRatio1);
    void setRatio2(float newRatio2);

    static constexpr int MaxChannels { 2 };

private:
    double sampleRate { 48000.0 };

    DSP::DelayLine delayLine;

    DSP::GranularPitchShifter shift1;
    DSP::GranularPitchShifter shift2;

    DSP::Ramp<float> buildupRamp;

    float buildupMs { 0.f };
    float blocksize { 20.f };
    float ratio1 { 1.f };
    float ratio2 { 1.f };

    // Temporary buffers
    std::vector<std::vector<float>> delayOut;
    std::vector<std::vector<float>> shifted1;
    std::vector<std::vector<float>> shifted2;

    // Pointer arrays to access the above
    std::vector<float*> delayOutPtrs;
    std::vector<float*> shifted1Ptrs;
    std::vector<float*> shifted2Ptrs;

};

}