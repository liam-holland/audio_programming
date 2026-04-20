/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FileLoader.h"

//==============================================================================
Assignment_3AudioProcessorEditor::Assignment_3AudioProcessorEditor (Assignment_3AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
   
    // Create the generic UI
    genericEditor = std::make_unique<juce::GenericAudioProcessorEditor>(p);
    addAndMakeVisible( *genericEditor );

    // Register the basic formats
    formatManager.registerBasicFormats();

    // Set up the button
    loadButton.setButtonText("Load Audio File...");
    addAndMakeVisible(loadButton);
    // Define what happens when the button is clicked
    loadButton.onClick = [this] { loadButtonClicked(); };

    // Create attachments to the balls
    ball1Attachment = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.parameters, "ball_1_toggle", Ball1);
    ball2Attachment = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.parameters, "ball_2_toggle", Ball2);
    ball3Attachment = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.parameters, "ball_3_toggle", Ball3);
    backwardsAttachment = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.parameters, "backwards_toggle", backwardsButton);

    // Choose ball
    ballMenu.setText("Choose ball", true);
    addAndMakeVisible(ballMenu);
    ballMenuAttachment = std::make_unique< juce::AudioProcessorValueTreeState::ComboBoxAttachment>( audioProcessor.parameters, "ball_menu", ballMenu);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 500);
    startTimerHz(100);

}



void Assignment_3AudioProcessorEditor::loadButtonClicked()
{
    // Create the chooser
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file to play...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.wav;*.mp3;*.aiff" // Filters
    );

    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    // Launch the file chooser
    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            juce::File file = fc.getResult();

            if (file.exists())
            {
                // Give the file to the function in PluginProcessor.cpp
                audioProcessor.loadAudioFile(file);
            }
        }
    );
}


void Assignment_3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Carve out 60 pixels at the top for the button
    loadButton.setBounds(area.removeFromTop(60).reduced(10));

    // The Generic Editor automatically takes the rest of the space
    genericEditor -> setBounds(area);
}

void Assignment_3AudioProcessorEditor::timerCallback()
{
    repaint(); // This tells the OS to call the paint() function
}

Assignment_3AudioProcessorEditor::~Assignment_3AudioProcessorEditor()
{
}

//==============================================================================
void Assignment_3AudioProcessorEditor::paint (juce::Graphics& g)
{

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}
