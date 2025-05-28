#include "AllPass.h"

namespace DSP
{

AllPass::AllPass(float initDelayMs, float initCoeff, unsigned int initNumChannels) :
    delayLine(static_cast<unsigned int>(std::ceil(std::fmax(initDelayMs, 1.f) * static_cast<float>(0.001 * sampleRate))),
               static_cast<unsigned int>(std::min(std::max(initNumChannels, 1u), MaxChannels))),
    delayTimeMs { initDelayMs },
    coeff { initCoeff }
{
}

AllPass::~AllPass()
{
}

void AllPass::prepare(double newSampleRate, unsigned int newNumChannels)
{
    sampleRate = newSampleRate;
    delayLine.prepare(static_cast<unsigned int>(std::round(delayTimeMs * static_cast<float>(0.001 * sampleRate))), newNumChannels);
    clear();
}


void AllPass::clear()
{
    delayLine.clear();

    feedbackState[0] = 0.f;
    feedbackState[1] = 0.f;
}

void AllPass::setCoeff(const float newCoeff)
{
    coeff = std::clamp(newCoeff, -0.95f, 0.95f);
}

void AllPass::setDelayTime(float newDelayMs)
{
    delayTimeMs = std::fmax(newDelayMs, 1.f);;
    delayLine.setDelaySamples(static_cast<unsigned int>(std::round(newDelayMs * static_cast<float>(0.001 * sampleRate))));
}

void AllPass::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{    

    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Compute the input to the delay line 
        //
        float delayIn[2] { 0.f, 0.f };
        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            delayIn[ch] = -coeff * feedbackState[ch] + input[ch][n];
        }

        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            // Compute the output
            output[ch][n] = coeff * delayIn[ch] + feedbackState[ch];
        }

        // Process delay
        delayLine.process(feedbackState, delayIn, numChannels);
        
    }

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

float AllPass::getSample(unsigned int channel, float index)
{
    return delayLine.getSample(channel, index);
}

}