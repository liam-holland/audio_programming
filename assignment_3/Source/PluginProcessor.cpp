/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <random>
#include "BallSample.h"

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

    //std::random_device rd;
    //gen = std::mt19937(rd());

    // Load the binary of a file into the sampleBuffer
    fileLoader.loadIntoAudioBuffer( BinaryData::Cath_short_clip_wav, BinaryData::Cath_short_clip_wavSize, sampleBuffer);

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

    // Initiliase the balls with the sample rate
    testBall.prepare(sampleRate);
    testBall1.prepare(sampleRate);
    testBall2.prepare(sampleRate);
    testBall3.prepare(sampleRate);


    // Ball 0
    testBall.setAcceleration( 0.001f , -5.0f);
    testBall.setLoss( 0.01f ,0.01f );
    testBall.setStartPosition( 0.01f , 1.0f);
    testBall.setStartVelocity( 1.0f , -4.0f);
    testBall.solveMaxVelocity();

    // Ball 1
    testBall1.setAcceleration(0.0f, -1.0f);
    testBall1.setLoss(0.01f, 0.01f);
    testBall1.setStartPosition(0.02f, 0.99f);
    testBall1.setStartVelocity(1.0f, -6.0f);
    testBall1.solveMaxVelocity();

    // Ball 1
    testBall2.setAcceleration(0.0f, -10.0f);
    testBall2.setLoss(0.01f, 0.03f);
    testBall2.setStartPosition(0.04f, 0.99f);
    testBall2.setStartVelocity(0.75f, -10.0f);
    testBall2.solveMaxVelocity();

    //Grains per ball
    int grainsPerBall{ 16 };

    //Grains initialisation
    grains.resize(grainsPerBall);
    grains1.resize(grainsPerBall);
    grains2.resize(grainsPerBall);

    for ( auto & g: grains)
    {
        g.setSampleRate(sampleRate);
        g.setMinandMax(0.1f, 1.0f, 0.3f);
    }

    for (auto& g : grains1)
    {
        g.setSampleRate(sampleRate);
        g.setMinandMax(0.3f, 3.0f, 1.0f);
    }

    for (auto& g : grains2)
    {
        g.setSampleRate(sampleRate);
        g.setMinandMax(1.4f, 6.0f, 3.0f);
    }

    reverbParams.roomSize = 0.8f;
    reverbParams.wetLevel = 0.3f;
    reverbParams.dryLevel = 0.7f;
    reverbParams.width = 1.0f;

    reverb.setParameters(reverbParams);


    ////Grain 0
    //grain.setSampleRate(sampleRate);
    //grain.setMinandMax(0.02f, 0.3f);

    ////Grain 1
    //grain1.setSampleRate(sampleRate);
    //grain1.setMinandMax(0.1f, 1.0f);


}

void Assignment_3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clears any output channels that didn't contain input data
    for (auto ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    // Create a stereo output
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // If the buffer is empty, don't process!
    if (sampleBuffer.getNumSamples() == 0)
        return;

    // Number of samples in buffer
    int numSamples{ buffer.getNumSamples() };
    int sampleBufferSamples{ sampleBuffer.getNumSamples() };

    for (int i = 0; i < numSamples; i++)
    {

        Ball::BallState state = testBall.processMovement();
        Ball::BallState state1 = testBall1.processMovement();
        Ball::BallState state2 = testBall2.processMovement();

        // Assign a grain
        if (state.triggerY)
        {
            for (auto& g : grains)
            {
                if (!g.getGrainPlayState())
                {
                    g.checkForTrigger(state, sampleBufferSamples);
                    break;
                }
            }
        }

        // Assign a grain
        if (state1.triggerY)
        {
            for (auto& g : grains1)
            {
                if (!g.getGrainPlayState())
                {
                    g.checkForTrigger(state1, sampleBufferSamples);
                    break;
                }
            }
        }


        // Assign a grain
        if (state2.triggerY)
        {
            for (auto& g : grains2)
            {
                if (!g.getGrainPlayState())
                {
                    g.checkForTrigger(state2, sampleBufferSamples);
                    break;
                }
            }
        }

        float mixL = 0.0f;
        float mixR = 0.0f;

        //Mix all of the grains
        for (auto& g : grains)
        {
            auto out = g.grainOutput(sampleBuffer, sampleBufferSamples);
            mixL += out.left;
            mixR += out.right;
        }

        float mixL1 = 0.0f;
        float mixR1 = 0.0f;

        //Mix all of the grains
        for (auto& g : grains1)
        {
            auto out = g.grainOutput(sampleBuffer, sampleBufferSamples);
            mixL1 += out.left;
            mixR1 += out.right;
        }

        float mixL2 = 0.0f;
        float mixR2 = 0.0f;

        //Mix all of the grains
        for (auto& g : grains2)
        {
            auto out = g.grainOutput(sampleBuffer, sampleBufferSamples);
            mixL2 += out.left;
            mixR2 += out.right;
        }      


        leftChannel[i] += 0.6f*( mixL + mixL1 + mixL2);
        rightChannel[i] += 0.6f*( mixR + mixR1 + mixL2);


    }


    reverb.processStereo(leftChannel, rightChannel, numSamples);
    

        //grain.checkForTrigger( state, sampleBufferSamples );
        //Grain::grainStruct grainOut = grain.grainOutput( sampleBuffer, sampleBufferSamples);

        //leftChannel[i] += grainOut.left;
        //rightChannel[i] += grainOut.right;

        //grain1.checkForTrigger(state1, sampleBufferSamples);
        //Grain::grainStruct grainOut1 = grain1.grainOutput( sampleBuffer, sampleBufferSamples);

        //leftChannel[i] += grainOut1.left;
        //rightChannel[i] += grainOut1.right;

}


    //// Values that are going to be smoothed to avoid clicks
    //juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValueLeft;
    //juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValueRight;

    //// Number of samples to get to target value
    //int N = 8;

    //smoothedValueLeft.reset(N);
    //smoothedValueRight.reset(N);

    //smoothedValueLeft.setTargetValue(valueLeftN);
    //smoothedValueRight.setTargetValue(valueRightN);


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
