#include "LFO.h"
#include <cmath>

namespace DSP
{

LFO::LFO(
    LFOType initType,
    float initFreq,
    float initDepthMs
) :
    type { initType },
    frequency { std::fmax(initFreq, 0.f) }, // avoid zero frequency
    depthRamp(0.01f)
{
    setFrequency(frequency);
    setDepth(initDepthMs);
    
    // reset states
    phaseState = 0.f;
}

LFO::~LFO()
{
}

void LFO::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;

    // update phase increment for new sample rate
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * frequency;

    // reset states
    phaseState = 0.f;
}

float LFO::process()
{
    float osc { 0.f };

    switch (type)
    {
    case Sin:
        osc = 0.5f + 0.5f * std::sin(phaseState);
        break;

    case Tri:
        osc = std::fabs(phaseState - static_cast<float>(M_PI)) / static_cast<float>(M_PI);
        break;

    default: break;
    }

    phaseState = std::fmod(phaseState + phaseInc, static_cast<float>(2 * M_PI));

    // Apply mod depth and offset ramps
    depthRamp.applyGain(&osc, 1u);

    return osc;
}

void LFO::setFrequency(float freqHz)
{
    frequency = std::fmax(freqHz, 0.f); // avoid zero frequency

    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * frequency;
}

void LFO::setType(LFOType newType)
{
    type = newType;

    // reset states
    phaseState = 0.f;
}

void LFO::setDepth(float depthMs)
{
    depthMs = std::fmax(depthMs, 0.f);
    depthRamp.setTarget(depthMs * static_cast<float>(0.001 * sampleRate), true);
}

}
