#include "AllPass.h"

namespace DSP
{

AllPass::AllPass(float maxTimeMs, unsigned int maxNumChannels) :
    delayLine(static_cast<unsigned int>(std::ceil(static_cast<float>(std::fmax(maxTimeMs, 1.f)) * static_cast<float>(0.001 * sampleRate))),
               static_cast<unsigned int>(std::fmax(maxNumChannels, 1u))),
    delayTimeMs { 20.0f },
    coeff { 0.5f }
{
}

AllPass::~AllPass()
{
}

void AllPass::prepare(double newSampleRate, float maxTimeMs, unsigned int numChannels)
{
    sampleRate = newSampleRate;

    delayLine.prepare(static_cast<unsigned int>(std::round(maxTimeMs * static_cast<float>(0.001 * sampleRate))), MaxChannels);
    delayLine.setDelaySamples(1); // Keep at least 1 sample minimum fixed delay

    setCoeff(0.5f);

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
    coeff = newCoeff;
}

void AllPass::setDelayTime(float newDelayMs)
{
    delayTimeMs = newDelayMs;
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

float AllPass::getSample(unsigned int channel, float index)
{
    return delayLine.getSample(channel, index);
}

}