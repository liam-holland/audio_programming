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

/**
Creates a grain object.

The grain is something that will play a part of an audio sample

In this project, the properties of the grain are controlled by the ball and the sample chosen
*/
class Grain
{
public:

    Grain() = default;
    ~Grain() = default;

    /**
    * The grain output, once audio has been processed
    */
    struct grainStruct
    {
        float left;
        float right;
    };

    /**
    * Set the sample rate that the grain is working with. If this is refershed it will also determine the min and max sample lenths
    * 
    * @param _sampleRate Sample rate of the project
    */
    void setSampleRate(int _sampleRate)
    {
        sampleRate = _sampleRate;

        setMinandMax(minGrainLength, maxGrainLength, baseGrainLength);

        panSmoothed.reset(sampleRate, 0.02);
    }


    /**
    Set the min and max grain length of the samples in seconds
    
    @param _minGrainLength Minimum grain length in seconds
    @param _minGrainLength Maximum grain length in seconds
    @param _minGrainLength The "base" / "average" grain length in seconds
    */
    void setMinandMax(float _minGrainLength, float _maxGrainLength, float _baseGrainLength)
    {
        minGrainLength = _minGrainLength;
        maxGrainLength = _maxGrainLength;
        baseGrainLength = _baseGrainLength;

        maxGrainSamples = (int)(maxGrainLength * sampleRate);
        minGrainSamples = (int)(minGrainLength * sampleRate);
        baseGrainSampleLength = (int)(baseGrainLength * sampleRate);
    }

    /**
    Return the playing state of the grain
    
    @return isPlaying The playing state of the grain
    */
    bool getGrainPlayState()
    {
        return isPlaying;
    }

    /**
    Set the pan width of the grains
    */
    void setPanWidth( float _panWidth)
    {
        panWidth = _panWidth;
    }

    /**
    Set the smoothing of the panning
    */
    void setPanSmoothed()
    {
        panSmoothed.setTargetValue(pan * panWidth);
    }

    /**
    Track a ball and check if that ball has hit the floor or ceiling
    
    If the ball has hit a surface, calculate the start of the grain, length of the grain
    and if the grain is going to be played forwards or backwards
    */
    void checkForTrigger( Ball::BallState state, int _sampleBufferSamples)
    {
        // Set the values to be used in other functions
        ballMass = state.mass;
        ballXVelocityMax = state.xMaxVelocity;
        ballYVelocityMax = state.yMaxVelocity;
        ballYVelocity = state.yVelocity;
        pan = juce::jmap(state.xPosition, 0.0f, 1.0f, -1.0f, 1.0f);

        setPanSmoothed();

        // Assess if the sample should be played forwards of backwards
        if (allowBackwards)
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
        }
        else
        {
            forwards = true;
            forwardsMultiplier = 1;
        }

        // See if the trigger has been pressed
        if ( state.triggerY )
        {
            isPlaying = true;
            grainReadPos = 0;

            // Set the velocity % to be based on the x-direction velocity
            // The maximum velocity to allow is 10
            float velocityPercent = std::abs((state.xMaxVelocity > 0.0f))
                ? std::abs(state.xVelocity) / std::abs(state.xMaxVelocity)
                : 0.0f;

            grainSampleLength = juce::jlimit(minGrainSamples, maxGrainSamples, baseGrainSampleLength); //Choose the length of the grain
            grainSampleLength *= juce::jmap(velocityPercent, 0.75f, 1.25f); // Link the grainLength and velocity, but don't make them tied
            grainSampleLength = juce::jlimit(minGrainSamples, maxGrainSamples, grainSampleLength);

            // If it's a splash ball, quarter the playback time
            if (!state.baseBall)
            {
                grainSampleLength = int(grainSampleLength / 4.0f);
            }

            // Limit the plaback to be between 0 and 1
            float playBackX = juce::jlimit(0.0f, 1.0f, state.xPosition);
                      
            grainStartSample = (int)floor(playBackX * _sampleBufferSamples);

            // If the ball is going to go forwards, need to ensure that the index read will not be above the last sample.
            if (forwards)
            {
                grainStartSample = juce::jlimit( 0, _sampleBufferSamples - grainSampleLength - 1, grainStartSample);
            }

            // If the ball is going to go backwards, need to ensure that the index read will not be below zero.
            if (!forwards)
            {
                grainStartSample = juce::jlimit(grainSampleLength, _sampleBufferSamples - 1, grainStartSample);
            }
            

        }

    }



    /**
    Return the output of the grain, sample by sample

    @return Left and right channels
    */
    grainStruct grainOutput(const juce::AudioBuffer<float>& _sampleBuffer , int _sampleBufferSamples )
    {

        // Fill sample if the state is "ON"
        if (isPlaying)
        {
            float phase = (float)grainReadPos / (float)grainSampleLength;
            phase = juce::jlimit(0.0f, 1.0f, phase);
            float envValue = 0.5f * (1.0f - cos(2.0f * juce::MathConstants<float>::pi * phase)); // Hann window for playback without clicks

            float velocityYPecent = std::abs(ballYVelocity) / std::abs(ballYVelocityMax);

            int currentGrainIDX = grainStartSample + grainReadPos * forwardsMultiplier;

            // Get a better panning, using contant power
            // As shown here: https://dsp.stackexchange.com/questions/21691/algorithm-to-pan-audio
            float panS = panSmoothed.getNextValue(); // -1 to +1
            float theta = panS * (juce::MathConstants<float>::pi / 4.0f);

            float leftGain = (std::cos(theta) + std::sin(theta)) * 0.70710678f;
            float rightGain = (std::cos(theta) - std::sin(theta)) * 0.70710678f;

            if (currentGrainIDX >= 0 && currentGrainIDX < _sampleBufferSamples)
            {
       
                // Sum to mono
                float mono = 0.5 * (
                  _sampleBuffer.getSample(0, currentGrainIDX) * envValue * ballMass * velocityYPecent
                +_sampleBuffer.getSample(_sampleBuffer.getNumChannels() > 1 ? 1 : 0, currentGrainIDX) * envValue * ballMass * velocityYPecent
                   );

                // Apply the panning
                float sampleL = leftGain * mono;
                float sampleR = rightGain * mono;

                grainReadPos++;

                return { sampleL, sampleR };
            }
            
            // Always increase the read position
            grainReadPos++;

            if (grainReadPos >= grainSampleLength)
            {
                isPlaying = false;
 
            }
                        
        }

        return { 0.0f, 0.0f };
    }

    /**
    Set the grain to be played backwards
    */
    void setAllowBackwards( bool _setBackwards)
    {  
        allowBackwards = _setBackwards;
    }

private:

    int sampleRate{ 48000 };

    float ballMass{ 1.0f };
    float ballYVelocity{ 1.0f };
    float ballYVelocityMax{ 10.0f };
    float ballXVelocityMax{ 10.0f };

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

    float phase{ 0.0f };

    bool forwards{ true };
    float forwardsMultiplier{ 1 };

    bool allowBackwards{ false };

    float panWidth{ 0.0f };
    float pan{ 0.0f };

    juce::SmoothedValue<float> panSmoothed;

};

