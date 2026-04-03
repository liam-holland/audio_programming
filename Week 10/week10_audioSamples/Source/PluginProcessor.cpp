/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Week10_audioSamplesAudioProcessor::Week10_audioSamplesAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{

    // Get a reader for the binary data
    auto reader = fileLoader.getReaderFor(BinaryData::MarimbaC380_wav, BinaryData::MarimbaC380_wavSize);

    // Create a a synthesiser sound
    juce::BigInteger noteRange;
    noteRange.setRange(0, 127, true);
    synth.addSound(new juce::SamplerSound("drums", *reader, noteRange, 60, 1.0f, 1.0f, 5.0f));

    // Create 8 voices within the synth sounds
    for ( int i = 0; i < voices; i++)
    {
        synth.addVoice(new juce::SamplerVoice());
    }

}

Week10_audioSamplesAudioProcessor::~Week10_audioSamplesAudioProcessor()
{
}

//==============================================================================

void Week10_audioSamplesAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    synth.setCurrentPlaybackSampleRate(sampleRate);
}


void Week10_audioSamplesAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear the samples before use
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

}


const juce::String Week10_audioSamplesAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Week10_audioSamplesAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Week10_audioSamplesAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Week10_audioSamplesAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Week10_audioSamplesAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Week10_audioSamplesAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Week10_audioSamplesAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Week10_audioSamplesAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Week10_audioSamplesAudioProcessor::getProgramName (int index)
{
    return {};
}

void Week10_audioSamplesAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void Week10_audioSamplesAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Week10_audioSamplesAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


//==============================================================================
bool Week10_audioSamplesAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Week10_audioSamplesAudioProcessor::createEditor()
{
    return new Week10_audioSamplesAudioProcessorEditor (*this);
}

//==============================================================================
void Week10_audioSamplesAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Week10_audioSamplesAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Week10_audioSamplesAudioProcessor();
}
