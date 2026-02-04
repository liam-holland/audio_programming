#pragma once

#include "PluginProcessor.h"

//==============================================================================
class MyProjectAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MyProjectAudioProcessorEditor (MyProjectAudioProcessor&);
    ~MyProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyProjectAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyProjectAudioProcessorEditor)
};
