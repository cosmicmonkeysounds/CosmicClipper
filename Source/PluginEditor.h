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

    //==============================================================================
    // AudioProcessorEditor overrides
    //==============================================================================
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Timer overrides
    //==============================================================================

    void timerCallback() override;
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
private:

    CosmicClipperAudioProcessor& audioProcessor;
    
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    // Positive Threshold knob
    juce::Slider posThreshKnob;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





