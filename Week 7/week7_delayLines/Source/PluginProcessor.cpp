/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
week7_delayLines::week7_delayLines()
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
week7_delayLines::~week7_delayLines()
{
}

//==============================================================================
// These two fucntions are the only two that we need
//==============================================================================




//=============== INITLISATION OF OSCILLATORS, FILTERS AND REVERB ============//

void week7_delayLines::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::ignoreUnused(sampleRate, samplesPerBlock);

    //Set start to 0 and end to 3 minutes (180 seconds)
    float startTime{ 0.0 };
    float endTime{ 180.0 };


    //================= START HERE ============================================

    //=============== INITLISATION OF DELAY LINE ============//

    delayLine.prepare(1.0f, sampleRate);

    // Clear all lists

    oscillatorGroups.clear();
    modulatorList.clear();
    filterList.clear();

    // Create the musical oscillators

    // =================== GROUP 1: Low foundational drones ===================
    OscillatorGroup group1;
    group1.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 60.0f, "saw", 0.4f));
    group1.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 62.0f, "triangle", 0.3f));
    group1.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 55.0f, "sin", 0.5f));
    group1.setStartAll(25.0f);
    group1.setDurationAll(180.0f);
    group1.setEnvelopeAll(sampleRate, 5, 0.3, 0.8, 5);
    oscillatorGroups.push_back(group1);


    // =================== GROUP 2: Mid harmonic layer ===================
    OscillatorGroup group2;
    group2.addOscillator(Oscillator(sampleRate, 10.0f, 0.0f, 200.0f, "sin", 0.4f));
    group2.addOscillator(Oscillator(sampleRate, 12.0f, 0.0f, 205.0f, "saw", 0.3f));
    group2.addOscillator(Oscillator(sampleRate, 11.0f, 0.0f, 198.0f, "triangle", 0.3f));
    group2.setStartAll(10.0f);
    group2.setDurationAll(120.0f);
    group2.setEnvelopeAll(sampleRate, 4, 0.4, 0.9, 4);
    oscillatorGroups.push_back(group2);

    // =================== GROUP 3: High shimmer / air ===================
    OscillatorGroup group3;
    group3.addOscillator(Oscillator(sampleRate, 20.0f, 0.0f, 500.0f, "saw", 0.2f));
    group3.addOscillator(Oscillator(sampleRate, 25.0f, 0.0f, 510.0f, "sin", 0.2f));
    group3.addOscillator(Oscillator(sampleRate, 22.0f, 0.0f, 520.0f, "triangle", 0.15f));
    group3.setStartAll(20.0f);
    group3.setDurationAll(100.0f);
    group3.setEnvelopeAll(sampleRate, 6, 0.3, 0.7, 6);
    oscillatorGroups.push_back(group3);

    // =================== GROUP 4: Detuned layer ===================
    OscillatorGroup group4;
    group4.addOscillator(Oscillator(sampleRate, 5.0f, 0.0f, 123.0f, "sin", 0.25f));
    group4.addOscillator(Oscillator(sampleRate, 5.5f, 0.0f, 124.3f, "sin", 0.25f));
    group4.addOscillator(Oscillator(sampleRate, 6.0f, 0.0f, 122.7f, "saw", 0.2f));
    group4.setStartAll(5.0f);
    group4.setDurationAll(120.0f);
    group4.setEnvelopeAll(sampleRate, 4, 0.4, 0.95, 4);
    oscillatorGroups.push_back(group4);

    // =================== GROUP 5: Noise / texture layer ===================
    OscillatorGroup group5;
    group5.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 0.0f, "pink", 0.001f));
    group5.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 0.0f, "white", 0.002f));
    group5.setStartAll(0.0f);
    group5.setDurationAll(180.0f);
    group5.setEnvelopeAll(sampleRate, 10, 5, 0.3, 3);
    oscillatorGroups.push_back(group5);

    // =================== GROUP 6: Slowly adding oscillators ===================
    OscillatorGroup group6;
    group6.addOscillator(Oscillator(sampleRate, 15.0f, 0.0f, 180.0f, "saw", 0.3f));
    group6.addOscillator(Oscillator(sampleRate, 15.5f, 0.0f, 182.0f, "triangle", 0.3f));
    group6.setStartAll(15.0f);
    group6.setDurationAll(160.0f);
    group6.setEnvelopeAll(sampleRate, 12, 0.2, 0.95, 20);
    oscillatorGroups.push_back(group6);

    // =================== GROUP 7: Flutter ===================
    OscillatorGroup group7;
    group7.addOscillator(Oscillator(sampleRate, 15.0f, 0.0f, 1480.0f, "sin", 0.1f));
    group7.setStartAll(15.0f);
    group7.setDurationAll(120.0f);
    group7.setEnvelopeAll(sampleRate, 8, 0.2, 0.95, 8);
    oscillatorGroups.push_back(group7);

    // =================== GROUP 8:HF ===================
    OscillatorGroup group8;
    group8.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 3000.0f, "sin", 0.4f));
    group8.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 3010.0f, "sin", 0.2f));
    group8.setStartAll(0.0f);
    group8.setDurationAll(97.0f);
    group8.setEnvelopeAll(sampleRate, 5, 5, 0.8, 8);
    oscillatorGroups.push_back(group8);

    // =================== GROUP 9: Low - mid drones for second half ===================
    OscillatorGroup group9;
    group9.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 45.0f, "saw", 0.6f));
    group9.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 52.0f, "triangle", 0.4f));
    group9.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 90.0f, "sin", 0.2f));
    group9.addOscillator(Oscillator(sampleRate, 0.0f, 0.0f, 80.0f, "saw", 0.8f));
    group9.setStartAll(70.0f);
    group9.setDurationAll(180.0f);
    group9.setEnvelopeAll(sampleRate, 5, 0.3, 0.8, 5);
    oscillatorGroups.push_back(group9);


    // Create modulation oscillators

    modulatorList.addOscillator(Oscillator(sampleRate, startTime, endTime, 0.75 / endTime, "sin", 1)); 

    modulatorList.setStartAll(startTime);
    modulatorList.setDurationAll(endTime);
    modulatorList.setEnvelopeAll(sampleRate, 0.001, 0.001, 0.999, 0.001);


}

