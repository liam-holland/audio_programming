/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
//==============================================================================


//=============== INITLISATION OF OSCILLATORS, FILTERS AND REVERB ============//

void Assignment_2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::ignoreUnused(sampleRate, samplesPerBlock);

    //Set start to 0 and end to 3 minutes (180 seconds)
    float startTime{ 0.0 };
    float endTime{ 180.0 };


    //================= START HERE ============================================

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
    group1.setStartAll(0.0f);
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

    modulatorList.addOscillator(Oscillator(sampleRate, startTime, endTime, 1.5 / endTime, "sin", 1)); // start on high frequencies, end on low
    modulatorList[0].setPhaseShift(0.25f);
    modulatorList.addOscillator(Oscillator(sampleRate, startTime, endTime, 1.75 / endTime, "triangle", 1)); // add reverb slowly
    modulatorList.addOscillator(Oscillator(sampleRate, startTime, endTime, 0.01f, "sin", 1)); // slowly pan from left to right and back
    modulatorList.addOscillator(Oscillator(sampleRate, startTime, endTime, 20.0f, "sin", 1)); // flutter 20 times a second (FM)
    modulatorList.addOscillator(Oscillator(sampleRate, startTime, endTime, 0.75 / endTime, "sin", 1)); // move the notch frequency

    modulatorList.setStartAll(startTime);
    modulatorList.setDurationAll(endTime);
    modulatorList.setEnvelopeAll(sampleRate, 0.001, 0.001, 0.999, 0.001);


    // FILTER --------------------------------------------

    filterList.push_back(juce::IIRFilter());
    filterList.push_back(juce::IIRFilter());

    filterList[0].reset();
    filterList[1].reset();


    // REVERB --------------------------------------------

    // Set initial parameters

    // Reverb 1
    reverb1.setSampleRate(sampleRate);
    juce::Reverb::Parameters reverbParam1{};
    reverbParam1.roomSize =  0.5f ;
    reverbParam1.damping = 0.5f ;
    reverbParam1.wetLevel =  1.0f ;
    reverbParam1.dryLevel = 0.0f ;
    reverbParam1.width = 0.5f ;

    reverb1.setParameters(reverbParam1);

    // Reverb 2

    reverb2.setSampleRate(sampleRate);
    juce::Reverb::Parameters reverbParam2{};
    reverbParam2.roomSize = 0.3f;
    reverbParam2.damping = 0.2f;
    reverbParam2.wetLevel = 1.0f;
    reverbParam2.dryLevel = 0.0f;
    reverbParam2.width = 1.0f;

    reverb1.setParameters(reverbParam2);

}

//=============== CREATION OF SAMPLE VALUES, FILTERING AND EFFECTS ============//

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

        // =========== PROCESS MUSICAL OSCILLATORS ===========//

        // Create a sample vector and add all of the values to it
        std::vector<float> sampleVector;

        sampleVector.push_back(softLimit(oscillatorGroups[0].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[1].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[2].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[3].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[4].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[5].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[7].processOscList()));

        // Use frequency modulation one of the oscillatiors for a flutter effect
        float flutterMod{ juce::jmap( modulatorList[3].make() ,-1.0f, 1.0f , 1400.0f , 1480.0f) };
        oscillatorGroups[6][0].setFrequency(flutterMod);
        sampleVector.push_back(softLimit(oscillatorGroups[6].processOscList()));


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

        // =========== PROCESS THE MODULATORS ===========//

        // High pas filter
        // I want the filter to focus on the extremes (high and low frequencies)
        
        float rawHighPass = modulatorList[0].make();

        // Simple Sigmoid Shaping, learnt about this is AML
        // This formula "pushes" values toward the extremes (-1 and 1)
        float intensity = 4.0f; // Higher = more time at the ends, less in the middle
        float sigmoid = std::tanh(intensity * rawHighPass) / std::tanh(intensity);

        // Can use jmap to set the boundaries of the modulator
        float highPassMod = juce::jmap(sigmoid, -1.0f, 1.0f, 1.0f, 6000.0f);

        // Notch filter
        float peakMod = juce::jmap(modulatorList[4].make(), -1.0f, 1.0f, 1000.0f, 6000.0f);

        // Panning modulator
        float panMod{ juce::jmap(modulatorList[2].make() , -1.0f , 1.0f , 0.1f , 0.9f) };
        
        // =========== SET FILTERS AND PROCESS THE SAMPLES ===========//


        // Create our filter coefficients
        auto c1 = juce::IIRCoefficients::makeHighPass(getSampleRate(), highPassMod, 1.0);
        auto c2 = juce::IIRCoefficients::makePeakFilter(getSampleRate(), peakMod, 0.4, 1.2);

        // Set coefficients
        filterList[0].setCoefficients(c1);
        filterList[1].setCoefficients(c2);

        // Create a vector to store the filter outputs
        std::vector<float> filterVector;

        // Process samples with filters
        filterVector.push_back(  filterList[0].processSingleSampleRaw( initial_mix ) );
        filterVector.push_back(  filterList[1].processSingleSampleRaw( initial_mix ) );

        // =========== FINAL MIXING AND PANNING BEFORE REVERB ===========//

        // Make sure we have samples to work with
        float filter_mix = 0.0f;

        // Mix the oscillator output
        if (!filterVector.empty())
        {
            // Sum all the elements in the vector
            float sum = 0.0f;
            for (int i = 0; i < filterVector.size(); ++i)
            {
                sum += filterVector[i];
            }

            // Divide the sum by the number of elements to get the mean
            filter_mix = 0.8f * (sum / static_cast<float>(filterVector.size()));
        }
        else
        {
            // If the vector is empty, the mean is zero
            filter_mix = 0.0f;
        }

        // Pass and final gain adjustment
        float masterRight{ filter_mix };
        float masterLeft{ filter_mix };

        float panLeft = panMod;
        float panRight = (1.0f - panMod);

        float finalGain = 2.0f;

        leftChannel[i] = panLeft * masterLeft * finalGain;
        rightChannel[i] = panRight * masterRight * finalGain;

    }
    
    // =========== SETUP THE REVERB ===========//

    std::vector<float> dryLeft(numSamples);
    std::vector<float> dryRight(numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        dryLeft[i] = leftChannel[i];
        dryRight[i] = rightChannel[i];
    }


    // Apply the reverb to the samples

    reverb1.processStereo(leftChannel, rightChannel, numSamples);
    reverb2.processStereo(leftChannel, rightChannel, numSamples);

    // Apply wet and dry modulation
    for (int i = 0; i < numSamples; ++i)
    {
        float wetMod = juce::jmap(modulatorList[1].make(),-1.0f, 1.0f, 0.0f, 1.0f); // 0 = dry, 1=fully wet

        leftChannel[i] = dryLeft[i] * (1.0f - wetMod) + leftChannel[i] * wetMod;
        rightChannel[i] = dryRight[i] * (1.0f - wetMod) + rightChannel[i] * wetMod;
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
