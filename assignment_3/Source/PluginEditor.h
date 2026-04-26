/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//The AudioProcessorEditor plays the same role in an Audio Plug-in that the main content component has in a standalone app.
// See Tutorial: The main component.

//==============================================================================
/**
*/
class Assignment_3AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          public juce::Timer
                
{
public:
    Assignment_3AudioProcessorEditor (Assignment_3AudioProcessor&);
    ~Assignment_3AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void timerCallback() override; 
    void resized() override;

private:


    void loadButtonClicked();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Assignment_3AudioProcessor& audioProcessor;

    // Create button and file chooser
    juce::TextButton loadButton;
    std::unique_ptr< juce::FileChooser > fileChooser;

    // This is the auto generated UI
    std::unique_ptr<juce::GenericAudioProcessorEditor> genericEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Assignment_3AudioProcessorEditor)

};
