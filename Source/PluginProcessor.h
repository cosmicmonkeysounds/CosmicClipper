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
    Fifo<juce::AudioBuffer<float>> scopeFifo, inputFifo, outputFifo;
    
    //==============================================================================
    // for keeping track of the gui elements and their values
    juce::AudioProcessorValueTreeState parametersTreeState;
    
    //==============================================================================
    // holds all transfer function info
    enum ClippingTypes
    {
        HardClipping = 0,
        Tanh,
        Strange1
    };
    
    
    //==============================================================================
    // list of clipping functions
    std::vector< std::function<void(float&)> > transferFuncs
    {
        //==============================================================================
        // 0. HardClipping
        //==============================================================================
        [this]( float& currentSample )
        {
            if( currentSample > currPosThresh )
            {
                float diff = (currentSample - currPosThresh) * posAlgoModifierParam->load();
                currentSample -= diff;
            }
                
            if( currentSample < currNegThresh )
            {
                float diff = (currentSample - currNegThresh) * negAlgoModifierParam->load();
                currentSample -= diff;
            }
                
        },
        
        //==============================================================================
        // 1. Tanh
        //==============================================================================
        [this]( float& currentSample )
        {
            if( currentSample > currPosThresh )
            {
                float mappedVal = juce::jmap( posAlgoModifierParam->load(),
                                              0.f, 1.f,
                                              1.f, 10.f );
                
                currentSample = juce::dsp::FastMathApproximations::tanh( currentSample * mappedVal );
            }
                
                
            if( currentSample < currNegThresh )
            {
                float mappedVal = juce::jmap( negAlgoModifierParam->load(),
                                                0.f, 1.f,
                                                1.f, 10.f );
                  
                currentSample = juce::dsp::FastMathApproximations::tanh( currentSample * mappedVal );
            }
        }
    };
    
    ClippingTypes posClippingType, negClippingType = HardClipping;
    
    std::function<void(float&)> posAlgo{transferFuncs[HardClipping]};
    std::function<void(float&)> negAlgo{posAlgo};
    
    void setPosAlgo( ClippingTypes type ) { posAlgo = transferFuncs[type]; }
    void setNegAlgo( ClippingTypes type ) { negAlgo = transferFuncs[type]; }
    
    AudioBufferQueue<float>& getAudioBufferQueue() { return scopeDataQueue; }
    
    float getPosThresh() { return *posThreshParam; }
    float getNegThresh() { return *negThreshParam; }
    
    bool isThreshFree()      { return *linkThreshParam < 0.5f ? false : true; }
    bool isThreshAbsolute()  { return *absoluteParam   < 0.5f ? false : true; }
    bool isThreshRelative()  { return *relativeParam   < 0.5f ? false : true; }
    
    bool isAlgoLinked()      { return *algoLinkParam < 0.f ? false : true; }
    
    int getPosAlgoType() { return (int)posAlgoParam->load(); }
    int getNegAlgoType() { return (int)negAlgoParam->load(); }
    
private:
    
    //==============================================================================
    // Visualiser stuff
    //==============================================================================
    
    AudioBufferQueue<float> scopeDataQueue;
    ScopeDataCollector<float> scopeDataCollector;
    
    //==============================================================================
    // the cross-thread parameters that get attached to the
    // parametersTreeState in PluginProcessor.cpp ctor
    std::atomic<float> *posThreshParam  = nullptr, *negThreshParam   = nullptr,
                       *linkThreshParam = nullptr, *absoluteParam    = nullptr, *relativeParam = nullptr, *algoLinkParam = nullptr,
                       *inputLevelParam = nullptr, *outputLevelParam = nullptr, *gainParam     = nullptr,
                       *posAlgoModifierParam = nullptr, *negAlgoModifierParam = nullptr,
                       *posAlgoParam    = nullptr, *negAlgoParam     = nullptr;
    
    std::atomic<bool> linkedThreshold{false};
    
    //==============================================================================
    // for calculating smooth ramping in processBlock()
    int currSampleRatio;
    
    void rampParameter( float param, float& curr, float& prev )
    {
        curr = param;
        
        if( curr != prev )
            curr += (prev - curr) * currSampleRatio;
        
        else
            prev = curr;
    }
    
    float currPosThresh{1.f},   prevPosThresh{1.f},  currNegThresh{-1.f},  prevNegThresh{-1.f},
          currInputLevel{1.f},  prevInputLevel{1.f}, currOutputLevel{1.f}, prevOutputLevel{1.f},
          currGain{1.f},        prevGain{1.f};

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
