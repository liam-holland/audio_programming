/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include <random>

#include "PluginProcessor.h"
#include "PluginEditor.h"
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
    //fileLoader.loadIntoAudioBuffer( BinaryData::Cath_short_clip_wav, BinaryData::Cath_short_clip_wavSize, sampleBuffer);
    
    // Set up pointers to all the parameter values
    reverbSlider = parameters.getRawParameterValue( "reverb_wet" );
    splashNumber = parameters.getRawParameterValue( "splash" );
    grainBaseLengthPerc = parameters.getRawParameterValue( "grain_size_perc" );
    grainBaseDeviation = parameters.getRawParameterValue( "grain_deviation_perc" );
    granulatorMix = parameters.getRawParameterValue("gran_mix");

    centreOfGravityX = parameters.getRawParameterValue("centre_of_gravity_x");
    centreOfGravityY = parameters.getRawParameterValue("centre_of_gravity_y");

    ballXAcceleration = parameters.getRawParameterValue("x_acceleration");
    ballYAcceleration = parameters.getRawParameterValue("y_acceleration");
    ballMass = parameters.getRawParameterValue("ball_mass");
    ballXLoss = parameters.getRawParameterValue("ball_X_loss");
    ballYLoss = parameters.getRawParameterValue("ball_Y_loss");
    ballXStartVelocity = parameters.getRawParameterValue("x_initial_velocity");
    ballYStartVelocity = parameters.getRawParameterValue("y_initial_velocity");
    ballXStartPosition = parameters.getRawParameterValue("x_initial_position");
    ballYStartPosition = parameters.getRawParameterValue("y_initial_position");

    lowestFrequency = parameters.getRawParameterValue("lowest_frequency");
    highestFrequency = parameters.getRawParameterValue("highest_frequency");

    // Listen to the parameters
    parameters.addParameterListener( "ball_menu", this );

    // Initial load when we start the plugin
    lastSelectedBall = 0;
    loadSettingsFromSlot(0);

    // Set all the parameters to default
    resetParametersToDefault();

}

Assignment_3AudioProcessor::~Assignment_3AudioProcessor()
{
}

void Assignment_3AudioProcessor::saveCurrentSettingsToSlot(int ballIndex)
{
    // Check that the ball index is within the 1-3 limit
    if (ballIndex < 0 || ballIndex > 2) return;

    // Create a reference to the saved ball settings
    auto& s = savedBallSettings[ballIndex];

    // Read the current values from the APVTS and update the reference
    s.xInitialVelocity = *parameters.getRawParameterValue("x_initial_velocity");
    s.yInitialVelocity = *parameters.getRawParameterValue("y_initial_velocity");
    s.xInitialPosition = *parameters.getRawParameterValue("x_initial_position");
    s.yInitialPosition = *parameters.getRawParameterValue("y_initial_position");
    s.mass = *parameters.getRawParameterValue("ball_mass");
    s.gravityX = *parameters.getRawParameterValue("centre_of_gravity_x");
    s.gravityYChoice = (int)*parameters.getRawParameterValue("centre_of_gravity_2");
    s.xLoss = *parameters.getRawParameterValue("ball_X_loss");
    s.yLoss = *parameters.getRawParameterValue("ball_Y_loss");
    s.xAcceleration = *parameters.getRawParameterValue("x_acceleration");
    s.yAcceleration = *parameters.getRawParameterValue("y_acceleration");

    // Also save the specific toggle for this ball
    juce::String toggleID = "ball_" + juce::String(ballIndex + 1) + "_toggle";
    s.isActive = *parameters.getRawParameterValue(toggleID) > 0.5f;
}

void Assignment_3AudioProcessor::loadSettingsFromSlot(int ballIndex)
{
    if (ballIndex < 0 || ballIndex > 2) return;


    auto& s = savedBallSettings[ballIndex];
    isUpdatingFromInternalSource = true; // Prevent the "save" logic from triggering

    // Push values back to the parameters (this updates the GUI Sliders)
    parameters.getParameter("x_initial_velocity")->setValueNotifyingHost(parameters.getParameterRange("x_initial_velocity").convertTo0to1(s.xInitialVelocity));
    parameters.getParameter("y_initial_velocity")->setValueNotifyingHost(parameters.getParameterRange("y_initial_velocity").convertTo0to1(s.yInitialVelocity));
    parameters.getParameter("x_initial_position")->setValueNotifyingHost(parameters.getParameterRange("x_initial_position").convertTo0to1(s.xInitialPosition));
    parameters.getParameter("y_initial_position")->setValueNotifyingHost(parameters.getParameterRange("y_initial_position").convertTo0to1(s.yInitialPosition));
    parameters.getParameter("ball_mass")->setValueNotifyingHost(parameters.getParameterRange("ball_mass").convertTo0to1(s.mass));
    parameters.getParameter("centre_of_gravity_x")->setValueNotifyingHost(parameters.getParameterRange("centre_of_gravity_x").convertTo0to1(s.gravityX));
    parameters.getParameter("centre_of_gravity_2")->setValueNotifyingHost(s.gravityYChoice == 0 ? 0.0f : 1.0f);
    parameters.getParameter("ball_X_loss")->setValueNotifyingHost(parameters.getParameterRange("ball_X_loss").convertTo0to1(s.xLoss));
    parameters.getParameter("ball_Y_loss")->setValueNotifyingHost(parameters.getParameterRange("ball_Y_loss").convertTo0to1(s.yLoss));
    parameters.getParameter("x_acceleration")->setValueNotifyingHost(parameters.getParameterRange("x_acceleration").convertTo0to1(s.xAcceleration));
    parameters.getParameter("y_acceleration")->setValueNotifyingHost(parameters.getParameterRange("y_acceleration").convertTo0to1(s.yAcceleration));

    isUpdatingFromInternalSource = false;
}

