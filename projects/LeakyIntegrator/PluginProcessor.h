#pragma once

#include <JuceHeader.h>

#include "LeakyIntegrator.h"
#include "Ramp.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String Coeff { "coeff" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String Coeff { "Coefficient" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
        static constexpr float CoeffDefault { 0.5f };
        static constexpr float CoeffMin { 0.0f };
        static constexpr float CoeffMax { 1.0f };
        static constexpr float CoeffInc { 0.001f };
        static constexpr float CoeffSkw { 0.5f };
    }
}

class LeakyIntegratorProcessor : public juce::AudioProcessor
{
public:
    LeakyIntegratorProcessor();
    ~LeakyIntegratorProcessor() override;

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
    DSP::LeakyIntegrator leakyIntegrator;
    
    DSP::Ramp<float> enableRamp;
    bool enabled { true };

    juce::AudioBuffer<float> fxBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeakyIntegratorProcessor)
};