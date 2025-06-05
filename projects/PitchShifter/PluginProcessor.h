#pragma once

#include <JuceHeader.h>
#include "GranularPitchShifter.h"

namespace Param
{
    namespace ID
    {
        static const juce::String PitchRatio { "pitch_ratio" };
    }

    namespace Name
    {
        static const juce::String PitchRatio { "Pitch Ratio" };
    }
}

class PitchShifterProcessor : public juce::AudioProcessor
{
public:
    PitchShifterProcessor();
    ~PitchShifterProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================

private:
    mrta::ParameterManager parameterManager;
    DSP::GranularPitchShifter pitchShifter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchShifterProcessor)
};
