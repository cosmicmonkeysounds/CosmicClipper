/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CosmicClipperAudioProcessorEditor::CosmicClipperAudioProcessorEditor( CosmicClipperAudioProcessor& p )
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    startTimerHz(20);
    
    addAndMakeVisible(gl);
    
    setSize(1100, 700);
}


CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
    stopTimer();
}


void CosmicClipperAudioProcessorEditor::paint( juce::Graphics& g )
{
    
}

void CosmicClipperAudioProcessorEditor::resized()
{
    gl.setBounds(getLocalBounds());
}

void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
    if( audioProcessor.fifo.pull(graphicsBuffer) )
    {
    }
    
}
