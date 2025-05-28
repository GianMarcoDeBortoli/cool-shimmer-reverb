#pragma once

#include <JuceHeader.h>

#include "AllPass.h"
#include "Ramp.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String Coeff { "coeff" };
        static const juce::String DelayTimeMs { "delay time ms" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Coeff { "Coefficient" };
        static const juce::String DelayTimeMs { "Delay" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };

        static constexpr float CoeffDefault { 0.5f };
        static constexpr float CoeffMin { -1.f };
        static constexpr float CoeffMax { 1.f };
        static constexpr float CoeffInc { 0.01f };
        static constexpr float CoeffSkw { 0.5f };

        static constexpr float DelayTimeDefault { 10.f };
        static constexpr float DelayTimeMin { 1.f };
        static constexpr float DelayTimeMax { 20.f };
        static constexpr float DelayTimeInc { 0.01f };
        static constexpr float DelayTimeSkw { 0.5f };
    }

    namespace Units
    {
        static const juce::String Ms { "ms" };
    }
}

class AllPassFilterProcessor : public juce::AudioProcessor
{
public:
    AllPassFilterProcessor();
    ~AllPassFilterProcessor() override;

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
    DSP::AllPass AllPassFilter;
    DSP::Ramp<float> enableRamp;

    float delayTimeMs;
    float coeff;

    bool enabled { true };
    juce::AudioBuffer<float> fxBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AllPassFilterProcessor)
};