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
    
    setLookAndFeel( &customColour );
    
    startTimerHz(20);
        
//=====================================================================================================
    
    addAndMakeVisible( posThreshSlider );
    posThreshAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState,
                                                              "positive threshold",
                                                              posThreshSlider.slider );
    

    negThreshSlider.flipped = true;
    addAndMakeVisible( negThreshSlider );
    negThreshAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState,
                                                              "negative threshold",
                                                              negThreshSlider.slider );
    
//=====================================================================================================
    
    addAndMakeVisible( scopeComponent );
    
//=====================================================================================================
    
    setSize( 1100, 700 );
}


CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
    stopTimer();
}


void CosmicClipperAudioProcessorEditor::paint( juce::Graphics& g )
{
    g.fillAll( myColours[Colours::BLUE_DARK] );
}


void CosmicClipperAudioProcessorEditor::resized()
{
    
//=====================================================================================================
    
    float mainWindowPadding = juce::jmin( getBounds().getWidth(), getBounds().getHeight() ) * 0.02f;
    juce::Rectangle<int> r = getBounds().reduced( mainWindowPadding );
    
//=====================================================================================================
    
    auto visualiserArea = r.removeFromTop( r.getHeight() * 0.7f );
    
//=====================================================================================================
        
    auto scopeArea = visualiserArea.removeFromLeft( visualiserArea.getWidth() * 0.7f );
    const float scopeTraceScaler = 0.4f;
    
    scopeComponent.withBackgroundColour( myColours[Colours::BLUE_MID] )
                  .withLineColour( myColours[Colours::PINK_LIGHT] )
                  .withScaler( scopeTraceScaler )
                  .setBounds( scopeArea );
    
//=====================================================================================================
    
    const float sliderVerticleScale = 6.f;
    const int sliderVerticleOffset = 9;
    const int sliderAreaHeight = visualiserArea.getHeight() * scopeTraceScaler / sliderVerticleScale;
    
    auto thresholdSliderArea = visualiserArea.removeFromLeft( visualiserArea.getWidth() * 0.1f )
                                             .reduced( 0, sliderAreaHeight );
    
    
    posThreshSlider.setBounds( thresholdSliderArea.removeFromTop(thresholdSliderArea.getHeight() * 0.5f)
                                                  .translated(0, sliderVerticleOffset) );
    
    
    negThreshSlider.setBounds( thresholdSliderArea.removeFromTop(posThreshSlider.getBounds().getHeight())
                                                  .translated(0, -sliderVerticleOffset) );
    
//=====================================================================================================
    
    const int controlPanelVerticlePadding = 10;
    
    juce::Rectangle<int> controlArea = r;
    
    //posThreshKnob.setBounds( controlArea.removeFromLeft(controlArea.getHeight()/5) );
    
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
