/*
  ==============================================================================

    Grain.h
    Created: 10 Apr 2026 1:07:35pm
    Author:  Liam Holland

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Ball.h"



class Grain
{
public:

    Grain() = default;
    ~Grain() = default;

    struct grainStruct
    {
        float left;
        float right;
    };

    void setBallType(std::string _ballType)
    {
        if (_ballType =="base")
        {
            baseBall = true;
        }
        else
        {
            baseBall = false;
        }
    }

    bool getDelBall()
    {
        return endBall;
    }


    void setSampleRate(int _sampleRate)
    {
        sampleRate = _sampleRate;

        setMinandMax(minGrainLength, maxGrainLength, baseGrainLength);
    }


    void setMinandMax(float _minGrainLength, float _maxGrainLength, float _baseGrainLength)
    {
        minGrainLength = _minGrainLength;
        maxGrainLength = _maxGrainLength;
        baseGrainLength = _baseGrainLength;

        maxGrainSamples = (int)(maxGrainLength * sampleRate);
        minGrainSamples = (int)(minGrainLength * sampleRate);
        baseGrainSampleLength = (int)(baseGrainLength * sampleRate);
    }

    bool getGrainPlayState()
    {
        return isPlaying;
    }

    void checkForTrigger( Ball::BallState state, int _sampleBufferSamples)
    {

        if (state.xVelocity > 0)
        {
            forwards = true;
            forwardsMultiplier = 1;
        }
        else
        {
            forwards = false;
            forwardsMultiplier = -1;
        }

        // See if the trigger has been pressed

        //if (state.triggerY && !isPlaying)
        if ( state.triggerY )
        {
            isPlaying = true;
            grainReadPos = 0;

            float velocityPercent = (state.yMaxVelocity > 0.0f)
                ? std::abs(state.yVelocity) / std::abs(state.yMaxVelocity)
                : 0.0f;

            grainSampleLength = juce::jlimit(minGrainSamples, maxGrainSamples, baseGrainSampleLength); //Choose the length of the grain
            grainSampleLength *= juce::jmap(velocityPercent, 0.75f, 1.25f); // Link the grainLength and velocity, but don't make them tied
                       
            // Add some jitter as to where the grains play
            // The spread of the jitter is dependennt on the speed the ball is travelling the xDirection
            float spread = std::abs(state.xVelocity) * 0.2f;
            float jitter = (juce::Random::getSystemRandom().nextFloat() - 0.5f) * spread;
            float playBackX = state.xPosition + jitter;
            playBackX = juce::jlimit(0.0f, 1.0f, playBackX);

            grainStartSample = (int)floor(playBackX * _sampleBufferSamples);

            if (!forwards)
            {
                grainStartSample = juce::jlimit(grainSampleLength, _sampleBufferSamples - 1, grainStartSample);
            }
            

        }

    }


    grainStruct grainOutput(const juce::AudioBuffer<float>& _sampleBuffer , int _sampleBufferSamples )
    {
        // Fill sample if the state is "ON"
        if (isPlaying)
        {
            float phase = (float)grainReadPos / (float)grainSampleLength;
            phase = juce::jlimit(0.0f, 1.0f, phase);
            float envValue = 0.5f * (1.0f - cos(2.0f * juce::MathConstants<float>::pi * phase));

            int currentGrainIDX = grainStartSample + grainReadPos * forwardsMultiplier;

            if (currentGrainIDX >= 0 && currentGrainIDX < _sampleBufferSamples)
            {

                float sampleL = _sampleBuffer.getSample(0, currentGrainIDX) * envValue;
                float sampleR = _sampleBuffer.getSample( _sampleBuffer.getNumChannels() > 1 ? 1 : 0, currentGrainIDX) * envValue;

                grainReadPos++;

                return { sampleL, sampleR };
            }

            grainReadPos++;

            if (grainReadPos >= grainSampleLength)
            {
                isPlaying = false;

                if (!baseBall)
                {
                    endBall = true;
                }
              
            }
                        
        }

        return { 0.0f, 0.0f };
    }


private:

    int sampleRate{ 48000 };


    bool isPlaying{ false };


    float maxGrainLength{ 0.4f };
    float minGrainLength{ 0.01f };
    float baseGrainLength{ 0.3f };

    int maxGrainSamples{ 4 };
    int minGrainSamples{ 2 };

    int grainReadPos{ 0 };
    int grainStartSample{ 0 };

    int baseGrainSampleLength{ 0 };
    int grainSampleLength{ 0 };

    float phase{ 0 };

    bool forwards{ true };
    float forwardsMultiplier{ 1 };

    bool baseBall{ true };
    bool endBall{ true };

};

