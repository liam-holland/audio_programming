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
    parameters(*this, nullptr, "ParamTreeIdentifier", createParameters())
{

    // Load the binary of a file into the sampleBuffer
    fileLoader.loadIntoAudioBuffer( BinaryData::Cath_short_clip_wav, BinaryData::Cath_short_clip_wavSize, sampleBuffer);
    
    reverbSlider = parameters.getRawParameterValue( "reverb_wet" );
    splashNumber = parameters.getRawParameterValue( "splash" );
    grainBaseLengthPerc = parameters.getRawParameterValue( "grain_size_perc" );
    grainBaseDeviation = parameters.getRawParameterValue( "grain_deviation_perc" );
    granulatorMix = parameters.getRawParameterValue("gran_mix");

    ballXAcceleration = parameters.getRawParameterValue("x_acceleration");
    ballYAcceleration = parameters.getRawParameterValue("y_acceleration");
    ballMass = parameters.getRawParameterValue("ball_mass");
    ballXLoss = parameters.getRawParameterValue("ball_X_loss");
    ballYLoss = parameters.getRawParameterValue("ball_Y_loss");
    ballXStartVelocity = parameters.getRawParameterValue("x_initial_velocity");
    ballYStartVelocity = parameters.getRawParameterValue("y_initial_velocity");
    ballXStartPosition = parameters.getRawParameterValue("x_initial_position");
    ballYStartPosition = parameters.getRawParameterValue("y_initial_position");

    resetParametersToDefault();

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

    //Max number of balls
    int numberofBalls{ 32 };
    balls.resize(32);

    // Prepare all the balls, but don't create any
    for (auto& b : balls)
    {
        b.prepare(sampleRate);
    }

    //Currently this is working as maxGrains overall
    int maxGrains{ 512 };
    grains.resize(maxGrains);

    for ( auto & g: grains)
    {
        g.setSampleRate(sampleRate);
        g.setMinandMax(0.1f, 4.0f, 1.5f);
    }

    // Reverb parameters
    reverbParams.roomSize = 0.8f;
    reverbParams.width = 1.0f;
    reverb.setParameters(reverbParams);

}

// Create a function that will load the audio files when called
void Assignment_3AudioProcessor::loadAudioFile(const juce::File& file)
{
    // .getFullPathName() converts the File object to the String the loader expects
    fileLoader.loadIntoAudioBuffer( file.getFullPathName(), sampleBuffer);
}

int Assignment_3AudioProcessor::ballMenuSelection()
{
    int selectedBallIndex = juce::roundToInt(parameters.getRawParameterValue("ball_menu")->load());
    return selectedBallIndex;
}

void Assignment_3AudioProcessor::resetParametersToDefault()
{
    // get the parameters directly from the processor
    auto& params = getParameters();

    for (auto* p : params)
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(p))
        {
            rangedParam->setValueNotifyingHost(rangedParam->getDefaultValue());
        }
    }
}

