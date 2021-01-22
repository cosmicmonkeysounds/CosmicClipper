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

class CosmicClipperAudioProcessorEditor;

class GenericEditor : public juce::Component
{
public:
    
    GenericEditor( CosmicClipperAudioProcessorEditor& _parent, juce::AudioProcessorValueTreeState& vts )
        : parent( _parent ), valueStateTree(vts)
    {
        posThreshLabel.setText( "Positive Threshold", juce::dontSendNotification );
        addAndMakeVisible( posThreshLabel );
        
        addAndMakeVisible( posThreshKnob );
        posThreshAttachment.reset( new SliderAttachment(valueStateTree, "positive threshold", posThreshKnob) );
        
        linkThreshButton.setButtonText( "Link Thresholds" );
        addAndMakeVisible( linkThreshButton );
        linkThreshAttachment.reset( new ButtonAttachment(valueStateTree, "link thresholds", linkThreshButton) );
        
        setSize( getBounds().getWidth(), getBounds().getHeight() );
    }
    
    //==============================================================================
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
private:
    
    CosmicClipperAudioProcessorEditor& parent;
    juce::AudioProcessorValueTreeState& valueStateTree;
    
    juce::Label  posThreshLabel;
    juce::Slider posThreshKnob;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    juce::ToggleButton linkThreshButton;
    std::unique_ptr<ButtonAttachment> linkThreshAttachment;
    
};


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
    
    juce::AudioBuffer<float> graphicsBuffer;
    Visualiser visualiser;
    
    juce::Slider posThreshKnob;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    //GenericEditor editor{ *this, audioProcessor.parameters };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CosmicClipperAudioProcessorEditor)
};


//==============================================================================


