/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Colours.h"



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
    g.fillAll (CustomColours::DarkBlue());
}



void CosmicClipperAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    panel.setBounds (bounds.reduced (50));
}



void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
}
