/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <fstream>
#include <random>

#include "Ball.h"
#include "FileLoader.h"



//==============================================================================
/**
*/
class Assignment_3AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Assignment_3AudioProcessor();
    ~Assignment_3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Assignment_3AudioProcessor)

    Ball testBall;

    std::ofstream file{ "ball_test.csv" };

    int sample{ 0 };

    FileLoader fileLoader;

    juce::AudioBuffer<float> sampleBuffer{};

    int initialSampleLength{ 0 };

    int grainSize{ 2048 };

    int randomNumber{ 0 };

    // For creating random numbers
    std::mt19937 gen; 

    int bufferStartSample = 0;

    Ball::BallState state{ 1.0f , 0.0f };

    int playForwards{ 1 };

    int startOrEnd = 0;

    // For changing speed

    float speed = 1.0f; // <--- your speed control

};
