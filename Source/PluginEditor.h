/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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

struct MyColours
{

    std::vector<juce::Colour> myColours
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
    

    juce::Colour operator[] (Colours c)
    {
        return myColours[c];
    }
};


struct MyLookAndFeel : juce::LookAndFeel_V4
{
    
    MyLookAndFeel()
    {
        setColour( juce::Slider::backgroundColourId, myColours[Colours::PINK_DARK]  );
        setColour( juce::Slider::trackColourId,      myColours[Colours::PINK_LIGHT] );
        setColour( juce::Slider::thumbColourId,      myColours[Colours::PINK_NEON]  );
    }
    
    MyColours myColours;
};



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
    
    MyLookAndFeel customColour;
    MyColours myColours;
    
    //==============================================================================
    // Oscilloscope
    //==============================================================================
    
    ScopeComponent<float> scopeComponent;
    
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    juce::Rectangle<int> thresholdBackgroundArea;
    
    struct CosmicSlider : juce::Component
    {
        bool flipped = false;
        juce::Slider slider;
        
        CosmicSlider()
        {
            DBG( "regular CTOR" );
            slider.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
            slider.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
            addAndMakeVisible( slider );
        }
        
        void paint( juce::Graphics& g )
        {
            slider.repaint();
        }
        
        void resized()
        {
            slider.setBounds( getLocalBounds() );
            
            if( flipped )
            {
                slider.setTransform( juce::AffineTransform::verticalFlip(slider.getHeight()) );
            }
        }
    };
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    CosmicSlider posThreshSlider;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    CosmicSlider negThreshSlider;
    std::unique_ptr<SliderAttachment> negThreshAttachment;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





