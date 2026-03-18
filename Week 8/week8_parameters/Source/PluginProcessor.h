/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Envelope.h"
#include "OscillatorGroup.h"
#include "Oscillator.h"
#include "DelayLine.h"


//==============================================================================
/**
*/
class week8_parameters  : public juce::AudioProcessor
{
public:
    //==============================================================================
    week8_parameters();
    ~week8_parameters() override;

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(week8_parameters)

    //================= ADD GLOBAL VARIABLES HERE ========================//

    DelayLine delayLine;

    // Create a vector if vectors. Should change this into a class 
    std::vector<OscillatorGroup> oscillatorGroups;
    
    // This will house all of the modulators
    OscillatorGroup modulatorList;

    // This will house all of the filters
    std::vector<juce::IIRFilter> filterList;

    // Two reverbs is enough, so dont need a reverb bank
    juce::Reverb reverb1;
    juce::Reverb reverb2;

    int totalSamplesProcessed = 0;

    /**
    Apply a soft limit to an input signal to avoid hard clipping
    */
    float softLimit(float input) {
        return std::tanh(input) * 0.9f;
    };

    float pan{ 0.5f };

    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* yourParameter1;
    std::atomic<float>* yourParameter2;
    std::atomic<float>* yourParameter3;
    std::atomic<float>* yourParameter4;
};