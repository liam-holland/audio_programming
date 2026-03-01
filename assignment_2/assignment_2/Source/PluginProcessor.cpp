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
    group4.setDurationAll(140.0f);
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

    // =================== GROUP 6: Long evolving pads / slow movement ===================
    OscillatorGroup group6;
    group6.addOscillator(Oscillator(sampleRate, 15.0f, 0.0f, 180.0f, "saw", 0.3f));
    group6.addOscillator(Oscillator(sampleRate, 15.5f, 0.0f, 182.0f, "triangle", 0.3f));
    group6.setStartAll(15.0f);
    group6.setDurationAll(160.0f);
    group6.setEnvelopeAll(sampleRate, 8, 0.2, 0.95, 8);
    oscillatorGroups.push_back(group6);

    // =================== GROUP 7: Flutter ===================
    OscillatorGroup group7;
    group7.addOscillator(Oscillator(sampleRate, 15.0f, 0.0f, 340.0f, "sin", 0.3f));
    group7.setStartAll(15.0f);
    group7.setDurationAll(160.0f);
    group7.setEnvelopeAll(sampleRate, 8, 0.2, 0.95, 8);
    oscillatorGroups.push_back(group7);


    // Create modulation oscillators

    modulatorList.addOscillator(Oscillator( sampleRate, startTime, endTime, 0.1f, "sin", 1) );
    modulatorList.addOscillator(Oscillator( sampleRate, startTime, endTime, 1.0f, "triangle", 1) );
    modulatorList.addOscillator(Oscillator( sampleRate, startTime, endTime, 10.0f, "sin", 1) );

    modulatorList.setStartAll(startTime);
    modulatorList.setDurationAll(endTime);
    modulatorList.setEnvelopeAll(sampleRate, 0.001, 0.001, 0.999, 0.001);



    // FILTER --------------------------------------------

    juce::IIRFilter filter1;
    float filter1start = 5.0;
    float filter2duration = 20;
    filterList.push_back(filter1);
    filterList[0].reset();


    juce::IIRFilter filter2;
    filterList.push_back(filter2);


    // REVERB --------------------------------------------

    // Set initial parameters

    // Reverb 1
    juce::Reverb reverb1{};
    float reverb1Start = 10.0;
    float reverb1Duration = 20.0;
    float reverb1Wet = 0.4;

    reverb1.setSampleRate(sampleRate);
    juce::Reverb::Parameters reverbParam1{};
    reverbParam1.roomSize =  0.5f ;
    reverbParam1.damping = 0.5f ;
    reverbParam1.wetLevel =  reverb1Wet ;
    reverbParam1.dryLevel = (1.0 - reverb1Wet) ;
    reverbParam1.width = 0.5f ;

    reverb1.setParameters(reverbParam1);

    juce::Reverb reverb2{};

    reverb2.setSampleRate(sampleRate);
    juce::Reverb::Parameters reverbParam2{};
    reverbParam2.roomSize = 0.2f;
    reverbParam2.damping = 0.2f;
    reverbParam2.wetLevel = 0.2f;
    reverbParam2.dryLevel = 0.2f;
    reverbParam2.width = 0.1f;

    reverb2.setParameters(reverbParam2);


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

        // Create a sample vector and add all of the values to it
        std::vector<float> sampleVector;

        sampleVector.push_back(softLimit(oscillatorGroups[0].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[1].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[2].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[3].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[4].processOscList()));
        sampleVector.push_back(softLimit(oscillatorGroups[5].processOscList()));

        float mod3{ setModBoundary(modulatorList[2].make() , 300.0f , 1000.0f) };
        oscillatorGroups[6][0].setFrequency(mod3);
        sampleVector.push_back(softLimit(oscillatorGroups[6].processOscList()));

        // Make sure we have samples to work with
        float initial_mix = 0.0f;

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


        // Add one to the number of samples processed
        totalSamplesProcessed++;

        //// If this was inside the for loop, I was getting strange distortion

        //// Set the modulation of the filters if desired
        float mod1{ setModBoundary( modulatorList[0].make() , 50.0f , 1000.0f)};
        //float mod2{ setModBoundary(modulatorList[1].make() , 100.0 , 40.0) };
        //
        //

        ////// Create our filter coefficients
        auto c1 = juce::IIRCoefficients::makeLowPass(getSampleRate(), mod1, 2.5);
        //auto c1 = juce::IIRCoefficients::makeLowPass(getSampleRate(), 400, 0.707);
        //auto c2 = juce::IIRCoefficients::makeLowPass(getSampleRate(), mod2, 0.707);
        ////auto c3 = juce::IIRCoefficients::makeLowPass(getSampleRate(), 300 , 0.707); // No modulation example 

        ////// The filter contruction needs to stay outside of the loop
        filterList[0].setCoefficients(c1);
        //filterList[1].setCoefficients(c2);

        //filter1.setCoefficients(c1);

        float f1 =  filterList[0].processSingleSampleRaw(initial_mix);
        //float f1 = filter1.processSingleSampleRaw(mix1);
        //filterList[1].processSamples(  rightChannel, numSamples);

        //// FINAL MIXING ------------------------------------------

        float masterRight{ (f1) / 2.0f };
        float masterLeft{ (f1) / 2.0f };

        float modPan{ setModBoundary( modulatorList[1].make() , 0.1 , 0.9) };

        float panLeft = modPan;
        float panRight = (1.0f - modPan);

        float finalGain = 2;

        leftChannel[i] = panLeft * masterLeft * finalGain;
        rightChannel[i] = panRight * masterRight * finalGain;


    }
    
    ////// REVERB -----------------------------------

    std::vector<float> dryLeft(numSamples);
    std::vector<float> dryRight(numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        dryLeft[i] = leftChannel[i];
        dryRight[i] = rightChannel[i];
    }


    ////// Apply the reverb to the samples

    reverb1.processStereo(leftChannel, rightChannel, numSamples);
    reverb2.processStereo(leftChannel, rightChannel, numSamples);


    // Apply wet and dry modulation
    for (int i = 0; i < numSamples; ++i)
    {
        float wetMod = setModBoundary(modulatorList[1].make(), 0.0f, 1.0f); // 0=dry, 1=fully wet

        leftChannel[i] = dryLeft[i] * (1.0f - wetMod) + leftChannel[i] * wetMod;
        rightChannel[i] = dryRight[i] * (1.0f - wetMod) + rightChannel[i] * wetMod;
    }

    //// FINAL SAFETY
    // We do this LAST to ensure that even if the reverb or filter 
    // pushed the signal high, it is caught before leaving the plugin.
    for (int i = 0; i < numSamples; i++)
    {
        leftChannel[i] = softLimit(leftChannel[i]);
        rightChannel[i] = softLimit(rightChannel[i]);
    }

    //// --- TEMPORARY STEREO TEST ---
    //// Mute the right channel entirely
    //for (int i = 0; i < numSamples; i++) {
    //    rightChannel[i] = 0.0f;
    //}




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
