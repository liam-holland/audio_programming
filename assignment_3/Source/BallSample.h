/*
  ==============================================================================

    BallSample.h
    Created: 8 Apr 2026 2:44:43pm
    Author:  Liam Holland

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct lr_Output
{
    float L_out;
    float R_out;
};

float InterpolateHermite4pt3oX(float x0, float x1, float x2, float x3, float t)
{
    float c0 = x1;
    float c1 = 0.5F * (x2 - x0);
    float c2 = x0 - (2.5F * x1) + (2 * x2) - (.5F * x3);
    float c3 = (.5F * (x3 - x0)) + (1.5F * (x1 - x2));
    return (((((c3 * t) + c2) * t) + c1) * t) + c0;
}

lr_Output processSamples( juce::AudioBuffer<float>& _sampleBuffer, int _initalSampleLength , float _playBack, int _channelNumber, Ball::BallState _state  )
{

    float readPositionFloat = _initalSampleLength * _playBack;
    int readPosition = floor(readPositionFloat);

    int i1 = juce::jlimit(0, _initalSampleLength - 1, (int)floor(readPositionFloat));
    int i0 = juce::jlimit(0, _initalSampleLength - 1, i1 - 1); // Previous
    int i2 = juce::jlimit(0, _initalSampleLength - 1, i1 + 1);  // Next
    int i3 = juce::jlimit(0, _initalSampleLength - 1, i1 + 2);  // Next-Next

    float frac = readPositionFloat - i1;

    float y0_L = _sampleBuffer.getSample(0, i0);
    float y1_L = _sampleBuffer.getSample(0, i1);
    float y2_L = _sampleBuffer.getSample(0, i2);
    float y3_L = _sampleBuffer.getSample(0, i3);

    float y0_R = _sampleBuffer.getSample(_channelNumber, i0);
    float y1_R = _sampleBuffer.getSample(_channelNumber, i1);
    float y2_R = _sampleBuffer.getSample(_channelNumber, i2);
    float y3_R = _sampleBuffer.getSample(_channelNumber, i3);

    float valueLeftI = InterpolateHermite4pt3oX(y0_L, y1_L, y2_L, y3_L, frac);
    float valueRightI = InterpolateHermite4pt3oX(y0_R, y1_R, y2_R, y3_R, frac);

    if (std::abs(_state.yVelocity) < 0.01f && _state.yPosition <= 0.0f) {
        valueLeftI = 0.0f;
        valueRightI = 0.0f;
    }

    // Create a simple volume curve based on position
    // The ball is loudest at the floor (position 0.0)
    float amplitude = 1.0f - _state.yPosition;
    // Squaring it makes the "decay" sound more natural/exponential
    amplitude *= amplitude;

    return { valueLeftI * amplitude, valueRightI * amplitude };


}
