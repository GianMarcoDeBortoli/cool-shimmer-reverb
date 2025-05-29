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
        double initSampleRate,                 // Sample rate in Hz
        unsigned int initNumChannels,          // Number of channels (1 or 2)
        unsigned int initPreDelaySamples,      // Predelay
        float initToneControlCoeff,            // Tone control
        float initInputDiffusionCoeff_1_2,     // Input diffuser 1 & 2
        float initInputDiffusionCoeff_3_4,     // Input diffuser 3 & 4
        LFO::LFOType initLFOType,              // LFO type
        float initLFOFreqHz,                   // LFO frequency in Hz
        float initLFODepthMs,                  // LFO depth in milliseconds
        float initLFOOffsetMs,                 // LFO offset in milliseconds
        float initDecayDiffusionCoeff_1,       // Decay diffuser 1 left & right
        float initDampingFilterCoeff,          // Damping filter coefficient
        float initDampingCoeff,                // Damping coefficient
        float initDecayDiffusionCoeff_2       // Decay diffuser 2 left & right
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
    // Set methods
    void setPreDelay(unsigned int newPreDelaySamples);

    void setToneControl(float newCoeff);

    void setInputDiffusionCoeff_1(float newDiffCoeff);
    void setInputDiffusionCoeff_2(float newDiffCoeff);

    void setLFOtype(LFO::LFOType newModType);
    void setLFOfreqHz(float newModRateHz);
    void setLFOdepthMs(float newModDepthMs);
    void setLFOoffsetMs(float newModOffsetMs);

    void setDecayDiffusionCoeff_1(float newDiffCoeff);

    void setDampingFilterCoeff(float newCoeff);

    void setDampingCoeff(float newCoeff);

    void setDecayDiffusionCoeff_2(float newDiffCoeff);

    // ==================================================
    // Constants for the Dattorro Reverb algorithm
    static constexpr unsigned int MaxChannels { 2 };
    static constexpr float inputDiffDelayMs_1 { 142.f / 30000.f * 1000.f };
    static constexpr float inputDiffDelayMs_2 { 107.f / 30000.f * 1000.f };
    static constexpr float inputDiffDelayMs_3 { 379.f / 30000.f * 1000.f };
    static constexpr float inputDiffDelayMs_4 { 277.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_left_1 { 672.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_right_1 { 908.f / 30000.f * 1000.f };
    static constexpr float delayMs_left_1 { 4453.f / 30000.f * 1000.f };
    static constexpr float delayMs_right_1 { 4217.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_1 { 353.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_2 { 3627.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_1 { 266.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_2 { 2974.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_left_2 { 1800.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_right_2 { 2656.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_3 { 1913.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_3 { 1228.f / 30000.f * 1000.f };

private:
    double sampleRate;
    // --------- FEEDFORWARD ---------
    // Predelay
    DSP::DelayLine preDelay;
    // Tone control
    DSP::LeakyIntegrator toneControl;
    // Input Diffusers
    DSP::AllPass inputDiffuser_1;
    DSP::AllPass inputDiffuser_2;
    DSP::AllPass inputDiffuser_3;
    DSP::AllPass inputDiffuser_4;
    // ---------- RECURSION ----------
    // LFO for allpass delay line modulation
    DSP::LFO lfo;
    // Decay Diffusers 1
    DSP::AllPass decayDiffuser_left_1;
    DSP::AllPass decayDiffuser_right_1;
    // Delays
    DSP::DelayLine delay_left_1;
    DSP::DelayLine delay_right_1;
    // Damping
    DSP::LeakyIntegrator dampingFilter;
    float dampingCoeff;
    // Decay Diffusers 2
    DSP::AllPass decayDiffuser_left_2;
    DSP::AllPass decayDiffuser_right_2;
};

}