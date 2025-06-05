#pragma once

#include <JuceHeader.h>
#include "Shimmer.h"
#include "Ramp.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Buildup { "buildup" };
        static const juce::String Shift1 { "shift1" };
        static const juce::String Shift2 { "shift2" };
        static const juce::String Amount {"amount"};
    }

    namespace Name
    {
        static const juce::String Buildup { "Buildup" };
        static const juce::String Shift1 { "Shift 1" };
        static const juce::String Shift2 { "Shift 2" };
        static const juce::String Amount {"Amount" };   
    }

    namespace Ranges
    {
        static constexpr float BuildupMin { 1.f };
        static constexpr float BuildupMax { 2500.f };
        static constexpr float BuildupInc { 0.01f };
        static constexpr float BuildupSkw { 0.5f };

        static constexpr float Shift1Min { 0.25f };
        static constexpr float Shift1Max { 2.f };
        static constexpr float Shift1Inc { 0.01f };
        static constexpr float Shift1Skw { 1.0f };

        static constexpr float Shift2Min { 0.25f };
        static constexpr float Shift2Max { 2.f };
        static constexpr float Shift2Inc { 0.01f };
        static constexpr float Shift2Skw { 1.0f };

        static constexpr float AmountDefault { 0.5f };
        static constexpr float AmountMin { 0.f };
        static constexpr float AmountMax { 1.f };
        static constexpr float AmountInc { 0.01f };
        static constexpr float AmountSkw { 1.0f };
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
    DSP::Shimmer shimmer;

    DSP::Ramp<float> amountRamp;
    // DSP::Ramp<float> wetRamp;
    // DSP::Ramp<float> dryRamp;

    // float enabled { 1.f };
    // float mix { 0.5f };

    juce::AudioBuffer<float> fxBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShimmerAudioProcessor)
};