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
// SCOPE PANEL
    
    addAndMakeVisible( scopeComponent );
        
//=====================================================================================================
// THRESHOLD SLIDER PANEL
    
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
// METER PANEL
    
    addAndMakeVisible( inputMeter );
    inputMeter.withBackgroundColour( myColours[Colours::PINK_DARK] )
              .withMeterColour( myColours[Colours::PINK_LIGHT] );
    
    addAndMakeVisible( dbScale );
    
    addAndMakeVisible( outputMeter );
    outputMeter.withBackgroundColour( myColours[Colours::PINK_DARK] )
               .withMeterColour( myColours[Colours::PINK_LIGHT] );
    
//=====================================================================================================
// CONTROL PANEL
    
    addAndMakeVisible( inputKnob );
    inputKnobAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState,
                                                              "input gain",
                                                              inputKnob.knob );
    
    addAndMakeVisible( outputKnob );
    outputKnobAttachment = std::make_unique<SliderAttachment>( audioProcessor.parametersTreeState,
                                                               "output gain",
                                                               outputKnob.knob );
    
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
    // BACKGROUND
    g.fillAll( myColours[Colours::BLUE_DARK] );
    
    // OUTLINES OF PANELS
    g.setColour( myColours[Colours::PINK_MID] );
    
    g.fillRect( scopePanel );
    g.fillRect( thresholdBackgroundArea );
    g.fillRect( meterPanel );
    g.fillRect( controlPanel );
    
    // INSIDE PANELS
    juce::Colour lighterBlueBG = myColours[Colours::BLUE_MID].withMultipliedAlpha(0.95f);
    g.setColour( lighterBlueBG );
    
    g.fillRect( scopePanel.reduced(innerWindowPadding) );
    
    g.setColour( lighterBlueBG.withMultipliedAlpha(0.5f) );
    g.fillRect( controlPanel.reduced(innerWindowPadding) );
    
    g.setColour( myColours[Colours::BLUE_DARK] );
    g.fillRect( thresholdBackgroundArea.reduced(innerWindowPadding) );
    g.fillRect( meterPanel.reduced(innerWindowPadding) );
}

void CosmicClipperAudioProcessorEditor::resized()
{
    
//=====================================================================================================
    
    float mainWindowPadding = juce::jmin( getBounds().getWidth(), getBounds().getHeight() ) * 0.02f;
    juce::Rectangle<int> r  = getBounds().reduced( mainWindowPadding );
    
//=====================================================================================================
    
    auto visualiserArea = r.removeFromTop( r.getHeight() * 0.7f );
    
//=====================================================================================================
        
    scopePanel = visualiserArea.removeFromLeft( visualiserArea.getWidth() * 0.8f );
    auto scopeArea = scopePanel.reduced( innerWindowPadding );
    const float scopeTraceScaler = 0.45f;
    
    scopeComponent.withBackgroundColour( myColours[Colours::BLUE_MID] )
                  .withLineColour( myColours[Colours::PINK_LIGHT] )
                  .withScaler( scopeTraceScaler )
                  .setBounds( scopeArea );
    
//=====================================================================================================
    
    const float sliderVerticleScale = 17.85f;
    const int sliderVerticleOffset  = 9;
    const int sliderAreaHeight = visualiserArea.getHeight() * scopeTraceScaler / sliderVerticleScale;
    
    thresholdBackgroundArea = visualiserArea.removeFromLeft( visualiserArea.getWidth() * 0.3f );
    auto thresholdSliderArea = thresholdBackgroundArea.reduced( 0, sliderAreaHeight );
    
    posThreshSlider.setBounds( thresholdSliderArea.removeFromTop(thresholdSliderArea.getHeight() * 0.5f)
                                                  .translated(0, sliderVerticleOffset) );
    
    negThreshSlider.setBounds( thresholdSliderArea.removeFromTop(posThreshSlider.getBounds().getHeight())
                                                  .translated(0, -sliderVerticleOffset) );
    
//=====================================================================================================
    
    meterPanel = visualiserArea;
    auto meterArea = meterPanel.reduced( innerWindowPadding * 2 );
    
    inputMeter.setBounds( meterArea.removeFromLeft(meterArea.getWidth() / 3) );
    
    dbScale.ticks   = inputMeter.ticks;
    dbScale.yOffset = inputMeter.getY();
    dbScale.setBounds( meterArea.removeFromLeft(meterArea.getWidth() / 2) );
    
    outputMeter.setBounds( meterArea );
    
//=====================================================================================================
    
    controlPanel = r.reduced( 0, innerWindowPadding )
                    .translated( 0, innerWindowPadding );
    
    auto knobPanel = controlPanel.reduced( innerWindowPadding );
    
    inputKnob.setBounds(  knobPanel.removeFromLeft(knobPanel.getCentreX()) );
    outputKnob.setBounds( knobPanel.removeFromLeft(knobPanel.getCentreX()) );
    
//=====================================================================================================
    
}

void CosmicClipperAudioProcessorEditor::timerCallback()
{
    
    if( audioProcessor.scopeFifo.pull(scopeGraphicsBuffer) )
    {
        
        juce::AudioBuffer<float> centerChannelBuffer{ 1, scopeGraphicsBuffer.getNumSamples() };
        centerChannelBuffer.clear();
        
        for( int sample = 0; sample < scopeGraphicsBuffer.getNumSamples(); ++sample )
        {
            float inputSample = 0.f;
            
            for( int channel = 0; channel < scopeGraphicsBuffer.getNumChannels(); ++channel )
            {
                if( const float* inputChannel = scopeGraphicsBuffer.getWritePointer(channel) )
                {
                    inputSample += inputChannel[sample];
                }
            } // end of channel loop
            
            //centerChannelBuffer.setSample( 1, sample, inputSample / 2.f );
            
        } // end of sample loop
        
    } // end of scopeFifo pull
    
    scopeComponent.updateThresholds( audioProcessor.getPosThresh(), audioProcessor.getNegThresh() );
    
    if( audioProcessor.inputFifo.pull(inputGraphicsBuffer) )
    {
        int numSamples  = inputGraphicsBuffer.getNumSamples();
        int numChannels = inputGraphicsBuffer.getNumChannels();
        
        float rmsLevel = 0.f;
        
        for( int channel = 0; channel < numChannels; ++channel )
        {
            rmsLevel += inputGraphicsBuffer.getRMSLevel( channel, 0, numSamples );
        }
        
        rmsLevel = juce::Decibels::gainToDecibels( rmsLevel / (float)numChannels );
        inputMeter.update( rmsLevel );
    }
    
    if( audioProcessor.outputFifo.pull(outputGraphicsBuffer) )
    {
        int numSamples  = outputGraphicsBuffer.getNumSamples();
        int numChannels = outputGraphicsBuffer.getNumChannels();
        
        float rmsLevel = 0.f;
        
        for( int channel = 0; channel < numChannels; ++channel )
        {
            rmsLevel += outputGraphicsBuffer.getRMSLevel( channel, 0, numSamples );
        }
        
        rmsLevel = juce::Decibels::gainToDecibels( rmsLevel / (float)numChannels );
        outputMeter.update( rmsLevel );
    }

}
