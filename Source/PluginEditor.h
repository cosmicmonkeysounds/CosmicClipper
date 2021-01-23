/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class CosmicClipperAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer
{
public:
    CosmicClipperAudioProcessorEditor(CosmicClipperAudioProcessor&);
    ~CosmicClipperAudioProcessorEditor() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    
    //==============================================================================
    // my stuff
    //==============================================================================
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    
private:

    // Rich Black FOGRA 29
    juce::Colour backgroundColour{ 7, 14, 31 };
    
    CosmicClipperAudioProcessor& audioProcessor;
    
    juce::AudioBuffer<float> graphicsBuffer;
    
    //==============================================================================
    // Oscilloscope
    //==============================================================================
    
    ScopeComponent<float> scopeComponent;
    
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    // Positive Threshold knob
    juce::Slider posThreshKnob;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





