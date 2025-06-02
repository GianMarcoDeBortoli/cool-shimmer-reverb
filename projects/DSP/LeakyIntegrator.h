#pragma once

#include "Ramp.h"

namespace DSP
{

class LeakyIntegrator
{
public:
    // Constructor
    LeakyIntegrator(float initCoeff);
    // Destructor
    ~LeakyIntegrator();

    // No copy semantics
    LeakyIntegrator(const LeakyIntegrator&);
    const LeakyIntegrator& operator=(const LeakyIntegrator&);
    // No move semantics
    LeakyIntegrator(LeakyIntegrator&&) = delete;
    const LeakyIntegrator& operator=(LeakyIntegrator&&) = delete;

    // ======================
    // Prepare coefficient ramp
    void prepare(double sampleRate);

    // Clear content of internal buffer
    void clear();

    // Set new coefficient
    void setCoeff(float newCoeff);

    // Process block of audio 
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Process single sample of audio
    void process(float* output, const float* input, unsigned int numChannels);
    // ======================

private:
    DSP::Ramp<float> coeffRamp;
    float coeff;

    // one state per channel
    float feedbackState[2] { 0.f, 0.f };
};

}