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

    // This helper manages audio formats (WAV, AIFF, etc.)
    juce::AudioFormatManager formatManager;

    void readFromReaderToAudioBuffer(juce::AudioFormatReader* reader, juce::AudioBuffer<float>& sampleBuffer)
    {
        // resize the referenced buffer
        sampleBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);

        // read data into the buffer
        reader->read(&sampleBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

    }

    // This is the auto-generated UI
    std::unique_ptr<juce::GenericAudioProcessorEditor> genericEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Assignment_3AudioProcessorEditor)

};
