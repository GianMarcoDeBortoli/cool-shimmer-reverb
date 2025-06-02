#pragma once

#include <JuceHeader.h>
#include "DattorroDelayLine.h"
#include "LeakyIntegrator.h"
#include "DattorroAllPass.h"
#include "LFO.h"

namespace DSP
{

class DattorroReverb
{
public:
    // Constructor
    DattorroReverb(
        double initSampleRate = 29761,               // Sample rate in Hz
        unsigned int initNumChannels = 2u,           // Number of channels (1 or 2)
        unsigned int initPreDelaySamples = 9600u,    // Predelay in samples
        float initToneControlCoeff = 0.9995f,        // Tone control coefficient
        float initInputDiffusionCoeff_1_2 = 0.750f,  // Input diffuser 1 & 2 coefficient
        float initInputDiffusionCoeff_3_4 = 0.625f,  // Input diffuser 3 & 4 coefficient
        LFO::LFOType initLFOType = LFO::Sin,         // LFO wave type
        float initLFOFreqHz = 0.5f,                  // LFO frequency in Hz
        float initLFODepthMs = 2.f,                  // LFO depth in milliseconds
        float initLFOOffsetMs = 1.f,                 // LFO offset in milliseconds
        float initDecayDiffusionCoeff_1 = 0.70f,     // Decay diffuser 1 left & right coefficient
        float initDampingFilterCoeff = 0.005f,       // Damping filter coefficient
        float initDecayCoeff = 0.50f,                // Damping coefficient
        float initDecayDiffusionCoeff_2 = 0.50f      // Decay diffuser 2 left & right coefficient
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

    void setDecayCoeff(float newCoeff);

    void setDecayDiffusionCoeff_2(float newDiffCoeff);

    // ==================================================
    // Constants for the Dattorro Reverb algorithm
    // Number of channels
    static constexpr unsigned int MaxChannels { 2 };
    // Input diffusers delay line lengths in milliseconds
    static constexpr float inputDiffDelayMs_1 { 142.f / 30000.f * 1000.f };
    static constexpr float inputDiffDelayMs_2 { 107.f / 30000.f * 1000.f };
    static constexpr float inputDiffDelayMs_3 { 379.f / 30000.f * 1000.f };
    static constexpr float inputDiffDelayMs_4 { 277.f / 30000.f * 1000.f };
    // Decay diffusers delay line lengths in milliseconds
    static constexpr float decayDiffDelayMs_left_1 { 672.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_right_1 { 908.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_left_2 { 1800.f / 30000.f * 1000.f };
    static constexpr float decayDiffDelayMs_right_2 { 2656.f / 30000.f * 1000.f };
    // Delay lines delay lengths in milliseconds
    static constexpr float delayMs_left_1 { 4453.f / 30000.f * 1000.f };
    static constexpr float delayMs_left_2 { 3720.f / 30000.f * 1000.f };
    static constexpr float delayMs_right_1 { 4217.f / 30000.f * 1000.f };
    static constexpr float delayMs_right_2 { 3163.f / 30000.f * 1000.f };
    // Indices from which we tap the output
    // LEFT CHANNEL
    static constexpr float tapOutMs_left_1 { 266.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_2 { 2974.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_3 { 1913.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_4 { 1996.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_5 { 1990.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_6 { 187.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_left_7 { 1066.f / 30000.f * 1000.f };
    // RIGHT CHANNEL
    static constexpr float tapOutMs_right_1 { 353.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_2 { 3627.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_3 { 1228.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_4 { 2673.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_5 { 2111.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_6 { 335.f / 30000.f * 1000.f };
    static constexpr float tapOutMs_right_7 { 121.f / 30000.f * 1000.f };

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
    // Delay lines 1
    DSP::DelayLine delay_left_1;
    DSP::DelayLine delay_right_1;
    // Damping
    DSP::LeakyIntegrator dampingFilter;
    // Decay
    float decayCoeff;
    // Decay Diffusers 2
    DSP::AllPass decayDiffuser_left_2;
    DSP::AllPass decayDiffuser_right_2;
    // Delay lines 2
    DSP::DelayLine delay_left_2;
    DSP::DelayLine delay_right_2;
    // Feedback state
    float feedbackState[2] { 0.f, 0.f };
};

}