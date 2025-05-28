#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"
#include "LeakyIntegrator.h"
#include "AllPass.h"
#include "LFO.h"

namespace DSP
{

class DattorroReverb
{
public:
    // Constructor
    DattorroReverb(
        unsigned int initNumChannels,
        unsigned int initPreDelaySamples,      // Predelay
        float initToneControlCoeff,            // Tone control
        float initInputDiffuserDelayMs_1,      // Input diffuser 1
        float initInputDiffuserDelayMs_2,      // Input diffuser 2
        float initInputDiffusionCoeff_1_2,     // Input diffuser 1 & 2
        float initInputDiffuserDelayMs_3,      // Input diffuser 3
        float initInputDiffuserDelayMs_4,      // Input diffuser 4
        float initInputDiffusionCoeff_3_4,     // Input diffuser 3 & 4
        LFO::LFOType initLFOType = LFO::Sin,   // LFO type
        float initLFOFreqHz = 0.5f,            // LFO frequency in Hz
        float initLFODepthMs = 0.f             // LFO depth in milliseconds
    );
    // Destructor
    ~DattorroReverb();

    // No copy semantics
    DattorroReverb(const DattorroReverb&) = delete;
    const DattorroReverb& operator=(const DattorroReverb&) = delete;
    // No move semantics
    DattorroReverb(DattorroReverb&&) = delete;
    const DattorroReverb& operator=(DattorroReverb&&) = delete;

    // ==================================================
    // Clear method
    void clear();

    // Prepare method
    void prepare(double newSampleRate, unsigned int newNumChannels, unsigned int newPreDelaySamples);

    // Process block of audio without modulation
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // ==================================================
    // Constants
    static constexpr unsigned int MaxChannels { 2 };

    // ==================================================
    // Set methods
    void setPreDelay(unsigned int newPreDelaySamples);
    void setToneControl(float newCoeff);
    void setInputDiffusionCoeff_1(float newDiffCoeff);
    void setInputDiffusionCoeff_2(float newDiffCoeff);

    void setLFOtype(LFO::LFOType newModType);
    void setLFOfreqHz(float newModRateHz);
    void setLFOdepthMs(float newModDepthMs);

private:
    // Predelay
    DSP::DelayLine preDelay;
    // Tone control
    DSP::LeakyIntegrator toneControl;
    // Input Diffusers
    DSP::AllPass inputDiffuser_1;
    DSP::AllPass inputDiffuser_2;
    DSP::AllPass inputDiffuser_3;
    DSP::AllPass inputDiffuser_4;
    // LFO for allpass delay line modulation
    DSP::LFO lfo;
};

}