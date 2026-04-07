/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <random>

//==============================================================================
Assignment_3AudioProcessor::Assignment_3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

    std::random_device rd;
    gen = std::mt19937(rd());

    // Load the binary of a file into the sampleBuffer
    fileLoader.loadIntoAudioBuffer( BinaryData::HarpA4_wav, BinaryData::HarpA4_wavSize, sampleBuffer);


}

Assignment_3AudioProcessor::~Assignment_3AudioProcessor()
{
}

//==============================================================================
// PREPARE TO PLAY AND PROCESS BLOCK//

void Assignment_3AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    testBall.prepare(sampleRate);

}

void Assignment_3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    // Clears any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Create a stereo output
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // If the buffer is empty, don't process!
    if (sampleBuffer.getNumSamples() == 0)
        return;

    // Number of samples in buffer
    int numSamples{ buffer.getNumSamples() };    

    initialSampleLength = sampleBuffer.getNumSamples();
    int maxGrainIndex = (initialSampleLength / numSamples) - 1;
    if (maxGrainIndex < 0) maxGrainIndex = 0;

    // Randomise the current grain start
    //std::uniform_int_distribution<> dist(0, maxGrainIndex);
    //int currentGrainStart = dist(gen) * numSamples;

    int currentGrainStart = (maxGrainIndex * numSamples);

    // Find the minimum amount of input  and output channels
    int channelsToCopy = std::min(totalNumOutputChannels, sampleBuffer.getNumChannels());

    //Channel number
    int channelNumber{ 0 };

    if (channelsToCopy == 2) //Check if input is stereo or mono
    {
        channelNumber = 1;
    }
    else
    {
        channelNumber = 0; //If the output channel, is greater than the input, make it mono
    }

    // Values that are going to be smoothed to avoid clicks
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValueLeft;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValueRight;

    // Number of samples to get to target value
    int N = 8;

    smoothedValueLeft.reset(N);
    smoothedValueRight.reset(N);

    // Use the veolcity direction to play forwards or backwards
    if (state.velocity > 0)
    {
        playForwards = 1;
        startOrEnd = 0;
    }
    else
    {
        playForwards = -1;
        startOrEnd = buffer.getNumSamples();
    }

    // The starting position of the sample buffer
    bufferStartSample = floor(currentGrainStart * (1 - state.position));

    // Values that we are going to slowly move towards
    int readPosN = bufferStartSample + startOrEnd + (N * playForwards);
    float valueLeftN = sampleBuffer.getSample(0, readPosN);
    float valueRightN = sampleBuffer.getSample(channelNumber, readPosN);
    smoothedValueLeft.setTargetValue(valueLeftN);
    smoothedValueRight.setTargetValue(valueRightN);

    for (int i = 0; i < numSamples; i++)
    {

        // Move the ball
        state = testBall.processMovement();

        float readPositionFloat = initialSampleLength * (1-state.position);
        int readPosition = floor(initialSampleLength * (1-state.position));

        // Update the reader using fractional interpolation
        //float readPosFloat = bufferStartSample + startOrEnd + (i * playForwards * speed);
        //int readPos = bufferStartSample + startOrEnd + (i * playForwards * speed);

        int index0 = (int)readPositionFloat;
        int index1 = index0 + 1;
        float frac = readPositionFloat - index0;

        float sample0L = sampleBuffer.getSample(0, index0);
        float sample1L = sampleBuffer.getSample(0, index1);

        float sample0R = sampleBuffer.getSample(channelNumber, index0);
        float sample1R = sampleBuffer.getSample(channelNumber, index1);

        float valueLeftI = sample0L + frac * (sample1L - sample0L);
        float valueRightI = sample0R + frac * (sample1R - sample0R);

        leftChannel[i] = valueLeftI;
        rightChannel[i] = valueRightI;


        //if ( i < N && (readPosition < initialSampleLength))
        //{
        //    leftChannel[i] = smoothedValueLeft.getNextValue();
        //    rightChannel[i] = smoothedValueRight.getNextValue();
        //}
        //else if ( i >=N && (readPosition < initialSampleLength))
        //{
        //    leftChannel[i] = valueLeftI;
        //    rightChannel[i] = valueRightI;
        //}
        //else
        //{
        //    leftChannel[i] = 0.0f;
        //    rightChannel[i] = 0.0f;
        //}


    }


}



//==============================================================================
const juce::String Assignment_3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Assignment_3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Assignment_3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Assignment_3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Assignment_3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Assignment_3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Assignment_3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Assignment_3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Assignment_3AudioProcessor::getProgramName (int index)
{
    return {};
}

void Assignment_3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void Assignment_3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Assignment_3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
bool Assignment_3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Assignment_3AudioProcessor::createEditor()
{
    return new Assignment_3AudioProcessorEditor (*this);
}

//==============================================================================
void Assignment_3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Assignment_3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Assignment_3AudioProcessor();
}
