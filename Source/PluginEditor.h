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
    
    enum Colours
    {
        BLUE_DARK,
        BLUE_MID,
        BLUE_LIGHT,
        BLUE_NEON,
        PINK_DARK,
        PINK_MID,
        PINK_LIGHT,
        PINK_NEON
    };
    
    std::vector<juce::Colour> colours
    {

        // BLUE_DARK, Rich Black FOGRA 29
        juce::Colour{ 7, 14, 31 },
        
        // BLUE_MID, Oxford Blue
        juce::Colour{ 12, 27, 64 },

        // BLUE_LIGHT, Lavender Blue
        juce::Colour{ 197, 198, 255 },
        
        // BLUE_NEON, Sky Blue Crayola
        juce::Colour{ 87, 213, 231 },
        
        // PINK_DARK, Twighlight Lavender
        juce::Colour{ 129, 55, 100 },
        
        // PINK_MID, Hot Pink
        juce::Colour{ 255, 109, 189 },
        
        // PINK_LIGHT, Cotton Candy
        juce::Colour{ 255, 190, 230 },
        
        // PINK_NEON, Paradise Pink
        juce::Colour{ 238, 66, 102 }
        
    };
    
    
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
    
    juce::Slider posThreshKnob;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    juce::Slider negThreshKnob;
    std::unique_ptr<SliderAttachment> negThreshAttachment;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





