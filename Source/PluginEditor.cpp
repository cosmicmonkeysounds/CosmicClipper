/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CosmicClipperAudioProcessorEditor::CosmicClipperAudioProcessorEditor (CosmicClipperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    startTimerHz(20);
        
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    // Positive Threshold Knob
    
    posThreshKnob.setSliderStyle( juce::Slider::SliderStyle::Rotary );
    posThreshKnob.setTextBoxStyle( juce::Slider::TextBoxBelow, true, 100, 50 );
    addAndMakeVisible( posThreshKnob );
    
    posThreshAttachment = std::make_unique<SliderAttachment>( audioProcessor.parameters, "positive threshold", posThreshKnob );
    
    //==============================================================================
    // Visualiser
    //==============================================================================
    
    
    
    setSize( 800, 640 );
}

CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void CosmicClipperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll( getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId) );
}

void CosmicClipperAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getBounds();
    
    //knobPanel.setBounds( getLocalBounds() );
    
    juce::Rectangle<int> visualiserBounds = bounds
                                             .withHeight( bounds.getHeight() * 0.70 )
                                             .withWidth(  bounds.getWidth()  * 0.75 );
    
    const int controlPanelVerticlePadding = 10;
    
    posThreshKnob.setBounds( 0,
                             visualiserBounds.getHeight() + controlPanelVerticlePadding,
                             visualiserBounds.getWidth(),
                             bounds.getHeight() - visualiserBounds.getHeight()
                            );
}

void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
}
