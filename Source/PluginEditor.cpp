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
    
    addAndMakeVisible( knobPanel );
    
    knobPanel.inputVolumeKnob->onValueChange = [this]()
    {
        audioProcessor.inputVolume.set( knobPanel.inputVolumeKnob->getValue() );
        
    };
    
    setSize( 640, 400 );
}

CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
}

//==============================================================================
void CosmicClipperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll( getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId) );
}

void CosmicClipperAudioProcessorEditor::resized()
{
    knobPanel.setBounds( getLocalBounds() );
}
