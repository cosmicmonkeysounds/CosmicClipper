/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define SINE_TEST 1

#include <JuceHeader.h>
#include <functional>
#include "Fifo.h"

//==============================================================================
/**
*/
class CosmicClipperAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CosmicClipperAudioProcessor();
    ~CosmicClipperAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::Atomic<float> inputVolume{0.f},
                        distortionThreshold{0.f},
                        distortionCeiling{0.f},
                        outputVolume{0.f};
    
    
    Fifo<juce::AudioBuffer<float>> fifo;
    
    enum ClippingTypes
    {
        HardClipping,
        SoftClipping,
        Strange1
    };

private:
    
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float> *posThreshParam  = nullptr,
                       *negThreshParam  = nullptr,
                       *linkThreshParam = nullptr;
    
    std::atomic<bool> linkedThreshold{false};
    
    float currPosThresh{1.f},
          prevPosThresh{1.f},
          currNegThresh{-1.f},
          prevNegThresh{-1.f};
    
    
    std::vector< std::function<void(float&)> > transferFuncs
    {
        // HARD CLIPPER
        [this]( float& currentSample )
        {
            if( currentSample > currPosThresh )
                currentSample = currPosThresh;
                
            if( currentSample < currNegThresh )
                currentSample = currNegThresh;
        }
    };
    
#if SINE_TEST == 1
    
    juce::dsp::Oscillator<float> testOscillator;
    
    double currentSampleRate = 0.0,
           currentAngle      = 0.0,
           angleDelta        = 0.0;
    
    void updateAngleDelta()
    {
        auto cyclesPerSample = 120.0 / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }
    
#endif
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CosmicClipperAudioProcessor)
};
