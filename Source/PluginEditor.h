/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KnobPanel.h"


class Visualiser : public juce::AudioVisualiserComponent
{
public:
    Visualiser() : AudioVisualiserComponent(1)
    {
        setBufferSize(128);
        setSamplesPerBlock(16);
        setColours( juce::Colours::black, juce::Colours::hotpink );
    }
    
    ~Visualiser()
    {
        
    }
};


//==============================================================================


class CosmicClipperAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer
{
public:
    CosmicClipperAudioProcessorEditor (CosmicClipperAudioProcessor&);
    ~CosmicClipperAudioProcessorEditor() override;

    //==============================================================================
    // AudioProcessorEditor overrides
    //==============================================================================
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Timer overrides
    //==============================================================================

    void timerCallback() override;
    
private:

    CosmicClipperAudioProcessor& audioProcessor;
    
    juce::AudioBuffer<float> graphicsBuffer;
    Visualiser visualiser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CosmicClipperAudioProcessorEditor)
};
