#include "DattorroDelayLine.h"

#include <algorithm>
#include <cmath>

namespace DSP
{

DelayLine::DelayLine(unsigned int maxLengthSamples, unsigned int numChannels)
{
    delayBufferSize = std::max(maxLengthSamples + 1u, 1u);
    for (unsigned int ch = 0; ch < numChannels; ++ch)
        delayBuffer.emplace_back(delayBufferSize, 0.f);
}

DelayLine::~DelayLine()
{
}

void DelayLine::setDelaySamples(unsigned int newDelaySamples)
{
    delaySamples = std::max(std::min(newDelaySamples, static_cast<unsigned int>(delayBuffer[0].size() - 1u)), 1u);
}

float DelayLine::getSample(unsigned int channel, unsigned int index) const
{   
    const unsigned int delayBufferSize{ static_cast<unsigned int>(delayBuffer[0].size()) };

    index = std::max(std::min(index, delayBufferSize - 1u), 1u);
    unsigned int workingReadIndex { (writeIndex + delayBufferSize - index) % delayBufferSize };

    return delayBuffer[channel][workingReadIndex];
}

void DelayLine::clear()
{
    for (auto& b : delayBuffer)
        std::fill(b.begin(), b.end(), 0.f);
}

void DelayLine::prepare(unsigned int newLengthSamples, unsigned int numChannels)
{
    delayBuffer.clear();
    for (unsigned int ch = 0; ch < numChannels; ++ch)
        delayBuffer.emplace_back(std::max(delayBufferSize + 1u, 1u), 0.f);

    delaySamples = newLengthSamples;
}

void DelayLine::process(float* output, const float* input, unsigned int numChannels)
{
    const unsigned int delayBufferSize{ static_cast<unsigned int>(delayBuffer[0].size()) };

    numChannels = std::min(numChannels, static_cast<unsigned int>(delayBuffer.size()));

    unsigned int workingWriteIndex { writeIndex };
    unsigned int workingReadIndex { (workingWriteIndex + delayBufferSize - delaySamples) % delayBufferSize };

    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        const float x { input[ch] };
        delayBuffer[ch][workingWriteIndex] = x;
        output[ch] = delayBuffer[ch][workingReadIndex];
    }

    ++writeIndex; writeIndex %= delayBufferSize;
}

void DelayLine::process(float* audioOutput, const float* audioInput, const float* modInput, unsigned int numChannels)
{
    const unsigned int delayBufferSize{ static_cast<unsigned int>(delayBuffer[0].size()) };

    // Calculate base indices based on fixed delay time
    unsigned int workingWriteIndex { writeIndex };
    unsigned int workingReadIndex { (workingWriteIndex + delayBufferSize - delaySamples) % delayBufferSize };

    numChannels = std::min(numChannels, static_cast<unsigned int>(delayBuffer.size()));
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        // Linear interpolation coefficients
        const float m { std::fmax(modInput[ch], 0.f) };
        const float mFloor { std::floor(m) };
        const float mFrac0 { m - mFloor };
        const float mFrac1 { 1.f - mFrac0 };

        // Calculate read indeces
        const unsigned int readIndex0 { (workingReadIndex + delayBufferSize + static_cast<unsigned int>(mFloor)) % delayBufferSize };
        const unsigned int readIndex1 { (readIndex0 + delayBufferSize - 1u) % delayBufferSize };

        // Read from delay line
        const float read0 = delayBuffer[ch][readIndex0];
        const float read1 = delayBuffer[ch][readIndex1];

        // Read audio input
        const float x { audioInput[ch] };

        // Write input
        delayBuffer[ch][workingWriteIndex] = x;

        // Interpolate output
        audioOutput[ch] = read0 * mFrac0 + read1 * mFrac1;

    }

    // Update persistent write index
    ++writeIndex; writeIndex %= delayBufferSize;
}

}