void Assignment_3AudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "ball_menu")
    {
        int newBallIndex = juce::roundToInt(newValue);

        // Save the current GUI slider positions into the PREVIOUS ball's storage
        saveCurrentSettingsToSlot(lastSelectedBall);

        // Update the index
        lastSelectedBall = newBallIndex;

        // Load the NEW ball's storage into the GUI sliders
        juce::MessageManager::callAsync([this, newBallIndex] {
            loadSettingsFromSlot(newBallIndex);
            });
    }
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

    // Prepare all the balls, but don't set any to exist
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

    muteSmoother.reset(sampleRate, 0.01); // 10ms ramp

    // Set initial state immediately to avoid a fade-in on startup
    float initialMute = (*parameters.getRawParameterValue("mute_balls") > 0.5f) ? 0.0f : 1.0f;
    muteSmoother.setCurrentAndTargetValue(initialMute);

}

// Create a function that will load the audio files when called
void Assignment_3AudioProcessor::loadAudioFile(const juce::File& file)
{
    // Tell the audio thread to stop reading the buffer
    isLoading = true;

    // Give the audio thread a moment to finish its current block
    filePosition = 0;

    // Do the loading
    fileLoader.loadIntoAudioBuffer(file.getFullPathName(), sampleBuffer);
    sampleBufferSamples = sampleBuffer.getNumSamples();

    // Tell the audio thread it's safe to come back
    isLoading = false;

    //juce::Logger::writeToLog("File selected: " + file.getFullPathName());
}

int Assignment_3AudioProcessor::ballMenuSelection()
{
    // Use load for safety
    int selectedBallIndex = juce::roundToInt(parameters.getRawParameterValue("ball_menu")->load());
    return selectedBallIndex;
}

int Assignment_3AudioProcessor::floorOrCeiling()
{
    int selectedFloorOrCeiling = juce::roundToInt(parameters.getRawParameterValue("centre_of_gravity_2")->load());
    return selectedFloorOrCeiling;
}

void Assignment_3AudioProcessor::resetParametersToDefault()
{
    // get the parameters directly from the processor
    auto& params = getParameters();

    for (auto* p : params)
    {
        // Need to use a dynamic cast since all the parameters are different types
        // Check if the parameter has a range
        if ( auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(p) )
        {   
            // Set the default value for that ranged parameter
            rangedParam->setValueNotifyingHost(rangedParam->getDefaultValue());
        }
    }
}

