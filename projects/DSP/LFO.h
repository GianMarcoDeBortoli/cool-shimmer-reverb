#pragma once

#include "Ramp.h"

namespace DSP
{

class LFO
{
public:
    enum LFOType : unsigned int
    {
        Sin = 0,
        Tri,
    };

    LFO(
        LFOType initType,
        float initFreq,
        float initDepthMs
    );
    ~LFO();

    LFO(const LFO&) = delete;
    LFO(LFO&&) = delete;
    const LFO& operator=(const LFO&) = delete;
    const LFO& operator=(LFO&&) = delete;

    // Update new sample rate
    void prepare(double sampleRate);

    // Generates the next sample of the LFO
    float process();

    // Select the waveform type
    void setType(LFOType type);

    // Set a new frequency for the LFO in Hz
    void setFrequency(float freqHz);

    // set a new depth for the LFO in Ms
    void setDepth(float depthMs);

private:
    double sampleRate { 48000.0 };

    float frequency { 0.f };
    LFOType type { Sin };

    float phaseState { 0.f };
    float phaseInc { 0.f };

    float depthMs { 0.f };
    DSP::Ramp<float> depthRamp;
};

}
