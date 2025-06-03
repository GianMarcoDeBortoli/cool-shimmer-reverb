#include "DattorroAllPass.h"

namespace DSP
{

AllPass::AllPass(float initDelaySamples, float initCoeff, unsigned int initNumChannels) :
    delayLine(initDelaySamples, static_cast<unsigned int>(std::min(std::max(initNumChannels, 1u), MaxChannels))),
    delayTimeSamples { initDelaySamples },
    coeff { initCoeff }
{
    setDelayTime(delayTimeSamples);
}

AllPass::~AllPass()
{
}

void AllPass::setCoeff(const float newCoeff)
{
    coeff = std::clamp(newCoeff, -0.98f, 0.98f);
}

void AllPass::setDelayTime(float newDelaySamples)
{
    delayLine.setDelaySamples(newDelaySamples);
}

float AllPass::getSample(unsigned int channel, unsigned int index)
{
    return delayLine.getSample(channel, index);
}

void AllPass::prepare(unsigned int newNumChannels)
{
    delayLine.prepare(delayTimeSamples, newNumChannels);
    clear();
}


void AllPass::clear()
{
    delayLine.clear();

    feedbackState[0] = 0.f;
    feedbackState[1] = 0.f;
}

void AllPass::process(float* output, const float* input, unsigned int numChannels)
{
    // Preallocate inputs to delay line
    float delayIn[2] { 0.f, 0.f };

    // Iterate over channels
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        // Compute the delay line input
        delayIn[ch] = -coeff * feedbackState[ch] + input[ch];
        // Compute the process output
        output[ch] = coeff * delayIn[ch] + feedbackState[ch];
    }

    // Feed the delay line
    delayLine.process(feedbackState, delayIn, numChannels);
}

void AllPass::process(float*output, const float* input, unsigned int numChannels, const float* modInput)
{
    // Preallocate inputs to delay line
    float delayIn[2] { 0.f, 0.f };

    // Iterate over channels
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        // Compute the delay line input
        delayIn[ch] = -coeff * feedbackState[ch] + input[ch];
        // Compute the process output
        output[ch] = coeff * delayIn[ch] + feedbackState[ch];
    }

    // Feed the delay line
    delayLine.process(feedbackState, delayIn, modInput, numChannels);
}

}