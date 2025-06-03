#pragma once

#include <JuceHeader.h>

#include "DattorroReverb.h"
#include "Ramp.h"
#include "juce_core/juce_core.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String Mix { "mix" };
        static const juce::String PreDelay { "predelay" };
        static const juce::String ToneControl { "tone_control" };
        static const juce::String DampFilterCoeff { "damping_filter_coeff" };
        static const juce::String DecayCoeff { "decay_coeff" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Mix { "Mix" };
        static const juce::String PreDelay { "Predelay" };
        static const juce::String ToneControl { "Tone Control" };
        static const juce::String DampFilterCoeff { "Damping Filter Coefficient" };
        static const juce::String DecayCoeff { "Decay Coefficient" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
        static constexpr bool EnabledDefault { true };

        static constexpr float MixDefault { 0.25f };
        static constexpr float MixMin { 0.f };
        static constexpr float MixMax { 1.f };
        static constexpr float MixInc { 0.01f };
        static constexpr float MixSkw { 0.5f };
        
        static constexpr float PreDelayDefault { 20.f };
        static constexpr float PreDelayMin { 0.f };
        static constexpr float PreDelayMax { 100.f };
        static constexpr float PreDelayInc { 1.f };
        static constexpr float PreDelaySkw { 1.f };

        static constexpr float ToneControlDefault { 0.3f };
        static constexpr float ToneControlMin { 0.f };
        static constexpr float ToneControlMax { 1.f };
        static constexpr float ToneControlInc { 0.01f };
        static constexpr float ToneControlSkw { 0.5f };

        static constexpr float DampFilterCoeffDefault { 0.005f };
        static constexpr float DampFilterCoeffMin { 0.f };
        static constexpr float DampFilterCoeffMax { 1.f };
        static constexpr float DampFilterCoeffInc { 0.01f };
        static constexpr float DampFilterCoeffSkw { 0.5f };

        static constexpr float DecayCoeffDefault { 0.5f };
        static constexpr float DecayCoeffMin { 0.f };
        static constexpr float DecayCoeffMax { 1.f };
        static constexpr float DecayCoeffInc { 0.01f };
        static constexpr float DecayCoeffSkw { 0.5f };
    }

    namespace Units
    {
        static const juce::String Ms { "ms" };
        static const juce::String Hz { "Hz" };
    }
}

class DattorroReverbProcessor : public juce::AudioProcessor
{
public:
    DattorroReverbProcessor();
    ~DattorroReverbProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    mrta::ParameterManager& getParameterManager() { return parameterManager; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    //==============================================================================

    static constexpr int MaxChannels { 2 };

private:
    mrta::ParameterManager parameterManager;
    // Sample rate
    double sampleRate { 48000.0 };
    //Enable/Disable the effect
    DSP::Ramp<float> enableRamp;
    bool enabled { true };
    // Mix dry/wet
    DSP::Ramp<float> mixRamp;
    float mix { 1.0f };
    // Dattorro Reverb instance
    DSP::DattorroReverb dattorroReverb;
    juce::AudioBuffer<float> revBuffer;
    // Predelay
    float preDelayMs;
    // Tone control
    float toneControl;
    // Damping
    float dampingFilterCoeff;
    // Decay
    float decayCoeff;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DattorroReverbProcessor)
};