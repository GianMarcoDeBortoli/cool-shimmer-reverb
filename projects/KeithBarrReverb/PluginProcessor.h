#pragma once

#include <JuceHeader.h>

#include "KeithBarrReverb.h"
#include "Ramp.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String DampingFactor { "damping_factor" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String DampingFactor { "Damping Factor" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };

        static constexpr float DampFilterCoeffDefault { 0.005f };
        static constexpr float DampFilterCoeffMin { 0.f };
        static constexpr float DampFilterCoeffMax { 1.f };
        static constexpr float DampFilterCoeffInc { 0.01f };
        static constexpr float DampFilterCoeffSkw { 0.5f };
    }

    // namespace Units
    // {
    //     static const juce::String Ms { "ms" };
    // }
}

class KeithBarrReverbProcessor : public juce::AudioProcessor
{
public:
    KeithBarrReverbProcessor();
    ~KeithBarrReverbProcessor() override;

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
    DSP::KeithBarrReverb KBReverb;
    DSP::Ramp<float> enableRamp;

    bool enabled { true };
    juce::AudioBuffer<float> fxBuffer;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeithBarrReverbProcessor)
};