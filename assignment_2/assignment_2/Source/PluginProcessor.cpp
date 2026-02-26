/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Oscillator.h"

//==============================================================================
Assignment_2AudioProcessor::Assignment_2AudioProcessor()
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
}

Assignment_2AudioProcessor::~Assignment_2AudioProcessor()
{
}

//==============================================================================
// These two fucntions are the only two that we need

void Assignment_2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::ignoreUnused(sampleRate, samplesPerBlock);

    //Set start to 0 and end to 3 minutes (180 seconds)
    float startTime{ 0.0 };
    float endTime{ 180.0 };

    // Create the musical oscillators

    oscillatorGroups.clear();

    // Create the first group -----------------------------

    std::vector<Oscillator> group1;

    Oscillator osc1(sampleRate, 0.0, 20.0, 100, "saw", 1);
    osc1.setEnvelope(sampleRate, 5, 0.1, 0.9, 5);
    group1.push_back(osc1);


    Oscillator osc2(sampleRate, 4.0, 20.0, 102.0, "square", 1);
    osc2.setEnvelope(sampleRate, 1, 0.1, 0.8, 5);
    group1.push_back(osc2);


    Oscillator osc3(sampleRate, 2.0, 20.0, 99, "triangle", 1);
    osc3.setEnvelope(sampleRate, 3, 0.4, 0.5, 3);
    group1.push_back(osc3);


    Oscillator osc4(sampleRate, 4.0, 22.0, 99, "pink", 0.4);
    osc4.setEnvelope(sampleRate, 3, 0.4, 0.6, 3);
    group1.push_back(osc4);

    oscillatorGroups.push_back(group1);

    // Create the second group -----------------------------

    std::vector<Oscillator> group2;

    Oscillator osc5(sampleRate, 0.0, 20.0, 600, "saw", 1);
    osc5.setEnvelope(sampleRate, 5, 0.1, 0.9, 5);
    group2.push_back(osc5);


    Oscillator osc6(sampleRate, 4.0, 20.0, 601, "square", 1);
    osc6.setEnvelope(sampleRate, 1, 0.1, 0.8, 5);
    group2.push_back(osc6);


    Oscillator osc7(sampleRate, 2.0, 20.0, 777, "triangle", 1);
    osc7.setEnvelope(sampleRate, 3, 0.4, 0.5, 3);
    group2.push_back(osc7);


    Oscillator osc8(sampleRate, 4.0, 22.0, 800, "pink", 0.4);
    osc8.setEnvelope(sampleRate, 3, 0.4, 0.6, 3);
    group2.push_back(osc8);

    oscillatorGroups.push_back(group2);

    // Create modulation oscillators

    Oscillator modOsc1(sampleRate, startTime, endTime, 6 , "sin", 1);
    modOsc1.setEnvelope(sampleRate, 0.001, 0.001, 0.999, 0.001);
    modulatorList.push_back(modOsc1);

    Oscillator modOsc2(sampleRate, startTime, endTime, 4, "sin", 1);
    modOsc2.setEnvelope(sampleRate, 0.001, 0.001, 0.999, 0.001);
    modulatorList.push_back(modOsc2);


    // REVERB --------------------------------------------
    
    // Set sample rate
    reverb.setSampleRate(sampleRate);

    // Initial settings
    reverbParameters.roomSize = 0.5f;   // 0.0 to 1.0
    reverbParameters.damping = 0.5f;    // 0.0 to 1.0
    reverbParameters.wetLevel = 0.33f;  // Volume of the reverb
    reverbParameters.dryLevel = 0.4f;   // Volume of the original sound
    reverbParameters.width = 1.0f;      // Stereo width

    reverb.setParameters(reverbParameters);


}

void Assignment_2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //// In case we have more outputs than inputs, this code clears any output
    //// channels that didn't contain input data, (because these aren't
    //// guaranteed to be empty - they may contain garbage).
    //// This is here to avoid people getting screaming feedback
    //// when they first compile a plugin, but obviously you don't need to keep
    //// this code if your algorithm always overwrites all the output channels.
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //    buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...

    // -------------------------------------------//
    // START TINKERING FROM HERE
    // -------------------------------------------//

    int numSamples{ buffer.getNumSamples() };
    auto* leftChannel{ buffer.getWritePointer(0) };
    auto* rightChannel{ buffer.getWritePointer(1) };

    

    for (int i = 0; i < numSamples; i++) {

        
        // Process the oscillator banks
        float mix1{ processOscList( oscillatorGroups[0] )};
        float mix2{ processOscList( oscillatorGroups[1] )};

        // Add one to the number of samples processed
        totalSamplesProcessed++;


        // FILTERS --------------------------------------------

        // Set the modulation of the filters if necessary
        float mod1 { setModBoundary(modulatorList[0].make() , 500.0 , 50.0) };
        float mod2 { setModBoundary(modulatorList[1].make() , 100.0 , 40.0) };


        // Create our filter coefficients
        auto c1 = juce::IIRCoefficients::makeLowPass(getSampleRate(), mod1 , 1.1);
        auto c2 = juce::IIRCoefficients::makeLowPass(getSampleRate(), mod2 , 1.5);
        auto c3 = juce::IIRCoefficients::makeLowPass(getSampleRate(), 300 , 0.707); // No modulation example 

        // The filter contruction needs to stay outside of the loop
        filter1.setCoefficients(c1);
        filter2.setCoefficients(c2);

        float f1 = filter1.processSingleSampleRaw( mix2 );
        float f2 = filter2.processSingleSampleRaw( mix1);

        // Soft clipping
        float drive = 2.0f; // increase for more distortion
        float softClipped1 = std::tanh(drive * f1);
        float softClipped2 = std::tanh(drive * f2);

        float masterRight{ softClipped1 };
        float masterLeft{ softClipped2 };


        leftChannel[i] = masterLeft;
        rightChannel[i] = masterRight;

    }


    // Apply the reverb to the samples

    reverb.processStereo(leftChannel, rightChannel, numSamples);


}


//==============================================================================

const juce::String Assignment_2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Assignment_2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Assignment_2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Assignment_2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Assignment_2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Assignment_2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Assignment_2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Assignment_2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Assignment_2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Assignment_2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void Assignment_2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Assignment_2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
bool Assignment_2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Assignment_2AudioProcessor::createEditor()
{
    return new Assignment_2AudioProcessorEditor (*this);
}

//==============================================================================
void Assignment_2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Assignment_2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Assignment_2AudioProcessor();
}
