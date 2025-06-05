#pragma once

#include <JuceHeader.h>

#include "DelayLine.h"
#include "Ramp.h"
#include "juce_core/juce_core.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String MethodOn { "method test" };
        static const juce::String Index { "index" };
        static const juce::String OffsetMs { "offset ms" };
        static const juce::String DepthMs { "depth ms" };
        static const juce::String ModeRateHz { "mod rate hz" };
        static const juce::String ModType { "mod type" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String MethodOn { "Test getSample method" };
        static const juce::String Index { "getSample index" };
        static const juce::String OffsetMs { "Offset in ms" };
        static const juce::String DepthMs { "Modulation depth in ms" };
        static const juce::String ModeRateHz { "Modulation rate in Hz" };
        static const juce::String ModType { "Modulation type" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
        static const juce::String MethodOff { "Off" };
        static const juce::String MethodOn { "On" };

        static constexpr float IndexDefault { 2000.f };
        static constexpr float IndexMin { 0.f };
        static constexpr float IndexMax { 5000.f };
        static constexpr float IndexInc { 1.f };
        static constexpr float IndexSkw { 1.f };

        static constexpr float OffsetDefault { 10.f };
        static constexpr float OffsetMin { 0.f };
        static constexpr float OffsetMax { 50.f };
        static constexpr float OffsetInc { 1.f };
        static constexpr float OffsetSkw { 1.f };

        static constexpr float DepthDefault { 10.f };
        static constexpr float DepthMin { 0.f };
        static constexpr float DepthMax { 20.f };
        static constexpr float DepthInc { 1.f };
        static constexpr float DepthSkw { 1.f };

        static constexpr float ModRateDefault { 1.f };
        static constexpr float ModRateMin { 0.f };
        static constexpr float ModRateMax { 10.f };
        static constexpr float ModRateInc { 0.1f };
        static constexpr float ModRateSkw { 0.1f };

        static const juce::String ModTypeSin { "Sine" };
        static const juce::String ModTypeTri { "Triangle" };
    }
}

class DelayLineProcessor : public juce::AudioProcessor
{
public:
    DelayLineProcessor();
    ~DelayLineProcessor() override;

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
    static constexpr unsigned int MaxDelay { 10000 };

    enum ModulationType : unsigned int
    {
        Sin = 0,
        Tri
    };

private:
    double sampleRate { 48000.0 };

    mrta::ParameterManager parameterManager;
    DSP::DelayLine delayLine;
    
    DSP::Ramp<float> enableRamp;
    bool enabled { true };

    DSP::Ramp<float> methodRamp;
    bool method { false };
    float index {0.f};
    DSP::Ramp<float> indexRamp;

    juce::AudioBuffer<float> fxBuffer;
    juce::AudioBuffer<float> methodBuffer;

    DSP::Ramp<float> offsetRamp;
    DSP::Ramp<float> modDepthRamp;

    float phaseState { 0.f };
    float phaseInc { 0.f };

    float offsetMs { 0.f };
    float modDepthMs { 0.f };
    float modRate { 0.f };

    ModulationType modType { Sin };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayLineProcessor)
};