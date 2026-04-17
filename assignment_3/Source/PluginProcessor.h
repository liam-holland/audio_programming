/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <fstream>
#include <random>
#include <vector>

#include "Ball.h"
#include "FileLoader.h"
#include "Grain.h"


//==============================================================================
/**
*/
class Assignment_3AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    Assignment_3AudioProcessor();
    ~Assignment_3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    //Get the ball coordiantes

    //// In Processor (Public)
    //float getBallX()
    //{
    //    return stateDraw.xPosition;
    //}

    //float getBallY()
    //{
    //    return stateDraw.yPosition;
    //}

    //bool getBallExists()
    //{
    //    return stateDraw.exists;
    //}

    //float getBallX2()
    //{
    //    return stateDraw2.xPosition;
    //}

    //float getBallY2()
    //{
    //    return stateDraw2.yPosition;
    //}

    void loadAudioFile(const juce::File& file);

    // Sliders
    juce::AudioProcessorValueTreeState parameters;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment_3AudioProcessor)

    //Set up for loading in an audio file
    // Create an audio buffer to fill
    juce::AudioBuffer<float> sampleBuffer{};

    // Initlialise file loader
    FileLoader fileLoader;


    // Set up for creating balls and grains
    std::vector<Ball> balls;
    std::vector<Grain> grains;


    std::atomic<float>* reverbSlider;
    std::atomic<float>* splashNumber;
    std::atomic<float>* grainBaseDeviation;
    std::atomic<float>* grainBaseLengthPerc;
    std::atomic<float>* granulatorMix;

    int filePosition = 0; 


    //Reverb
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    // Assign a grain
    void assignGrain(Ball::BallState _state, std::vector<Grain>& grains, int _sampleBufferSamples)
    {
        if (_state.triggerY)
        {
            for (auto& g : grains)
            {
                if (!g.getGrainPlayState())
                {
                    g.checkForTrigger(_state, _sampleBufferSamples);
                    break;
                }
            }
        }
    }

    //Mix the grains
    struct StereoSample {
        float left;
        float right;
    };

    StereoSample mixGrains(std::vector<Grain>& grains, const juce::AudioBuffer<float>& sampleBuffer, int _sampleBufferSamples)
    {
        float mixL = 0.0f;
        float mixR = 0.0f;

        for (auto& g : grains)
        {
            if (g.getGrainPlayState())
            {
                auto out = g.grainOutput(sampleBuffer, _sampleBufferSamples);
                mixL += out.left;
                mixR += out.right;

            }

        }

        return { mixL, mixR };
    }


    // Random number generator
    juce::Random random;

    // For drawing the balls
    Ball::BallState stateDraw;
    Ball::BallState stateDraw2;



        // Taken from https://stackoverflow.com/questions/1125666/how-do-you-do-bicubic-or-other-non-linear-interpolation-of-re-sampled-audio-da

        /**
        * x0 - first sample before
        * x1 - first sample after
        * x2 - 2ns sample after
        * x3 - 3rd sample after
        */
        //float InterpolateHermite4pt3oX(float x0, float x1, float x2, float x3, float t)
        //{
        //    float c0 = x1; 
        //    float c1 = 0.5F * (x2 - x0);
        //    float c2 = x0 - (2.5F * x1) + (2 * x2) - (.5F * x3);
        //    float c3 = (.5F * (x3 - x0)) + (1.5F * (x1 - x2));
        //    return (((((c3 * t) + c2) * t) + c1) * t) + c0;
        //}


};

