/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Oscillator.h"
#include "Filter.h"

//==============================================================================
/**
*/
class Assignment_2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Assignment_2AudioProcessor();
    ~Assignment_2AudioProcessor() override;

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment_2AudioProcessor)
    
    juce::Random random;

    // Create a vector if vectors. Should change this into a class 
    std::vector<std::vector<Oscillator>> oscillatorGroups;
    
    // This will house all of the modulators
    std::vector<Oscillator> modulatorList;

    int totalSamplesProcessed = 0;

    juce::IIRFilter filter1;
    juce::IIRFilter filter2;


    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParameters;

    float mix{ 0 };

    float setModBoundary(float _modValue, float modFloor, float modBase)
    {
        float modDiff = (modFloor + modBase) / 2.0f;
        float modMiddle = (modFloor - modBase) / 2.0f;

        return (modDiff * _modValue) + modMiddle;
    }

    float processOscList(std::vector<Oscillator>& _oscillatorGroup)
    {
        float mix { 0 };

        for (int o = 0; o < _oscillatorGroup.size(); o++)
        {

            mix += _oscillatorGroup[o].make();

        }

        mix = 0.9f * (mix / (_oscillatorGroup.size()));

        return mix;
    }

    //struct modBoundaryValues
    //{
    //    float modFloor;
    //    float modBase;
    //    float modMiddle;
    //    float modDiff;
    //};
};