//=============== CREATION OF SAMPLE VALUES, FILTERING AND EFFECTS ============//

void week7_delayLines::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...

    // -------------------------------------------//
    // START TINKERING FROM HERE
    // -------------------------------------------//

    int numSamples{ buffer.getNumSamples() };
    auto* leftChannel{ buffer.getWritePointer(0) };
    auto* rightChannel{ buffer.getWritePointer(1) };


    for (int i = 0; i < numSamples; i++) {

        // =========== PROCESS MUSICAL OSCILLATORS ===========//

        // Create a sample vector and add all of the values to it
        std::vector<float> sampleVector;

        sampleVector.push_back(softLimit(oscillatorGroups[0].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[1].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[2].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[3].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[4].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[5].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[6].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[7].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[8].processOscList()));

        
        // ==================== PROCESS MODULATORS =====================//
        // Use frequency modulation one of the oscillatiors for a flutter effect
        
        float delayMod{ juce::jmap(modulatorList[0].make(), -1.0f, 1.0f, 0.25f, 0.9f) };
        delayLine.setDelay(delayMod);


        // Add one to the number of samples processed
        totalSamplesProcessed++;

        // =========== MIX MUSICAL OSCILLATORS ===========//

        // set initial mix value
        float initial_mix{ 0.0f };

        // Mix the oscillator output
        if (!sampleVector.empty())
        {
            // Sum all the elements in the vector
            float sum = 0.0f;
            for ( int i = 0; i < sampleVector.size(); ++i)
            {
                sum += sampleVector[i];
            }

            // Divide the sum by the number of elements to get the mean
            initial_mix = 0.9f * (sum / static_cast<float>(sampleVector.size()));
        }
        else
        {
            // If the vector is empty, the mean is zero
            initial_mix = 0.0f;
        }


        // Pass and final gain adjustment
        float masterRight{ initial_mix };
        float masterLeft{ initial_mix };


        // =========== DELAY LINES ===========//
        

        masterRight = masterRight + 0.4 * delayLine.processDelay(masterRight);
        masterLeft = masterLeft + 0.4 * delayLine.processDelay(masterLeft);


        // =========== DELAY LINES ===========//

        float finalGain = 2.0f;

        leftChannel[i] =  masterLeft * finalGain;
        rightChannel[i] =  masterRight * finalGain;

    }
    


    // =========== FINAL SAFETY CLIP ===========//
    // Final check to ensure nothing clips
    for (int i = 0; i < numSamples; i++)
    {
        leftChannel[i] = softLimit(leftChannel[i]);
        rightChannel[i] = softLimit(rightChannel[i]);
    }


}


//==============================================================================

const juce::String week7_delayLines::getName() const
{
    return JucePlugin_Name;
}

bool week7_delayLines::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool week7_delayLines::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool week7_delayLines::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double week7_delayLines::getTailLengthSeconds() const
{
    return 0.0;
}

int week7_delayLines::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int week7_delayLines::getCurrentProgram()
{
    return 0;
}

void week7_delayLines::setCurrentProgram (int index)
{
}

const juce::String week7_delayLines::getProgramName (int index)
{
    return {};
}

void week7_delayLines::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void week7_delayLines::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool week7_delayLines::isBusesLayoutSupported (const BusesLayout& layouts) const
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
bool week7_delayLines::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* week7_delayLines::createEditor()
{
    return new week7_delayLinesEditor (*this);
}

//==============================================================================
void week7_delayLines::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void week7_delayLines::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new week7_delayLines();
}
