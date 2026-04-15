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
                       ),
#endif
    parameters(*this, nullptr,"ParamTreeIdentifier",
        {
            std::make_unique <juce::AudioParameterFloat> (juce::ParameterID("param_1",1),"one", 0.0f, 0.9f, 0.5f)
        })
{

    //std::random_device rd;
    //gen = std::mt19937(rd());

    // Load the binary of a file into the sampleBuffer
    fileLoader.loadIntoAudioBuffer( BinaryData::Cath_short_clip_wav, BinaryData::Cath_short_clip_wavSize, sampleBuffer);

    yourParameter1 = parameters.getRawParameterValue("param_1");

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
    //testBall.prepare(sampleRate);
    //testBall1.prepare(sampleRate);
    //testBall2.prepare(sampleRate);
    //testBall3.prepare(sampleRate);

    // Ball 0
    //testBall.setAcceleration( 0.001f , -20.0f);
    //testBall.setLoss( 0.01f ,0.01f );
    //testBall.setStartPosition( 0.01f , 1.0f);
    //testBall.setStartVelocity( 1.0f , -20.0f);
    //testBall.solveMaxVelocity();

    // Ball 1
    //testBall1.setAcceleration(0.0f, -1.0f);
    //testBall1.setLoss(0.01f, 0.01f);
    //testBall1.setStartPosition(0.02f, 0.99f);
    //testBall1.setStartVelocity(1.0f, -6.0f);
    //testBall1.solveMaxVelocity();

    // Ball 1
    //testBall2.setAcceleration(0.0f, -10.0f);
    //testBall2.setLoss(0.01f, 0.03f);
    //testBall2.setStartPosition(0.04f, 0.99f);
    //testBall2.setStartVelocity(0.75f, -10.0f);
    //testBall2.solveMaxVelocity();


    //Max number of balls
    int numberofBalls{ 32 };

    balls.resize(32);

    for (auto& b : balls)
    {
        b.prepare(sampleRate);
        b.setAcceleration(0.001f, -0.50f);
        b.setLoss(0.10f, 0.10f);
        b.setStartPosition(0.01f, 1.0f);
        b.setStartVelocity(1.0f, -1.0f);
        b.solveMaxVelocity();
    }

    balls[1].prepare(sampleRate);
    balls[1].setAcceleration(0.1f, -5.50f);
    balls[1].setLoss(0.10f, 0.10f);
    balls[1].setStartPosition(0.01f, 1.0f);
    balls[1].setStartVelocity(1.0f, -5.0f);
    balls[1].solveMaxVelocity();

    //Max grains per ball

    //Currently this is working as maxGrains overall
    int grainsPerBall{ 512 };

    //Grains initialisation
    grains.resize(grainsPerBall);
    //grains1.resize(grainsPerBall);
    //grains2.resize(grainsPerBall);

    for ( auto & g: grains)
    {
        g.setSampleRate(sampleRate);
        g.setMinandMax(0.1f, 0.8f, 0.5f);
    }

    //for (auto& g : grains1)
    //{
    //    g.setSampleRate(sampleRate);
    //    g.setMinandMax(0.3f, 3.0f, 1.0f);
    //}

    //for (auto& g : grains2)
    //{
    //    g.setSampleRate(sampleRate);
    //    g.setMinandMax(1.4f, 6.0f, 3.0f);
    //}

    
    reverbParams.roomSize = 0.8f;
    reverbParams.width = 1.0f;

    reverb.setParameters(reverbParams);



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
 
        //To test, make ball 1 and 2 exit
        balls[0].create();
        balls[1].create();

        float outLeft = 0.0f;
        float outRight = 0.0f;


        // I want to create splash grains!. Little grains that come off the main one, when a 'base' ball hits

        for (auto& b : balls)
        {
            if (b.getExists())
            {
                Ball::BallState state = b.processMovement();
                stateDraw = state;

                assignGrain(state, grains, sampleBufferSamples);
                StereoSample out = mixGrains(grains, sampleBuffer, sampleBufferSamples);
                
                outLeft += out.left;
                outLeft += out.right;
            }
        }

        leftChannel[i] += 0.6f * outLeft;
        rightChannel[i] += 0.6f * outRight;
     
        //if ( testBall.getEndBall() == true)
        //{
        //    testBall.~Ball();
        //}


        // Process the movement of the balls
        //state  = testBall.processMovement();
        //state1 = testBall1.processMovement();
        //state2 = testBall2.processMovement();

        // Assign a voice to the grains vector if triggered
        //assignGrain(state, grains, sampleBufferSamples);
        //assignGrain(state1, grains1, sampleBufferSamples);
        //assignGrain(state2, grains2, sampleBufferSamples);

        // Mix the grains
        //StereoSample out = mixGrains(grains, sampleBuffer, sampleBufferSamples);
        //StereoSample out1 = mixGrains(grains1, sampleBuffer, sampleBufferSamples);
        //StereoSample out2 = mixGrains(grains2, sampleBuffer, sampleBufferSamples);

        // Mix the output of the grains
        //leftChannel[i] += 0.6f * (out.left + out1.left + out2.left);
        //rightChannel[i] += 0.6f*( out.right + out1.right + out2.right);

        //leftChannel[i] += 0.6f * (out.left);
        //rightChannel[i] += 0.6f * (out.right);

    }

    float wet = *yourParameter1;

    reverbParams.wetLevel = wet;
    reverbParams.dryLevel = 1.0f - wet;
    reverbParams.width = 1.0f;

    reverb.setParameters(reverbParams);
    
    reverb.processStereo(leftChannel, rightChannel, numSamples);
    

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
    return new Assignment_3AudioProcessorEditor(*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Assignment_3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = parameters.copyState();
    std::unique_ptr < juce::XmlElement > xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Assignment_3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.


    std::unique_ptr < juce::XmlElement > xmlState(getXmlFromBinary(data,
        sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Assignment_3AudioProcessor();
}
