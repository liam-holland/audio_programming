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
    fileLoader.loadIntoAudioBuffer( BinaryData::drumming_wav, BinaryData::drumming_wavSize, sampleBuffer);

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

    maxGrainSamples = (int)(maxGrainLength * sampleRate);

    // Initiliase the balls with the sample rate
    testBall.prepare(sampleRate);
    testBall1.prepare(sampleRate);
    testBall2.prepare(sampleRate);
    testBall3.prepare(sampleRate);

    // Ball 0: The "Foundation" (Slow, wide ambient sweeps)
    testBall.setAcceleration( 1.0f , -9.0f);
    testBall.setLoss( 0.001f ,0.01f );    // Very low loss so it moves for a long time
    testBall.setStartPosition( 0.0f , 1.0f);
    testBall.setStartVelocity( 1.0f , -1.0f);
    testBall.solveMaxVelocity();

    //// Ball 1: The "Heartbeat" (Steady rhythmic pulse)
    //testBall1.setGravity(-0.4f);
    //testBall1.setLoss(0.01f);
    //testBall1.setStartPosition(0.5f);
    //testBall1.setStartVelocity(1.0f);

    //// Ball 2: The "Glitch" (Fast, chaotic "chirps" and flutters)
    //testBall2.setGravity(-1.6f);
    //testBall2.setLoss(0.05f);    // Higher loss so it settles into a rhythm faster
    //testBall2.setStartPosition(0.2f);
    //testBall2.setStartVelocity(2.0f);

    //// Ball 3: The "Accent" (Short-lived, high-energy bounces)
    //testBall3.setGravity(-0.1f);
    //testBall3.setLoss(0.1f);     // High loss causes it to die out musically
    //testBall3.setStartPosition(0.8f);
    //testBall3.setStartVelocity(0.0f);

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

    initialSampleLength = sampleBuffer.getNumSamples();

    for (int i = 0; i < numSamples; i++)
    {

        //Need to add edge detection to stop the buzzing

        // Move the ball
        state = testBall.processMovement();
        bool triggerPressed = state.triggerY;

        // See if the trigger has been pressed
        if (state.triggerY && !isGrainPlaying)
        {
            isGrainPlaying = true;
            grainReadPos = 0;

            // Get the current sample
            float playBackX = (state.xPosition);

            grainStartSample = (int)floor(playBackX * initialSampleLength);;

            float velocityPercent = abs(state.yVelocity) / abs(state.yMaxVelocity);
            grainLength = juce::jlimit(512, maxGrainSamples, (int)(velocityPercent * maxGrainSamples)); //Choose the length of the grain
            grainEndSample = juce::jlimit(grainStartSample, initialSampleLength, grainStartSample + grainLength); //The last sample of the grain


        }



        // Fill sample if the state is "ON"
        if (isGrainPlaying)
        {
            float phase = (float)grainReadPos / (float)grainLength;
            phase = juce::jlimit(0.0f, 1.0f, phase);
            float envValue = 0.5f * (1.0f - cos(2.0f * juce::MathConstants<float>::pi * phase));

            int currentGrainIDX = grainStartSample + grainReadPos;

            if (currentGrainIDX < initialSampleLength && currentGrainIDX < grainEndSample)
            {

                float sampleL = sampleBuffer.getSample(0, currentGrainIDX);
                float sampleR = sampleBuffer.getSample(sampleBuffer.getNumChannels() > 1 ? 1 : 0, currentGrainIDX);

                leftChannel[i] += sampleL * envValue;
                rightChannel[i] += sampleR * envValue;

                grainReadPos++;
            }

            if ( grainReadPos >= grainLength )
            {
                isGrainPlaying = false;
            }


        }



    //// Values that are going to be smoothed to avoid clicks
    //juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValueLeft;
    //juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedValueRight;

    //// Number of samples to get to target value
    //int N = 8;

    //smoothedValueLeft.reset(N);
    //smoothedValueRight.reset(N);

    //// Use the veolcity direction to play forwards or backwards
    //if (state.velocity > 0)
    //{
    //    playForwards = 1;
    //    startOrEnd = 0;
    //}
    //else
    //{
    //    playForwards = -1;
    //    startOrEnd = buffer.getNumSamples();
    //}

    //// The starting position of the sample buffer
    //bufferStartSample = floor(currentGrainStart * (1 - state.position));

    // Values that we are going to slowly move towards
    //int readPosN = bufferStartSample + startOrEnd + (N * playForwards);
    //float valueLeftN = sampleBuffer.getSample(0, readPosN);
    //float valueRightN = sampleBuffer.getSample(channelNumber, readPosN);
    //smoothedValueLeft.setTargetValue(valueLeftN);
    //smoothedValueRight.setTargetValue(valueRightN);

    

        //else //If the state is off, the everything is zero
        //{
        //    leftChannel[i] += 0;
        //    rightChannel[i] += 0;
        //}


        //if (state.triggerPlayingY && state.On)
        //{
        //    state.On = false;
        //    state.Off = true;
        //    state.triggerPlayingX = false;
        //}


        //state1 = testBall1.processMovement();
        //state2 = testBall2.processMovement();
        //state3 = testBall3.processMovement();

        
        //float playBack1 = (1.0f - state1.position);
        //float playBack2 = (1.0f - state2.position);
        //float playBack3 = (1.0f - state3.position);

        //Sample that the xdirection is on
       




        // I want velocity of the grain to be related to the amplitude of the output


        //lr_Output ballOut = processSamples(sampleBuffer, initialSampleLength, playBack, channelNumber, state);
        //lr_Output ballOut1 = processSamples(sampleBuffer, initialSampleLength, playBack1, channelNumber, state1);
        //lr_Output ballOut2 = processSamples(sampleBuffer, initialSampleLength, playBack2, channelNumber, state2);
        //lr_Output ballOut3 = processSamples(sampleBuffer, initialSampleLength, playBack3, channelNumber, state3);


        //// Update the reader using fractional interpolation
        ////float readPosFloat = bufferStartSample + startOrEnd + (i * playForwards * speed);
        ////int readPos = bufferStartSample + startOrEnd + (i * playForwards * speed);
      

        //// Ball 0: Far Left, Ball 1: Mid Left, Ball 2: Mid Right, Ball 3: Far Right
        //float mix = 30.0f;
        //leftChannel[i] = (ballOut.L_out * 1.0f + ballOut1.L_out * 0.7f + ballOut2.L_out * 0.3f + ballOut3.L_out * 0.1f) * mix;
        //rightChannel[i] = (ballOut.R_out * 0.1f + ballOut1.R_out * 0.3f + ballOut2.R_out * 0.7f + ballOut3.R_out * 1.0f) * mix;


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
