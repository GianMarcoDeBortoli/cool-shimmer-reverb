#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"
#include "AllPass.h"
#include "LeakyIntegrator.h"

namespace DSP
{

class DattorroReverb
{
public:
    // Constructor
    DattorroReverb(double sampleRate, unsigned int numChannels,
        float preDelayTimeMs, float bandwidth,
        float inputDiffusion1, float inputDiffusion2,
        float decayDiffusion1, float decayDiffusion2,
        float damping, float decay);
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
    void prepare(double sampleRate, unsigned int numChannels);

    // Process block of audio without modulation
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Process single sample of audio without modulation
    void process(float* output, const float* input, unsigned int numChannels);

    // Process block of audio with modulation
    void process(float* const* audioOutput, const float* const* audioInput, const float* const* modInput,
                 unsigned int numChannels, unsigned int numSamples);

    // Process single sample of audio with modulation
    void process(float* audioOutput, const float* audioInput, const float* modInput, unsigned int numChannels);

    // Set methods
    void setPreDelayTime(float newPreDelayMs);
    void setPreDiffusionBandwidth(float newBandwidth);
    void setInputDiffusion1(float newCoeff);
    void setInputDiffusion2(float newCoeff);
    void setDecayDiffusion1(float newCoeff);
    void setDamping(float newDamping);
    void setDecayDiffusion2(float newCoeff);
    void setDecayCoefficient(float newDecayCoeff);

    // ==================================================
    static constexpr float MaxChannels { 2 };

    static constexpr float MaxBandwidth { 0.999999f };
    static constexpr float MaxDiffusion{ 0.999999f };
    static constexpr float MaxDamping { 0.020f };
    static constexpr float MaxDecay { 0.999999f };

    static constexpr float InputDiffusion1a_Delay {142.f / 30000.f};
    static constexpr float InputDiffusion1b_Delay {107.f / 30000.f};
    static constexpr float InputDiffusion2a_Delay {379.f / 30000.f};
    static constexpr float InputDiffusion2b_Delay {277.f / 30000.f};
    static constexpr float DecayDiffusion1L_Delay {672.f / 30000.f};
    static constexpr float DecayDelay1L_Delay {4453.f / 30000.f};
    static constexpr float DecayDiffusion2L_Delay {1800.f / 30000.f};
    static constexpr float DecayDelay2L_Delay {3720.f / 30000.f};
    static constexpr float DecayDiffusion1R_Delay {908.f / 30000.f};
    static constexpr float DecayDelay1R_Delay {4217.f / 30000.f};
    static constexpr float DecayDiffusion2R_Delay {2656.f / 30000.f};
    static constexpr float DecayDelay2R_Delay {3163.f / 30000.f};

private:
    double sampleRate { 48000.0 };

    // Pre-diffusion phase
    DSP::DelayLine preDelay;
    float preDelayTimeMs;
    DSP::LeakyIntegrator preDiffusionFilter;
    // Diffusion phase
    DSP::AllPass inputDiffusion1a;
    DSP::AllPass inputDiffusion1b;
    DSP::AllPass inputDiffusion2a;
    DSP::AllPass inputDiffusion2b;
    // Decay phase left channel
    juce::AudioBuffer<float> recursionLeft;
    DSP::AllPass decayDiffusion1L;
    DSP::DelayLine decayDelay1L;
    DSP::LeakyIntegrator dampingL;
    DSP::AllPass decayDiffusion2L;
    DSP::DelayLine decayDelay2L;
    // Decay phase right channel
    juce::AudioBuffer<float> recursionRight;
    DSP::AllPass decayDiffusion1R;
    DSP::DelayLine decayDelay1R;
    DSP::LeakyIntegrator dampingR;
    DSP::AllPass decayDiffusion2R;
    DSP::DelayLine decayDelay2R;
    // Decay coefficient
    float decayCoeff;
};

}