#pragma once

#include <JuceHeader.h>
#include "Shimmer.h"
#include "KeithBarrReverb.h"
#include "DattorroReverb.h"
#include "ParametricEqualizer.h"
#include "Ramp.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String Mix { "plugin_mix" };

        // Pitch Shifter Parameters
        static const juce::String Buildup { "buildup" };
        static const juce::String Shift1 { "shift1" };
        static const juce::String Shift2 { "shift2" };
        static const juce::String Amount {"amount"};

        // Keith Barr Reverb Parameters
        static const juce::String Damping { "damping" };

        // Parametric Equalizer Parameters


        // Jon Dattorro's reverb parameters
        static const juce::String Brightness { "rev_brightness" };
        static const juce::String Decay { "rev_decay" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Mix { "Dry/Mix" };

        // Pitch Shifter Parameters
        static const juce::String Buildup { "Buildup" };
        static const juce::String Shift1 { "Shift 1" };
        static const juce::String Shift2 { "Shift 2" };
        static const juce::String Amount {"Amount" };   

        // Keith Barr Reverb Parameters
        static const juce::String Damping { "Damping" };

        // Jon Dattorro's reverb parameters
        static const juce::String Brightness { "Brightness" };
        static const juce::String Decay { "Decay" };
    }

    namespace Ranges
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
        static constexpr bool EnabledDefault { true };

        static constexpr float MixDefault { 0.5f };
        static constexpr float MixMin { 0.f };
        static constexpr float MixMax { 1.f };
        static constexpr float MixInc { 0.01f };
        static constexpr float MixSkw { 1.f };

        // Pitch Shifter Parameters
        static constexpr float BuildupDefault { 10.f };
        static constexpr float BuildupMin { 1.f };
        static constexpr float BuildupMax { 100.f };
        static constexpr float BuildupInc { 0.01f };
        static constexpr float BuildupSkw { 0.5f };

        static constexpr float Shift1Default { 2.f };
        static constexpr float Shift1Min { 0.25f };
        static constexpr float Shift1Max { 2.f };
        static constexpr float Shift1Inc { 0.01f };
        static constexpr float Shift1Skw { 1.0f };

        static constexpr float Shift2Default { 0.5f };
        static constexpr float Shift2Min { 0.25f };
        static constexpr float Shift2Max { 2.f };
        static constexpr float Shift2Inc { 0.01f };
        static constexpr float Shift2Skw { 1.0f };

        static constexpr float AmountDefault { 0.25f };
        static constexpr float AmountMin { 0.f };
        static constexpr float AmountMax { 0.5f };
        static constexpr float AmountInc { 0.01f };
        static constexpr float AmountSkw { 1.0f };

        // Keith Barr Reverb Parameters
        static constexpr float DampCoeffDefault { 0.5f };
        static constexpr float DampCoeffMin { 0.f };
        static constexpr float DampCoeffMax { 0.9f }; // Avoid feedback to saturate
        static constexpr float DampCoeffInc { 0.01f };
        static constexpr float DampCoeffSkw { 1.0f };
        
        // Jon Dattorro's reverb parameters
        static constexpr float BrightnessDefault { 0.5f };
        static constexpr float BrightnessMin { 0.f };
        static constexpr float BrightnessMax { 1.f };
        static constexpr float BrightnessInc { 0.01f };
        static constexpr float BrightnessSkw { 1.f };

        static constexpr float DecayDefault { 0.5f };
        static constexpr float DecayMin { 0.f };
        static constexpr float DecayMax { 1.f };
        static constexpr float DecayInc { 0.01f };
        static constexpr float DecaySkw { 1.f };
    }

    namespace Units
    {
        static const juce::String Ms { "ms" };
        static const juce::String Hz { "Hz" };
        static const juce::String dB { "dB" };
    }
}

class ShimmerAudioProcessor : public juce::AudioProcessor
{
public:
    ShimmerAudioProcessor();
    ~ShimmerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================

    static const unsigned int MaxDelaySizeSamples { 1 << 12 };
    static const unsigned int MaxChannels { 2 };
    static const unsigned int MaxProcessBlockSamples{ 32 };

private:
    mrta::ParameterManager parameterManager;
    // Sample rate
    double sampleRate { 48000.0 };
    //Enable/Disable the effect
    DSP::Ramp<float> enableRamp;
    bool enabled;
    // Mix dry/wet
    DSP::Ramp<float> mixRamp;
    float mix;
    // Shimmer effect
    DSP::Shimmer shimmer;
    // Pitch shifter parameters
    float shift1;
    float shift2;
    DSP::Ramp<float> amountRamp;
    float amount;
    float buildUp;
    // Keith Barr's reverb effect
    DSP::KeithBarrReverb KBReverb;
    DSP::ParametricEqualizer eq;
    // Keith Barr's reverb parameters
    float dampingCoeff;
    // Jon Dattorro's reverb effect
    DSP::DattorroReverb dattorroReverb;
    // Jon Dattorro's reverb parameters
    float brightness;
    float decay;
    DSP::Ramp<float> buildupRamp;

    // Internal buffer for processing
    juce::AudioBuffer<float> shimmerBuffer;
    juce::AudioBuffer<float> reverbBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShimmerAudioProcessor)
};