/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "OpenGLComponent.h"


class CosmicClipperAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer
{
public:
    CosmicClipperAudioProcessorEditor(CosmicClipperAudioProcessor&);
    ~CosmicClipperAudioProcessorEditor() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    
private:
    
    CosmicClipperAudioProcessor& audioProcessor;
    
    juce::AudioBuffer<float> graphicsBuffer;
    OpenGLComponent gl;
    
//================================================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





