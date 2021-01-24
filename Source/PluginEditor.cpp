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
        
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    // Positive Threshold Knob
    
    posThreshKnob.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
    posThreshKnob.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
    addAndMakeVisible( posThreshKnob );
    
    posThreshAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState,
                                                              "positive threshold",
                                                              posThreshKnob );
    
    negThreshKnob.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
    negThreshKnob.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
    addAndMakeVisible( negThreshKnob );
    
    negThreshAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState,
                                                              "negative threshold",
                                                              negThreshKnob );
    
    //==============================================================================
    // Visualiser
    //==============================================================================
    
    addAndMakeVisible( scopeComponent );
    
    
    setSize( 1100, 700 );
}

CosmicClipperAudioProcessorEditor::~CosmicClipperAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
    stopTimer();
}

//==============================================================================
void CosmicClipperAudioProcessorEditor::paint( juce::Graphics& g )
{
    g.fillAll( myColours[Colours::BLUE_DARK] );
}

void CosmicClipperAudioProcessorEditor::resized()
{
    
    float mainWindowPadding = juce::jmin( getBounds().getWidth(), getBounds().getHeight() ) * 0.02f;
    juce::Rectangle<int> r = getBounds().reduced( mainWindowPadding );
    DBG( "Main window area: " << r.toString() );
    
    juce::Rectangle<int> visualiserArea = r.removeFromTop( r.getHeight() * 0.7f );
    DBG( "Visualiser area:  " << visualiserArea.toString() );
    
    //==============================================================================
    // Oscilloscope
    
    juce::Rectangle<int> scopeArea = visualiserArea.removeFromLeft( visualiserArea.getWidth() * 0.7f );
    const float scopeTraceScaler = 0.4f;
    DBG( "Scope area:       " << scopeArea.toString() );
    
    scopeComponent.withBackgroundColour( myColours[Colours::BLUE_MID] )
                  .withLineColour( myColours[Colours::PINK_LIGHT] )
                  .withScaler( scopeTraceScaler )
                  .setBounds( scopeArea );
    
    //==============================================================================
    // Threshold Sliders
    
    float sliderScale = 6; //JUCE_LIVE_CONSTANT( 3.5f );
    
    int sliderAreaHeight = visualiserArea.getHeight() * scopeTraceScaler / sliderScale;
    
    juce::Rectangle<int> thresholdSliderArea = visualiserArea.removeFromLeft( visualiserArea.getWidth() * 0.1f )
                                                             .reduced( 0, sliderAreaHeight );
                                                             //.translated( 0, visualiserArea.getHeight() * scopeTraceScaler / 1.5f );
    
    DBG( "Slider area:      " << thresholdSliderArea.toString() );
    
    int sliderOffset = 9; //JUCE_LIVE_CONSTANT( 5 );
    
    posThreshKnob.setBounds( thresholdSliderArea.removeFromTop(thresholdSliderArea.getHeight() * 0.5f)
                                                .translated(0, sliderOffset) );
    
    DBG( "Pos slider:       " << posThreshKnob.getBounds().toString() );
    
    
    negThreshKnob.setBounds( thresholdSliderArea.removeFromTop(posThreshKnob.getBounds().getHeight())
                                                .translated(0, -sliderOffset) );
    
    DBG( "Neg slider:       " << negThreshKnob.getBounds().toString() );
    
    //==============================================================================
    // Control Area
    
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
