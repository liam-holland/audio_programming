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
   
    // 1. Create the generic UI (this builds all your sliders for you)
    genericEditor = std::make_unique<juce::GenericAudioProcessorEditor>(p);
    addAndMakeVisible(*genericEditor);

    // Register the basic formats
    formatManager.registerBasicFormats();

    // Set up the button
    loadButton.setButtonText("Load Audio File...");
    addAndMakeVisible(loadButton);

    // Define what happens when the button is clicked
    loadButton.onClick = [this] { loadButtonClicked(); };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 500);
    startTimerHz(100);

}

void Assignment_3AudioProcessorEditor::loadButtonClicked()
{
    // Create the chooser
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audi file to play...",
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
    genericEditor->setBounds(area);
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
    //// Fill the background
    //g.fillAll(juce::Colours::black);


    //if (audioProcessor.getBallExists())
    //{
    //    g.setColour(juce::Colours::cyan);
    //}
    //else
    //{
    //    g.setColour(juce::Colours::red);
    //}
    //// Set the ball color
    //g.setColour(juce::Colours::cyan);

    //// Draw Ball 1
    //// Get the position from the processor and multiply by width/height
    //auto x = audioProcessor.getBallX() * getWidth();
    //auto y = (1.0f-audioProcessor.getBallY()) * getHeight();
    //auto x2 = audioProcessor.getBallX2() * getWidth();
    //auto y2 = (1.0f-audioProcessor.getBallY2()) * getHeight();

    //// Draw a circle (x, y, width, height)
    //// Note: To center the ball, subtract half its size from x and y
    //g.fillEllipse(x - 10.0f, y - 10.0f, 20.0f, 20.0f);
    //g.fillEllipse(x2 - 10.0f, y2 - 10.0f, 20.0f, 20.0f);

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

//void Assignment_3AudioProcessorEditor::resized()
//{
//    // This is generally where you'll want to lay out the positions of any
//    // subcomponents in your editor..
//}
