#pragma once

#include <vector>
#include <cmath>

namespace DSP
{

class GranularPitchShifter
{
public:
    // Constructor: specify grain size (ms) and number of channels
    GranularPitchShifter(float blockSizeMs, unsigned int numChannels);

    // Prepare internal buffers with sample rate (must be called before processing)
    void prepare(double sampleRate);

    // Process audio buffers: input -> output (per-channel, interleaved)
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Set pitch ratio: 1.0 = no shift, >1.0 = pitch up, <1.0 = pitch down
    void setPitchRatio(float ratio);

    // Clear delay buffers and internal state
    void clear();

private:
    std::vector<std::vector<float>> delayBuffer; // Per-channel circular delay buffer
    std::vector<float> window;                  // Hann window

    unsigned int writeIndex { 0 };              // Write head index
    unsigned int bufferSize { 0 };              // Total delay buffer size (in samples)
    unsigned int blockSizeSamples { 0 };        // Grain size (in samples)
    float blockSizeMs { 0.0f };                 // Grain size (in milliseconds)
    unsigned int fracBlockSizeSamples { 0 };    // For overlapping grains
    float pitchRatio { 1.0f };                  // Playback rate of grains
    double sampleRate { 48000.0 };              // Sample rate
    unsigned int numChannels { 0 };             // Number of audio channels


    const float halfPi = M_PI / 2.0f;

    // Linear interpolation read from fractional delay position
    float readInterpolated(const std::vector<float>& buffer, float position) const;
};

} // namespace DSP
