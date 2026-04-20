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

    void loadAudioFile(const juce::File& file);

    //void ball1Click(bool _ballState);
    //void ball2Click(bool _ballState);
    //void ball3Click(bool _ballState);

    int ballMenuSelection();

    // My APVTS for all the parameters that the user can choose
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

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters()
    {
        return {
                 std::make_unique <juce::AudioParameterFloat>(juce::ParameterID("reverb_wet",1),"Reverb Wet/Dry", 0.0f, 0.9f, 0.5f)
                ,std::make_unique <juce::AudioParameterInt>(juce::ParameterID("splash",1),"Number of Splash Paricles", 0, 8, 3)
                ,std::make_unique <juce::AudioParameterFloat>(juce::ParameterID("grain_size_perc",1),"Grain Size %", 0.001f, 0.90f, 0.01f)
                ,std::make_unique <juce::AudioParameterFloat>(juce::ParameterID("grain_deviation_perc",1),"Grain Size Deviation %", 0.00f, 0.50f, 0.1f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gran_mix", 1), "Granulator Mix", 0.0f, 1.0f, 0.5f)
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ball_1_toggle", 1), "Ball 1 State", false )
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ball_2_toggle", 1), "Ball 2 State", false )
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ball_3_toggle", 1), "Ball 3 State", false )
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("backwards_toggle", 1), "Play Backwards", false )
                ,std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("ball_menu", 1), "Ball Menu", juce::StringArray{"1","2","3"} ,0)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ball_mass", 1), "Mass", 0.05f, 1.0f, 0.50f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ball_X_loss", 1), "Ball X Loss", 0.0f, 0.75f, 0.05f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ball_Y_loss", 1), "Ball Y Loss", 0.0f, 0.75f, 0.05f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("x_acceleration", 1), "X - Acceleration", -10.0f, 10.0f, 0.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("y_acceleration", 1), "Y - Acceleration", -10.0f, 10.0f, 0.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("x_initial_velocity", 1), "X - Initial Velocity", -10.0f, 10.0f, 1.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("y_initial_velocity", 1), "Y - Initial Velocity", -10.0f, 0.0f, -1.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("x_initial_position", 1), "X - Initial Position", 0.001f, 0.999f, 0.001f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("y_initial_position", 1), "Y - Initial Position", 0.001f, 0.999f, 0.001f)
        };
    }

    void resetParametersToDefault();

    // A list of IDs you want to group together
    const std::vector<juce::String> ballParamIDs{ "ball_mass", "ball_X_loss", "ball_Y_loss" };

    std::atomic<float>* reverbSlider;
    std::atomic<float>* splashNumber;
    std::atomic<float>* grainBaseDeviation;
    std::atomic<float>* grainBaseLengthPerc;
    std::atomic<float>* granulatorMix;

    std::atomic<float>* ballMass;
    std::atomic<float>* ballXStartVelocity;
    std::atomic<float>* ballYStartVelocity;
    std::atomic<float>* ballXStartPosition;
    std::atomic<float>* ballYStartPosition;
    std::atomic<float>* ballXLoss;
    std::atomic<float>* ballYLoss;
    std::atomic<float>* ballXAcceleration;
    std::atomic<float>* ballYAcceleration;

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

