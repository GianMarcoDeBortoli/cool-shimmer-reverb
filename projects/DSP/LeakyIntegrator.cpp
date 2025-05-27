#include "LeakyIntegrator.h"

#include <algorithm>

namespace DSP
{

LeakyIntegrator::LeakyIntegrator(const float initCoeff) :
    coeffRamp(0.01f)
{
    coeff = std::clamp(initCoeff, 0.f, 1.f);
    coeffRamp.setTarget(coeff, true);
}

LeakyIntegrator::~LeakyIntegrator()
{
}

void LeakyIntegrator::prepare(double sampleRate)
{
    coeffRamp.prepare(sampleRate, true, coeff);
}


void LeakyIntegrator::clear()
{
    feedbackState[0] = 0.f;
    feedbackState[1] = 0.f;
}

void LeakyIntegrator::setCoeff(const float newCoeff)
{
    coeff = std::clamp(newCoeff, 0.f, 1.f);
    coeffRamp.setTarget(coeff);
}

void LeakyIntegrator::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{    

    for (unsigned int n = 0; n < numSamples; ++n)
    {
        float toOutput[2] { 0.f, 0.f };
        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            output[ch][n] = coeffRamp.getNext() * input[ch][n] + (1.f - coeffRamp.getNext()) * feedbackState[ch];
            feedbackState[ch] = output[ch][n];
        }
    }

}

void LeakyIntegrator::process(float* output, const float* input, unsigned int numChannels)
{
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        output[ch] = coeffRamp.getNext() * input[ch] + (1.f - coeffRamp.getNext()) * feedbackState[ch];
        feedbackState[ch] = output[ch];
    }

}

}