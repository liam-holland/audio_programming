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
    void ballMenuChanged();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Assignment_3AudioProcessor& audioProcessor;

    // Create button and file chooser
    juce::TextButton loadButton;
    std::unique_ptr< juce::FileChooser > fileChooser;

    //Create ball buttons attachment, this will link the buttons to the APVTS
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ball1Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ball2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ball3Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> backwardsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ballMuteAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lockStateAttachment;

    // Create button the lock the state of the balls
    juce::ToggleButton lockButton;

    // Create buttons to add balls
    juce::ToggleButton Ball1;
    juce::ToggleButton Ball2;
    juce::ToggleButton Ball3;

    //Play backwards
    juce::ToggleButton backwardsButton;
    juce::ToggleButton ballMute;

    //Choose which ball
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ballMenuAttachment;
    juce::ComboBox ballMenu;

    // This helper manages audio formats (WAV, AIFF, etc.)
    juce::AudioFormatManager formatManager;

    void readFromReaderToAudioBuffer(juce::AudioFormatReader* reader, juce::AudioBuffer<float>& sampleBuffer)
    {
        // resize the referenced buffer
        sampleBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);

        // read data into the buffer
        reader->read(&sampleBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

    }

    // This is the auto generated UI
    std::unique_ptr<juce::GenericAudioProcessorEditor> genericEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Assignment_3AudioProcessorEditor)

};
