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
        static const juce::String Mix { "plugin_mix" };
        // Pitch shifter parameters
        static const juce::String Shift1 { "pitchshift_1" };
        static const juce::String Shift2 { "pitchshift_2" };
        // Keith Barr's reverb parameters
        static const juce::String ShiftAmount { "pitchshift_amount" };
        static const juce::String BuildUp { "rev_build_up" };
        static const juce::String Size { "rev_size" };
        static const juce::String Brightness { "rev_brightness" };
        // Jon Dattorro's reverb parameters
        static const juce::String Decay { "rev_decay" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Mix { "Dry/Mix" };
        // Pitch shifter parameters
        static const juce::String Shift1 { "Shift 1" };
        static const juce::String Shift2 { "Shift 2" };
        // Keith Barr's reverb parameters
        static const juce::String ShiftAmount { "Amount" };
        static const juce::String BuildUp { "Build Up" };
        static const juce::String Size { "Size" };
        static const juce::String Brightness { "Brightness" };
        // Jon Dattorro's reverb parameters
        static const juce::String Decay { "Decay" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
        static constexpr bool EnabledDefault { true };

        static constexpr float MixDefault { 0.5f };
        static constexpr float MixMin { 0.f };
        static constexpr float MixMax { 1.f };
        static constexpr float MixInc { 0.01f };
        static constexpr float MixSkw { 1.f };

        // Pitch shifter parameters
        static constexpr float ShiftDefault { 0.5f };
        static constexpr float ShiftMin { 0.f };
        static constexpr float ShiftMax { 1.f };
        static constexpr float ShiftInc { 0.01f };
        static constexpr float ShiftSkw { 1.f };

        // Keith Barr's reverb parameters
        static constexpr float ShiftAmountDefault { 0.5f };
        static constexpr float ShiftAmountMin { 0.f };
        static constexpr float ShiftAmountMax { 1.f };
        static constexpr float ShiftAmountInc { 0.01f };
        static constexpr float ShiftAmountSkw { 1.f };

        static constexpr float BuildUpDefault { 20.f };
        static constexpr float BuildUpMin { 0.f };
        static constexpr float BuildUpMax { 100.f };
        static constexpr float BuildUpInc { 0.1f };
        static constexpr float BuildUpSkw { 0.5f };

        static constexpr float SizeDefault { 0.5f };
        static constexpr float SizeMin { 0.f };
        static constexpr float SizeMax { 1.f };
        static constexpr float SizeInc { 0.01f };
        static constexpr float SizeSkw { 1.f };

        static constexpr float BrightnessDefault { 0.5f };
        static constexpr float BrightnessMin { 0.f };
        static constexpr float BrightnessMax { 1.f };
        static constexpr float BrightnessInc { 0.01f };
        static constexpr float BrightnessSkw { 1.f };

        // Jon Dattorro's reverb parameters
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
    bool enabled;
    // Mix dry/wet
    DSP::Ramp<float> mixRamp;
    float mix;
    // Pitch shifter parameters
    float shift1;
    float shift2;
    // Keith Barr's reverb parameters
    DSP::Ramp<float> amountRamp;
    float amount;
    DSP::Ramp<float> buildUpRamp;
    float buildUpMs;
    float size;
    float brightness;
    // Jon Dattorro's Reverb parameters
    float decay;
    // Jon Dattorro's Reverb instance
    DSP::DattorroReverb dattorroReverb;
    juce::AudioBuffer<float> revBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DattorroReverbProcessor)
};