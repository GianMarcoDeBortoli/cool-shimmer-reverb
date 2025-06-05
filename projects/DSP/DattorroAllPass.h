#pragma once

#include "DattorroDelayLine.h"

namespace DSP
{

class AllPass
{
public:
    AllPass(float initDelaySamples, float initCoeff, unsigned int initNumChannels);
    AllPass() = delete; // Prevent default constructor

    ~AllPass();

    AllPass(const AllPass&);    // copy constructor
    const AllPass& operator=(const AllPass&); // assignment operator

    AllPass(AllPass&&) = delete; // move constructor
    const AllPass& operator=(AllPass&&) = delete; // move assignment operator

    // =======================================

    // Set new coefficient
    void setCoeff(const float newCoeff);

    // Set delay time in ms
    void setDelayTime(float newDelaySamples);

    // Get sample from delay line at requested index
    float getSample(unsigned int channel, unsigned int index);

    // =======================================

    // Update sample rate, reallocates and clear internal buffers
    void prepare(unsigned int numChannels);

    // Clear content of internal buffer
    void clear();

    // Process single sample of audio
    void process(float* output, const float* input, unsigned int numChannels);

    // Process single sample of audio with modulation
    void process(float* output, const float* input, unsigned int numChannels, const float* modInput);

    // =======================================

    static constexpr unsigned int MaxChannels { 2 };

private: 
    // vector of delay lines of all sections
    DSP::DelayLine delayLine;
    
    float delayTimeSamples;
    float coeff;

    // one state per channel
    float feedbackState[2] { 0.f, 0.f };
};

}