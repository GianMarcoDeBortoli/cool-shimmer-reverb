#pragma once

#include "DelayLine.h"
#include "AllPass.h"


namespace DSP
{

class KeithBarrReverb
{
public:
    // Constructor
    KeithBarrReverb(
        unsigned int initNumChannels,                // Number of channels (1 or 2)
        float initDecayCoeff = 0.50f                // Damping coefficient        
    );
    KeithBarrReverb() = delete; // prevent default constructor
    // Destructor
    ~KeithBarrReverb();

    // No copy semantics
    KeithBarrReverb(const KeithBarrReverb&) = delete;
    const KeithBarrReverb& operator=(const KeithBarrReverb&) = delete;
    // No move semantics
    KeithBarrReverb(KeithBarrReverb&&) = delete;
    const KeithBarrReverb& operator=(KeithBarrReverb&&) = delete;

    // ==================================================
    // Clear method
    void clear();

    // Prepare method
    void prepare(double newSampleRate, unsigned int newNumChannels);

    // Process block of audio without modulation
    void process(float* const*  output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // ==================================================
    void setDampingFilterCoeff(float newCoeff);

    // ==================================================
    // Constants for the Keith Barr Reverb algorithm
    // Number of channels
    static constexpr unsigned int MaxChannels { 2 };
    // Sample rate in Hz 
    static constexpr float sampleRate_Original { 48000.f };
    // Input diffusers delay line lengths in milliseconds (Use the same as in Dattorro's reverb)
    static constexpr float inputDiffDelayMs_1 { 142.f / sampleRate_Original * 1000.f };
    static constexpr float inputDiffDelayMs_2 { 107.f / sampleRate_Original * 1000.f };
    static constexpr float inputDiffDelayMs_3 { 379.f / sampleRate_Original * 1000.f };
    static constexpr float inputDiffDelayMs_4 { 277.f / sampleRate_Original * 1000.f };
    // Ring all pass delay line lengths in milliseconds
    static constexpr float ringDelayMs_1 { 130.f / sampleRate_Original * 1000.f };
    static constexpr float ringDelayMs_2 { 83.f / sampleRate_Original * 1000.f };
    static constexpr float ringDelayMs_3 { 340.f / sampleRate_Original * 1000.f };
    static constexpr float ringDelayMs_4 { 250.f / sampleRate_Original * 1000.f };
    // Delay lines delay lengths in milliseconds
    static constexpr float delayMs_1  { 453.f / sampleRate_Original * 1000.f };
    static constexpr float delayMs_2  { 720.f / sampleRate_Original * 1000.f };
    static constexpr float delayMs_3 { 217.f / sampleRate_Original * 1000.f };
    static constexpr float delayMs_4 { 163.f / sampleRate_Original * 1000.f };

    // feedback / feedforward coefficients
    static constexpr float AllPassCoeff {  0.5f };


private:
    double sampleRate { 48000.0 };
    // --------- ALLPASS ---------
    // Input AllPass
    DSP::AllPass inputAllPass_1;
    DSP::AllPass inputAllPass_2;
    DSP::AllPass inputAllPass_3;
    DSP::AllPass inputAllPass_4;
    // Input AllPass
    DSP::AllPass ringAllPass_1;
    DSP::AllPass ringAllPass_2;
    DSP::AllPass ringAllPass_3;
    DSP::AllPass ringAllPass_4;


    // Damping
    float dampingCoeff;

    // --------- DELAYS ---------
    // Delay lines 2
    DSP::DelayLine delay_1;
    DSP::DelayLine delay_2;
    DSP::DelayLine delay_3;
    DSP::DelayLine delay_4;
    // States
    float feedbackState_1 { 0.f };
    float feedbackState_2 { 0.f };
    float feedbackState_3 { 0.f };
    float feedbackState_4 { 0.f };

};

}