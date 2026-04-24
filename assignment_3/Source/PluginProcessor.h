/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <fstream>
#include <random>
#include <vector>

#include "Ball.h"
#include "FileLoader.h"
#include "Grain.h"


//==============================================================================
/**
*/
class Assignment_3AudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    Assignment_3AudioProcessor();
    ~Assignment_3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================

    // Public functions to be initialised

    void loadAudioFile(const juce::File& file);

    int ballMenuSelection();

    int floorOrCeiling();

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Public variables
    // My APVTS for all the parameters that the user can choose
    juce::AudioProcessorValueTreeState parameters;
    int sampleBufferSamples{ 0 };

    //Mix the grains
    struct StereoSample {
        float left;
        float right;
    };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment_3AudioProcessor)

    int lastSelectedBall = 0;

    //Set up for loading in an audio file
    // Create an audio buffer to fill
    juce::AudioBuffer<float> sampleBuffer{};

    // Initlialise file loader
    FileLoader fileLoader;

    std::atomic<bool> isLoading{ false };

    // Set up for creating balls and grains
    std::vector<Ball> balls;
    std::vector<Grain> grains;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters()
    {
        return {
                 std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("ball_menu", 1), "Ball Menu", juce::StringArray{"1","2","3"} ,0)

                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("mute_balls", 1), "Mute Balls", false)
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ball_1_toggle", 1), "Ball 1 State", false)
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ball_2_toggle", 1), "Ball 2 State", false)
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ball_3_toggle", 1), "Ball 3 State", false)

                ,std::make_unique<juce::AudioParameterFloat>("divider1", "=== INITIAL BALL SETUP ===", 0.0f, 100.0f, 0.0f)

                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("x_initial_velocity", 1), "X - Initial Velocity", -10.0f, 10.0f, 1.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("y_initial_velocity", 1), "Y - Initial Velocity", -10.0f, 10.0f, -1.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("x_initial_position", 1), "X - Initial Position", 0.001f, 0.999f, 0.001f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("y_initial_position", 1), "Y - Initial Position", 0.001f, 0.999f, 0.001f)

                ,std::make_unique<juce::AudioParameterFloat>("divider2", "=== BALL PARAMETERS ===", 0.0f, 100.0f, 0.0f)

                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("lock_state", 1), "Lock Current Ball State", false)

                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ball_mass", 1), "Mass", 0.05f, 1.0f, 0.50f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("centre_of_gravity_x", 1), "Gravity X Centre", 0.00f, 1.0f, 0.50f)
                ,std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("centre_of_gravity_y", 1), "Gravity Y Centre 2", juce::StringArray{"Floor","Ceiling"} ,1)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ball_X_loss", 1), "Ball X Loss", 0.0f, 0.75f, 0.05f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ball_Y_loss", 1), "Ball Y Loss", 0.0f, 0.75f, 0.05f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("x_acceleration", 1), "X - Acceleration", 0.0f, 10.0f, 0.00f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("y_acceleration", 1), "Y - Acceleration", 0.0f, 10.0f, 0.00f)

                ,std::make_unique<juce::AudioParameterFloat>("divider3", "=== GRAIN PARAMETERS ===", 0.0f, 100.0f, 0.0f)
                
                ,std::make_unique<juce::AudioParameterBool>(juce::ParameterID("backwards_toggle", 1), "Play Backwards", false )
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lowest_frequency", 1), "Filter Bottom Frequency", 20.0f, 2000.0f, 20.0f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("highest_frequency", 1), "Filter Top Frequency", 20.0f, 2000.0f, 20.0f)
                ,std::make_unique <juce::AudioParameterFloat>(juce::ParameterID("reverb_wet",1),"Reverb Wet/Dry", 0.0f, 0.9f, 0.5f)
                ,std::make_unique <juce::AudioParameterInt>(juce::ParameterID("splash",1),"Number of Splash Paricles", 0, 8, 3)
                ,std::make_unique <juce::AudioParameterFloat>(juce::ParameterID("grain_size_perc",1),"Grain Size %", 0.001f, 0.90f, 0.01f)
                ,std::make_unique <juce::AudioParameterFloat>(juce::ParameterID("grain_deviation_perc",1),"Grain Size Deviation %", 0.00f, 0.50f, 0.1f)
                ,std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gran_mix", 1), "Granulator Mix", 0.0f, 1.0f, 0.5f)
    
        };
    }

    // Struct with all the initial conditions
    struct BallSettings
    {
        // Movement and Physics
        float xInitialVelocity = 1.0f;
        float yInitialVelocity = -1.0f;
        float xInitialPosition = 0.001f;
        float yInitialPosition = 0.001f;
        float mass = 0.5f;
        float xLoss = 0.05f;
        float yLoss = 0.05f;
        float xAcceleration = 0.0f;
        float yAcceleration = 0.0f;

        // Gravity and State
        float gravityX = 0.5f;
        int gravityYChoice = 1; // 0 for Floor, 1 for Ceiling
        bool isActive = false;
    };

    // An array to hold the state for our 3 base balls
    BallSettings savedBallSettings[3];

    // A helper to push the UI values into a storage slot
    void saveCurrentSettingsToSlot(int ballIndex);

    // A helper to pull storage values out to the UI
    void loadSettingsFromSlot(int ballIndex);

    // Flag to prevent a feedback loop
    bool isUpdatingFromInternalSource = false;

    // Default parameters
    void resetParametersToDefault();

    // Lock the current state, by removing any forces
    bool Assignment_3AudioProcessor::lockState();

    bool lockStateBool{ false };

    std::atomic<float>* reverbSlider;
    std::atomic<float>* splashNumber;
    std::atomic<float>* grainBaseDeviation;
    std::atomic<float>* grainBaseLengthPerc;
    std::atomic<float>* granulatorMix;

    std::atomic<float>* centreOfGravityX;
    std::atomic<float>* centreOfGravityY;

    std::atomic<float>* lowestFrequency;
    std::atomic<float>* highestFrequency;

    std::atomic<float>* ballMass;
    std::atomic<float>* ballXStartVelocity;
    std::atomic<float>* ballYStartVelocity;
    std::atomic<float>* ballXStartPosition;
    std::atomic<float>* ballYStartPosition;
    std::atomic<float>* ballXLoss;
    std::atomic<float>* ballYLoss;
    std::atomic<float>* ballXAcceleration;
    std::atomic<float>* ballYAcceleration;

    int filePosition = 0; 

    //Reverb
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    //// Assign a grain
    void assignGrain(Ball::BallState _state, std::vector<Grain>& grains, int _sampleBufferSamples);
    void spawnSplashes(Ball::BallState parentState);
    StereoSample mixGrains(std::vector<Grain>& grains, const juce::AudioBuffer<float>& sampleBuffer, int _sampleBufferSamples);

    // A high pass filter
    juce::IIRFilter filter;

    // Random number generator
    juce::Random random;

    juce::SmoothedValue<float> muteSmoother;


};

