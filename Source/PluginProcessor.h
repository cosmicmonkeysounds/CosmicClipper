/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define SINE_TEST 1

#include <JuceHeader.h>

#include "ScopeComponent.h"
#include "Fifo.h"


class CosmicClipperAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CosmicClipperAudioProcessor();
    ~CosmicClipperAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    // my stuff
    //==============================================================================
    
    //==============================================================================
    // for pushing shit to graphics thread safely
    Fifo<juce::AudioBuffer<float>> fifo;
    
    //==============================================================================
    // for keeping track of the gui elements and their values
    juce::AudioProcessorValueTreeState parametersTreeState;
    
    //==============================================================================
    // holds all transfer function info
    enum ClippingTypes
    {
        HardClipping,
        SoftClippingTanh,
        Strange1
    };
    
    
    //==============================================================================
    // list of clipping functions
    std::vector< std::function<void(float&)> > transferFuncs
    {
        //==============================================================================
        // HardClipping
        //==============================================================================
        [this]( float& currentSample )
        {
            if( currentSample > currPosThresh )
                currentSample = currPosThresh;
                
            if( currentSample < currNegThresh )
                currentSample = currNegThresh;
        },
        
        //==============================================================================
        // SoftClippingTanh
        //==============================================================================
        [this]( float& currentSample )
        {
            currentSample = juce::dsp::FastMathApproximations::tanh( currentSample );
        }
    };
    
    AudioBufferQueue<float>& getAudioBufferQueue()
    {
        return scopeDataQueue;
    }
    
private:
    
    //==============================================================================
    // Visualiser stuff
    //==============================================================================
    
    AudioBufferQueue<float> scopeDataQueue;
    ScopeDataCollector<float> scopeDataCollector;
    
    //==============================================================================
    // the cross-thread parameters that get attached to the
    // parametersTreeState in PluginProcessor.cpp ctor
    std::atomic<float> *posThreshParam  = nullptr,
                       *negThreshParam  = nullptr,
                       *linkThreshParam = nullptr;
    
    std::atomic<bool> linkedThreshold{false};
    
    //==============================================================================
    // for calculating smooth ramping in processBlock()
    float currPosThresh{1.f},
          prevPosThresh{1.f},
          currNegThresh{-1.f},
          prevNegThresh{-1.f};

    
//==============================================================================
// for testing purposes 
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
