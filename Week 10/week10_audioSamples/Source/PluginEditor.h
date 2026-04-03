/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FileLoader.h"

//==============================================================================
/**
*/
class Week10_audioSamplesAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Week10_audioSamplesAudioProcessorEditor (Week10_audioSamplesAudioProcessor&);
    ~Week10_audioSamplesAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Week10_audioSamplesAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Week10_audioSamplesAudioProcessorEditor)
};
