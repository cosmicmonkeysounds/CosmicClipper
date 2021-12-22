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
    
    startTimerHz (20);
    
    addAndMakeVisible (panel);
    
    setSize (1100, 700);
}


CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
    stopTimer();
}


void CosmicClipperAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void CosmicClipperAudioProcessorEditor::resized()
{
    panel.setBounds (getLocalBounds().reduced (50));
}

void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
}
