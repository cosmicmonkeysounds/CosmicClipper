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
    
//    addAndMakeVisible( knobPanel );
//
//    knobPanel.inputVolumeKnob->onValueChange = [this]()
//    {
//        audioProcessor.inputVolume.set( knobPanel.inputVolumeKnob->getValue() );
//
//    };
    
    addAndMakeVisible(visualiser);
    
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
    
    visualiser.setBounds( visualiserBounds );
}

void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
    //visualiser.clear();
    
    if( audioProcessor.fifo.pull(graphicsBuffer) )
    {
        for( int sample = 0; sample < graphicsBuffer.getNumSamples(); ++sample )
        {
            float inputSample = 0.f;
            
            for( int channel = 0; channel < graphicsBuffer.getNumChannels(); ++channel )
            {
                if( const float* inputChannel = graphicsBuffer.getWritePointer(channel) )
                {
                    inputSample += inputChannel[sample];
                }
            } // end of channel loop
            
            inputSample *= 0.5f; // boost level for readability
            visualiser.pushSample( &inputSample, 1 );
            
        } // end of sample loop
        
    } // end of fifo pull
}
