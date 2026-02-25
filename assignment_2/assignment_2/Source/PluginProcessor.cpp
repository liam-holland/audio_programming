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

    // Create the oscillators

    Oscillator osc1(1, sampleRate, 0.0, 10.0, 440.0, "saw", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc2(1, sampleRate, 0.0, 60.0, 550.0, "saw", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc3(1, sampleRate, 0.0, 60.0, 660.0, "saw", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc4(1, sampleRate, 14.0, 45.0, 80.0, "pink", 0.2, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc5(1, sampleRate, 12.0, 50.0, 70.0, "white", 0.4, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc6(1, sampleRate, 18.0, 22.0, 100.0, "sin", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc7(1, sampleRate, 6.0, 18.0, 250.0, "triangle", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc8(1, sampleRate, 7.0, 70.0, 200.0, "sin", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc9(1, sampleRate, 9.0, 12.0, 75.0, "square", 1, "multiply", std::vector<int>{1, 2, 3});
    //Oscillator osc10(1, sampleRate, 11.0, 100.0, 98.0, "sin", 1, "multiply", std::vector<int>{1, 2, 3});

    // Store the oscillators
    //oscillatorArray oscillatorList;

    oscillatorList.clear();

    oscillatorList.push_back(osc1);
    //oscillatorList.push_back(osc2);
    //oscillatorList.push_back(osc3);
    //oscillatorList.push_back(osc4);
    //oscillatorList.push_back(osc5);
    //oscillatorList.push_back(osc6);
    //oscillatorList.push_back(osc7);
    //oscillatorList.push_back(osc8);
    //oscillatorList.push_back(osc9);
    //oscillatorList.push_back(osc10);

    // FILTER --------------------------------------------

    // Create our filter coefficients
    auto coeffs = juce::IIRCoefficients::makeLowPass(sampleRate, 300.0, 0.707);

    leftFilter.setCoefficients(coeffs);
    rightFilter.setCoefficients(coeffs);


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

    // ENVELOPES ----------------

  
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

    int numSamples{ buffer.getNumSamples() };
    auto* leftChannel{ buffer.getWritePointer(0) };
    auto* rightChannel{ buffer.getWritePointer(1) };

    Envelope env1(getSampleRate(), 5, 0.05, 0.5, 5);


    for (int i = 0; i < numSamples; i++) {


        //If adding, this needs to be zero
        // If multiplying, needs to be greater than zero
        float sample{ 0 };


        for ( int o = 0; o < oscillatorList.size() ; o++)
        {

            oscillatorList[0].make();

            //if ( !oscillatorList[o].getActive() && ( totalSamplesProcessed >= oscillatorList[o].getStartSample() ) )
            //{
            //    oscillatorList[o].setActive( true ); // turn it on
            //}

            //if ( oscillatorList[o].getActive() && ( totalSamplesProcessed >= oscillatorList[o].getLastSample() ) )
            //{
            //    oscillatorList[o].setActive( false ); // turn it off
            //}

            //if (oscillatorList[o].getActive())
            //{
            //    sample += oscillatorList[o].process();
            //}

        }   

        sample = sample * env1.processEnvelope(totalSamplesProcessed);
        
        // If adding then need to divide  by number of oscillators. Omit this if multiplying
        leftChannel[i] = sample  / (2*oscillatorList.size());
        rightChannel[i] = sample  / (2*oscillatorList.size());

        totalSamplesProcessed++;

    }

    // Apply the filters to the samples

    //leftFilter.processSamples(leftChannel, numSamples);
    //rightFilter.processSamples(rightChannel, numSamples);


    // Apply the reverb to the samples

    //reverb.processStereo(leftChannel, rightChannel, numSamples);


    // 

    //totalSamplesProcessed += numSamples;

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
