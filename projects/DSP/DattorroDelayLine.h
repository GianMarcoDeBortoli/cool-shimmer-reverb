#pragma once

#include <vector>

namespace DSP
{

class DelayLine
{
public:
    DelayLine(unsigned int maxLengthSamples, unsigned int numChannels);
    ~DelayLine();

    // No default ctor
    DelayLine() = delete;

    // No copy semantics
    DelayLine(const DelayLine&) = delete;
    const DelayLine& operator=(const DelayLine&) = delete;

    // No move semantics
    DelayLine(DelayLine&&) = delete;
    const DelayLine& operator=(DelayLine&&) = delete;

    // Set the current delay time in samples
    void setDelaySamples(unsigned int newDelaySamples);

    // Get sample at requested index
    float getSample(unsigned int channel, unsigned int index) const;

    // Clear the contents of the delay buffer
    void clear();

    // Reallocate delay buffer for the new channel count and clear its contents
    void prepare(unsigned int newLengthSamples, unsigned int numChannels);

    // Single-sample flavour of the fixed delay time processing
    void process(float* output, const float* input, unsigned int numChannels);

    // Single sample flavour of the modulated delay time processing
    void process(float* audioOutput, const float* audioInput, const float* modInput, unsigned int numChannels);

private:
    std::vector<std::vector<float>> delayBuffer;
    unsigned int delayBufferSize { 0 };
    unsigned int delaySamples { 0 };
    unsigned int writeIndex { 0 };
};

}