void Assignment_3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    bool isBall1Active = *parameters.getRawParameterValue("ball_1_toggle") > 0.5f;
    bool isBall2Active = *parameters.getRawParameterValue("ball_2_toggle") > 0.5f;
    bool isBall3Active = *parameters.getRawParameterValue("ball_3_toggle") > 0.5f;
    bool backwards = *parameters.getRawParameterValue("backwards_toggle") > 0.5f;

    std::vector<bool> ballActive{ {isBall1Active,isBall2Active, isBall3Active} };

    for (int i = 0; i < 3; i++)
    {
        int ballSelected = ballMenuSelection();

        // Create and delete balls
        if (ballActive[i] && !balls[i].getExists() && ballSelected == i)
        {
            balls[i].create();
            balls[i].setBallType("base");
            balls[i].setAcceleration(*ballXAcceleration, *ballYAcceleration);
            balls[i].setLoss(*ballXLoss, *ballYLoss);
            balls[i].setMass(*ballMass);
            balls[i].setStartPosition(*ballXStartPosition, *ballYStartPosition);
            balls[i].setStartVelocity(*ballXStartVelocity,*ballYStartVelocity);
            balls[i].solveMaxVelocity();
        }
        else if (!ballActive[i] && balls[i].getExists())
        {
            balls[i].setExisits(false);
        }

        if (balls[i].getExists())
        {
            
            balls[ballSelected].setAcceleration(*ballXAcceleration, *ballYAcceleration);
            balls[ballSelected].setLoss(*ballXLoss, *ballYLoss);
            balls[ballSelected].setMass(*ballMass);

        }
    }


    // Clears any output channels that didn't contain input data
    for (auto ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    // Create a stereo output
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // If the buffer is empty, don't process!
    if (sampleBuffer.getNumSamples() == 0)
        return;

    // Number of samples in buffer and sample buffer
    int numSamples{ buffer.getNumSamples() };
    int sampleBufferSamples{ sampleBuffer.getNumSamples() };

    // Calculate min, max and base seconds
    float currentFileDuration = (float)sampleBufferSamples / getSampleRate();

    float base = currentFileDuration * *grainBaseLengthPerc;
    float dev = *grainBaseDeviation;

    float min = base * (1.0f - dev);
    float max = base * (1.0f + dev);

    float minClamped = juce::jlimit(0.001f, base, min);
    float maxClamped = juce::jlimit (base, 0.90f * currentFileDuration, max);

    for (auto& g : grains)
    {
        g.setMinandMax(minClamped, maxClamped, base);

        if (backwards)
        {
            g.setAllowBackwards(true);
        }
        else
        {
            g.setAllowBackwards(false);
        }

    }

    // Create wet and dry values for the mix
    float granMix = *granulatorMix;
    float dryGain = 1.0f - granMix;
    float wetGain = granMix;


    for (int i = 0; i < numSamples; i++)
    {

        // Read directly from the original file buffer
        float dryLeft =  sampleBuffer.getSample(0, filePosition);
        float dryRight = sampleBuffer.getNumChannels() > 1 ? sampleBuffer.getSample(1, filePosition) : dryLeft;

        // Loop the player if needed
        filePosition++;
        if (filePosition >= sampleBufferSamples) filePosition = 0;

        float outLeft = 0.0f;
        float outRight = 0.0f;

        // For each ball
        for ( auto& b : balls )
        {
            // Check if the ball exists
            if (b.getExists())
            {
                // If it exists, proccess the movement of that ball
                Ball::BallState state = b.processMovement();

                // If the ball has hit the floor or ceiling
                if (state.triggerY)
                {
                    // If it has, then assign a grain some information
                    assignGrain(state, grains, sampleBufferSamples);

                    // If it is a "baseBall" and the velocity is above 0.05
                    if (state.baseBall && std::abs(state.yVelocity * state.mass) > 0.05f)
                    {

                        int spawnedCount = 0;
                        int maxToSpawn = (int)*splashNumber;

                        // Look through the whole pool of balls for empty slots
                        for (auto& potentialSlot : balls)
                        {
                            // If a ball doesn't exist
                            if (!potentialSlot.getExists())
                            {

                                // Create splash balls, that come off from the main ball
                                potentialSlot.create();
                                potentialSlot.setBallType("splash");
                                potentialSlot.setStartPosition(state.xPosition, state.yPosition);
                                potentialSlot.setMass(0.2f);
                                potentialSlot.setLoss(0.1f, 0.1f);

                                // Vary the velocity of x and y
                                float vXVar = (random.nextFloat() - 0.5f) * 0.5f;
                                float vYVar = (random.nextFloat() - 0.5f) * 0.5f;

                                // Set the velocity of the balls
                                potentialSlot.setStartVelocity( state.xVelocity + vXVar,
                                    - state.yVelocity * (0.7f + vYVar));

                                potentialSlot.setAcceleration(0.001f, -5.0f);

                                spawnedCount++;

                                // stop searching after all the slots have been filled
                                if (spawnedCount >= maxToSpawn)
                                    break;
                            }
                        }
                    }
                }
            }
        }

        StereoSample wetOut = mixGrains(grains, sampleBuffer, sampleBufferSamples);

        // --- 3. MIXING ---
        // Combine dry file and wet grains based on the mix slider
        leftChannel[i] = (dryLeft * dryGain) + (wetOut.left * wetGain);
        rightChannel[i] = (dryRight * dryGain) + (wetOut.right * wetGain);

    }


    // Add reverb
    float wet = *reverbSlider;

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
