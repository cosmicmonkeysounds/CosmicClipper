/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CosmicClipperAudioProcessorEditor::CosmicClipperAudioProcessorEditor (CosmicClipperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), scopeComponent( p.getAudioBufferQueue() )
{
    
    startTimerHz(20);
        
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    // Positive Threshold Knob
    
    posThreshKnob.setSliderStyle( juce::Slider::SliderStyle::Rotary );
    posThreshKnob.setTextBoxStyle( juce::Slider::TextBoxBelow, true, 100, 50 );
    addAndMakeVisible( posThreshKnob );
    
    posThreshAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState, "positive threshold", posThreshKnob );
    
    //==============================================================================
    // Visualiser
    //==============================================================================
    
    addAndMakeVisible( scopeComponent );
    
    
    setSize( 1100, 700 );
}

CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void CosmicClipperAudioProcessorEditor::paint (juce::Graphics& g)
{
    DBG( "Painting background" );
    g.fillAll( backgroundColour );
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
    
    
    scopeComponent.setBounds( visualiserBounds );
}

void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
    if( audioProcessor.fifo.pull(graphicsBuffer) )
    {
        
        juce::AudioBuffer<float> centerChannelBuffer{ 1, graphicsBuffer.getNumSamples() };
        centerChannelBuffer.clear();
        
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
            
            //centerChannelBuffer.setSample( 1, sample, inputSample / 2.f );
            
        } // end of sample loop
        
    } // end of fifo pull

}
