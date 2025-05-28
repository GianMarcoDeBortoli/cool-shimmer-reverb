#pragma once

#include "DelayLine.h"

namespace DSP
{

class AllPass
{
public:
    AllPass(float initDelayMs, float initCoeff, unsigned int initNumChannels);
    AllPass() = delete; // Prevent default constructor

    ~AllPass();

    AllPass(const AllPass&);    // copy constructor
    const AllPass& operator=(const AllPass&); // assignment operator

    AllPass(AllPass&&) = delete; // move constructor
    const AllPass& operator=(AllPass&&) = delete; // move assignment operator

    // Update sample rate, reallocates and clear internal buffers
    void prepare(double sampleRate, unsigned int numChannels);

    // Clear content of internal buffer
    void clear();

    // Set new coefficient
    void setCoeff(const float newCoeff);

    // Set delay time in ms
    void setDelayTime(float newDelayMs);

    // Process block of audio
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Process single sample of audio
    void process(float* output, const float* input, unsigned int numChannels);

    // Get sample from delay line at requested index
    float getSample(unsigned int channel, float index);

    static constexpr unsigned int MaxChannels { 2 };

private: 
    double sampleRate { 48000.0 };

    // vector of delay lines of all sections
    DSP::DelayLine delayLine;
    
    float delayTimeMs;
    float coeff;

    // one state per channel
    float feedbackState[2] { 0.f, 0.f };
};

}