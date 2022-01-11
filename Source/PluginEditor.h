/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"
#include "Colours.h"
#include "CurveDrawingPanel.h"


//================================================================================================

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
    
    
    const int innerWindowPadding = 5;
    
    //==============================================================================
    // GUI Elements
    //==============================================================================
    

    CurveDrawingPanel panel;
    
    
    
//================================================================================================
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