void Assignment_3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // If I am currently loading a file, just clear the output and leave
    if (isLoading.load() || sampleBuffer.getNumSamples() == 0)
    {
        buffer.clear();
        return;
    }

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

    // Number of samples in buffer and sample buffer
    int numSamples{ buffer.getNumSamples() };

    // Get the values from the toggle boxes
    bool isBall1Active = *parameters.getRawParameterValue("ball_1_toggle") > 0.5f;
    bool isBall2Active = *parameters.getRawParameterValue("ball_2_toggle") > 0.5f;
    bool isBall3Active = *parameters.getRawParameterValue("ball_3_toggle") > 0.5f;
    bool backwards = *parameters.getRawParameterValue("backwards_toggle") > 0.5f;
    int muteBalls = (*parameters.getRawParameterValue("mute_balls") > 0.5f) ? 0 : 1;

    // Create a vector for the 3 base balls
    bool ballActive[3] = { isBall1Active, isBall2Active, isBall3Active };

    // Get the current value selected in the ball menu
    int ballSelected = ballMenuSelection();

    // Loop through all the base balls
    for (int i = 0; i < 3; i++)
    {
        // Create and delete 

        // If the ball has been marked as active, but doesn't exist, then we create it using the user defined variables
        if ( ballActive[i] && !balls[i].getExists() && ballSelected == i)
        {
            balls[i].create();
            balls[i].setBallType("base");
            balls[i].setAcceleration(*ballXAcceleration, *ballYAcceleration);
            balls[i].setLoss(*ballXLoss, *ballYLoss);
            balls[i].setMass(*ballMass);
            balls[i].setStartPosition(*ballXStartPosition, *ballYStartPosition);
            balls[i].setStartVelocity(*ballXStartVelocity,*ballYStartVelocity);
        }
        // If the ball has been marked as in active and exisits, then we set the ball to not exists
        else if (!ballActive[i] && balls[i].getExists())
        {
            balls[i].setExisits(false);
        }

        // If the ball exists, then we are able to manipulate the values of the ball at the start of each buffer
        if (balls[i].getExists())
        {
            balls[ballSelected].setAcceleration(*ballXAcceleration, *ballYAcceleration);
            balls[ballSelected].setLoss(*ballXLoss, *ballYLoss);
            balls[ballSelected].setMass(*ballMass);
            balls[ballSelected].setCentreOfGravity( *centreOfGravityX, floorOrCeiling());

        }
    }


    // Calculations for  the min and max size of the grains that I want to allow
    float currentFileDuration = (float)sampleBufferSamples / getSampleRate();

    // Calculate the base length
    float base = currentFileDuration * *grainBaseLengthPerc;

    // force base to be at least slightly larger than the absolute minimum
    base = std::max(0.002f, base);

    // Ensure the upper bound of the file is also larger than base
    float topLimit = std::max(base + 0.01f, 0.90f * currentFileDuration);

    float dev = *grainBaseDeviation;
    float min = base * (1.0f - dev);
    float max = base * (1.0f + dev);

    // Now these are safe because we've guaranteed the order of the limits
    float minClamped = juce::jlimit(0.001f, base, min);
    float maxClamped = juce::jlimit(base, topLimit, max);

    // Set the min and max length of the grains, plus set if they are allowed to be played backwards
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

    float muteTarget = (*parameters.getRawParameterValue("mute_balls") > 0.5f) ? 0.0f : 1.0f;
    muteSmoother.setTargetValue(muteTarget);


    // Link the total kinetic energy of the simulation to the filter
    float totalEnergy = 0.0f;
    int activeCount = 0;

    float lowBound = *lowestFrequency;
    float highBound = *highestFrequency;

    float sensitivityScale = 0.5;

    for (auto& b : balls) {
        if (b.getExists()) {
            auto state = b.getState(); // Assuming you have a getter for velocity
            totalEnergy += 0.5f * b.getMass() * (state.xVelocity * state.xVelocity + state.yVelocity * state.yVelocity);
            activeCount++;
        }
    }

    // Map and update the filter once per block
    if (activeCount > 0) {
        float normEnergy = juce::jlimit(0.0f, 1.0f, totalEnergy * sensitivityScale);
        float freq = juce::jmap(normEnergy, 0.0f, 1.0f, lowBound, highBound);
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), freq));
    }

    for (int i = 0; i < numSamples; i++)
    {

        // Read directly from the original file buffer
        float dryLeft =  sampleBuffer.getSample(0, filePosition);
        float dryRight = sampleBuffer.getNumChannels() > 1 ? sampleBuffer.getSample(1, filePosition) : dryLeft;

        // Loop the player
        filePosition++;
        if (filePosition >= sampleBufferSamples) filePosition = 0;

        // Initialise variables
        float outLeft = 0.0f;
        float outRight = 0.0f;
        float ballY = 0.0f;

        // THIS IS THE MAIN PROCESSING LOOP//
        // For each ball,
        for ( auto& b : balls )
        {
            // Check if the ball exists
            if (b.getExists())
            {
                // If it exists, proccess the movement of that ball
                Ball::BallState state = b.processMovement();

                //ballY = state.yPosition;

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
                                float vXVar = (random.nextFloat() - 0.5f) * state.xAcceleration;
                                float vYVar = (random.nextFloat() - 0.5f) * state.yAcceleration;

                                // Set the velocity of the balls
                                potentialSlot.setStartVelocity( state.xVelocity + vXVar,
                                    - state.yVelocity + vYVar);

                                // Set the acceleration of the spash balls
                                potentialSlot.setAcceleration(state.xAcceleration, state.yAcceleration);

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

        // Get the output after processing the grains
        StereoSample wetOut = mixGrains(grains, sampleBuffer, sampleBufferSamples);

        float filteredLeft = filter.processSingleSampleRaw(wetOut.left * wetGain);
        float filteredRight = filter.processSingleSampleRaw(wetOut.right * wetGain);

        float currentMuteGain = muteSmoother.getNextValue();

        // MIXING 
        // Combine dry file and wet grains based on the mix slider
        leftChannel[i] = (dryLeft * dryGain) + (filteredLeft * currentMuteGain) ;
        rightChannel[i] = (dryRight * dryGain) + (filteredRight * currentMuteGain);

    }


    // Add reverb
    float wet = *reverbSlider;

    reverbParams.wetLevel = wet;
    reverbParams.dryLevel = 1.0f - wet;
    reverbParams.width = 1.0f;

    reverb.setParameters(reverbParams);
    reverb.processStereo(leftChannel, rightChannel, numSamples);
    
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
