#include "LFO.h"
#include <cmath>

namespace DSP
{

LFO::LFO(
    LFOType initType,
    float initFreq,
    float initDepthMs,
    float initOffsetMs
) :
    type { initType },
    frequency { std::fmax(initFreq, 0.f) }, // avoid zero frequency
    depthRamp(0.01f),
    offsetRamp(0.01f)
{
    setFrequency(frequency);
    setDepth(initDepthMs);
    setOffset(initOffsetMs);
    
    // reset states
    phaseState[0] = 0.f;
    phaseState[1] = static_cast<float>(M_PI / 2.0); // offset for stereo LFO
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
    phaseState[0] = 0.f;
    phaseState[1] = static_cast<float>(M_PI / 2.0); // offset for stereo LFO
}

float* LFO::process()
{
    switch (type)
    {
    case Sin:
        osc[0] = 0.5f + 0.5f * std::sin(phaseState[0]);
        osc[1] = 0.5f + 0.5f * std::sin(phaseState[1]);
        break;

    case Tri:
        osc[0] = std::fabs(phaseState[0] - static_cast<float>(M_PI)) / static_cast<float>(M_PI);
        osc[1] = std::fabs(phaseState[1] - static_cast<float>(M_PI)) / static_cast<float>(M_PI);
        break;

    default: break;
    }

    phaseState[0] = std::fmod(phaseState[0] + phaseInc, static_cast<float>(2 * M_PI));
    phaseState[1] = std::fmod(phaseState[1] + phaseInc, static_cast<float>(2 * M_PI));

    // Apply mod depth and offset ramps
    depthRamp.applyGain(osc, 2u);
    offsetRamp.applySum(osc, 2u);

    return osc;
}

void LFO::setType(LFOType newType)
{
    type = newType;

    // reset states
    phaseState[0] = 0.f;
    phaseState[1] = 0.f;
}

void LFO::setFrequency(float newFreqHz)
{
    frequency = std::fmax(newFreqHz, 0.f);
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * frequency;
}

void LFO::setDepth(float newDepthMs)
{
    depthMs = std::fmax( newDepthMs, 0.f);
    depthRamp.setTarget(depthMs * static_cast<float>(0.001 * sampleRate), true);
}

void LFO::setOffset(float newOffsetMs)
{
    offsetMs = std::fmax(newOffsetMs, 0.f);
    offsetRamp.setTarget(newOffsetMs * static_cast<float>(0.001 * sampleRate), true);
}

}
