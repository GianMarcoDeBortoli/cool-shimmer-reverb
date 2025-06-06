#pragma once

#include <JuceHeader.h>
#include "DattorroDelayLine.h"
#include "LeakyIntegrator.h"
#include "DattorroAllPass.h"
#include "LFO.h"
#include "Ramp.h"

namespace DSP
{

class DattorroReverb
{
public:
    // Constructor
    DattorroReverb(
        double initSampleRate = 29761,               // Sample rate in Hz
        unsigned int initNumChannels = 2u,           // Number of channels (1 or 2)
        float initDampingFilterCoeff = 0.005f,       // Damping filter coefficient
        float initDecayCoeff = 0.50f                 // Damping coefficient
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
    void prepare(double newSampleRate, unsigned int newNumChannels);

    // Process block of audio without modulation
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // ==================================================
    // Set methods
    void setBandwidth(float newCoeff);
    void setBrightness(float newCoeff);
    void setDecay(float newCoeff);

    // ==================================================
    // Constants for the Dattorro Reverb algorithm
    // Number of channels
    static constexpr unsigned int MaxChannels { 2 };
    // Sample rate in Hz used in the original algorithm
    static constexpr float sampleRate_Original { 30000.f };
    // Predelay
    static constexpr float preDelayMs { 20.f };
    // Tone control coefficient
    static constexpr float toneControlCoeff { 0.9995f };
    // Input diffusers delay line lengths in milliseconds
    static constexpr float inputDiffCoeff_1_2 { 0.750f };
    // static constexpr float inputDiffCoeff_1_2 { 0.f };
    static constexpr float inputDiffDelayMs_1 { 142.f / sampleRate_Original * 1000.f };
    static constexpr float inputDiffDelayMs_2 { 107.f / sampleRate_Original * 1000.f };
    static constexpr float inputDiffCoeff_3_4 { 0.625f };
    // static constexpr float inputDiffCoeff_3_4 { 0.f };
    static constexpr float inputDiffDelayMs_3 { 379.f / sampleRate_Original * 1000.f };
    static constexpr float inputDiffDelayMs_4 { 277.f / sampleRate_Original * 1000.f };
    // Decay diffusers delay line lengths in milliseconds
    static constexpr float decayDiffCoeff_1 { 0.70f };
    // static constexpr float decayDiffCoeff_1 { 0.f };
    static constexpr float decayDiffDelayMs_left_1  {  672.f / sampleRate_Original * 1000.f };
    static constexpr float decayDiffDelayMs_right_1 {  908.f / sampleRate_Original * 1000.f };
    static constexpr float decayDiffCoeff_2 { 0.50f };
    // static constexpr float decayDiffCoeff_2 { 0.f };
    static constexpr float decayDiffDelayMs_left_2  { 1800.f / sampleRate_Original * 1000.f };
    static constexpr float decayDiffDelayMs_right_2 { 2656.f / sampleRate_Original * 1000.f };
    // Delay lines delay lengths in milliseconds
    static constexpr float delayMs_left_1  { 4453.f / sampleRate_Original * 1000.f };
    static constexpr float delayMs_left_2  { 3720.f / sampleRate_Original * 1000.f };
    static constexpr float delayMs_right_1 { 4217.f / sampleRate_Original * 1000.f };
    static constexpr float delayMs_right_2 { 3163.f / sampleRate_Original * 1000.f };
    // Indices from which we tap the output
    // LEFT CHANNEL
    static constexpr float tapOutMs_left_1 {  266.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_left_2 { 2974.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_left_3 { 1913.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_left_4 { 1996.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_left_5 { 1990.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_left_6 {  187.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_left_7 { 1066.f / sampleRate_Original * 1000.f };
    // RIGHT CHANNEL
    static constexpr float tapOutMs_right_1 {  353.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_right_2 { 3627.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_right_3 { 1228.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_right_4 { 2673.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_right_5 { 2111.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_right_6 {  335.f / sampleRate_Original * 1000.f };
    static constexpr float tapOutMs_right_7 {  121.f / sampleRate_Original * 1000.f };

    // LFO
    static constexpr LFO::LFOType lfoType { LFO::Sin };     // LFO wave type
    static constexpr float lfoFreqHz { 0.5f };              // LFO frequency in Hz
    static constexpr float lfoDepthMs { 16.f / sampleRate_Original * 1000.f };  // LFO depth in milliseconds
    static constexpr float lfoOffsetMs { 0.f };            // LFO offset in milliseconds

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
    DSP::Ramp<float> decayCoeffRamp;
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