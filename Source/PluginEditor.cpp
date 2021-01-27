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
    
    auto& tree = audioProcessor.parametersTreeState;
    
    startTimerHz(20);
    
//=====================================================================================================
// SCOPE PANEL
    
    addAndMakeVisible( scopeComponent );
        
//=====================================================================================================
// THRESHOLD SLIDER PANEL
    
    auto& posSlider = posThreshSlider.slider;
    addAndMakeVisible( posThreshSlider );
    posThreshAttachment = std::make_unique<SliderAttachment>( tree, "positive threshold", posSlider );
    
    auto& negSlider = negThreshSlider.slider;
    negThreshSlider.flipped = true;
    addAndMakeVisible( negThreshSlider );
    negThreshAttachment = std::make_unique<SliderAttachment>( tree, "negative threshold", negSlider );
    
    addAndMakeVisible( notLinked );
    notLinked.setRadioGroupId( LinkedButtons );
    notLinkAttachment = std::make_unique<ButtonAttachment>( tree, "not linked", notLinked );
    notLinked.onClick = [&]
    {
        updateToggleState( &notLinked, "not linked" );
        posSlider.onValueChange = [&] { };//posSlider.setValue(posSlider.getValue()); };
        negSlider.onValueChange = [&] { };//negSlider.setValue(negSlider.getValue()); };
    };
    
    addAndMakeVisible( absoluteRadio );
    absoluteRadio.setRadioGroupId( LinkedButtons );
    absoluteAttachment = std::make_unique<ButtonAttachment>( tree, "absolute", absoluteRadio );
    absoluteRadio.onClick = [&]
    {
        updateToggleState( &absoluteRadio, "absolute" );
        posSlider.onValueChange = [&] { negSlider.setValue(posSlider.getValue()); };
        negSlider.onValueChange = [&] { posSlider.setValue(negSlider.getValue()); };
    };
    
    addAndMakeVisible( relativeRadio );
    relativeRadio.setRadioGroupId( LinkedButtons );
    relativeAttachment = std::make_unique<ButtonAttachment>( tree, "relative", relativeRadio );
    relativeRadio.onClick = [&]
    {
        updateToggleState( &relativeRadio, "relative" );
        
        posSlider.onValueChange = [&]
        {
            negSlider.setValue( juce::jmax(negSlider.getValue() - (1.0 - posSlider.getValue()), 0.0) );
        };
        
        negSlider.onValueChange = [&]
        {
            posSlider.setValue( juce::jmax(posSlider.getValue() - (1.0 - negSlider.getValue()), 0.0) );
        };
    };
    
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
    
    addAndMakeVisible( gainKnob );
    gainKnobAttachment = std::make_unique<SliderAttachment>( tree,
                                                             "gain",
                                                             gainKnob.knob );
    
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
    g.fillRect( thresholdBackgroundArea.expanded(innerWindowPadding, 0) );
    g.fillRect( meterPanel );
    g.fillRect( controlPanel );
    
    // INSIDE PANELS
    juce::Colour lighterBlueBG = myColours[Colours::BLUE_MID].withMultipliedAlpha(0.95f);
    g.setColour( lighterBlueBG );
    
    g.fillRect( scopePanel.reduced(innerWindowPadding) );
    
    g.setColour( lighterBlueBG.withMultipliedAlpha(0.5f) );
    g.fillRect( controlPanel.reduced(innerWindowPadding) );
    
    g.setColour( myColours[Colours::BLUE_DARK] );
    g.fillRect( thresholdBackgroundArea.reduced(0, innerWindowPadding) );
    g.fillRect( meterPanel.reduced(innerWindowPadding) );
    
    const float x               = thresholdBackgroundArea.getX();
    const float w               = thresholdBackgroundArea.getWidth();
    const float yCentre         = thresholdBackgroundArea.getCentreY();
    const float centreDashes[2] = { 8.f, 5.f };
    
    juce::Line<float> l{ x, yCentre, x+w, yCentre };
    
    g.setColour( juce::Colours::whitesmoke );
    g.drawDashedLine( l, centreDashes, 2 );
}

void CosmicClipperAudioProcessorEditor::resized()
{
    
//=====================================================================================================
    
    float mainWindowPadding = juce::jmin( getBounds().getWidth(), getBounds().getHeight() ) * 0.02f;
    juce::Rectangle<int> r  = getBounds().reduced( mainWindowPadding );
    
//=====================================================================================================
    
    auto topBox = r.removeFromTop( r.getHeight() * 0.7f );
    
//=====================================================================================================
        
    scopePanel = topBox.removeFromLeft( topBox.getWidth() * 0.7f );
    auto scopeArea = scopePanel.reduced( innerWindowPadding );
    const float scopeTraceScaler = 0.45f;
    
    scopeComponent.withBackgroundColour( myColours[Colours::BLUE_MID] )
                  .withLineColour( myColours[Colours::PINK_LIGHT] )
                  .withScaler( scopeTraceScaler )
                  .setBounds( scopeArea );
    
//=====================================================================================================
    
    int meterPanelWidth = topBox.getWidth();
    
//=====================================================================================================
    
    const float sliderVerticleScale = 21.f;
    const int sliderVerticleOffset  = 9;
    const int sliderAreaHeight = topBox.getHeight() * scopeTraceScaler / sliderVerticleScale;
    
    thresholdBackgroundArea = topBox.reduced( innerWindowPadding, 0 )
                                    .translated( -innerWindowPadding/2, 0 );
    
    auto topControlArea = thresholdBackgroundArea.reduced( 0, sliderAreaHeight );
    int xPadding = topControlArea.getWidth()/6;
    auto thresholdSliderArea = topControlArea.removeFromLeft( xPadding );
    
    posThreshSlider.setBounds( thresholdSliderArea.removeFromTop(thresholdSliderArea.getHeight() * 0.5f)
                                                  .translated(0, sliderVerticleOffset) );
    
    negThreshSlider.setBounds( thresholdSliderArea.removeFromTop(posThreshSlider.getBounds().getHeight())
                                                  .translated(0, -sliderVerticleOffset) );
    
    notLinked.setBounds( thresholdSliderArea.removeFromLeft(xPadding) );
    
//=====================================================================================================
    
    // Last sliver
    thresholdControlArea = topBox;
    
//=====================================================================================================

    meterPanel = r.removeFromRight( meterPanelWidth )
                  .reduced( 0, innerWindowPadding )
                  .translated( -innerWindowPadding/2 , innerWindowPadding );
    
    auto meterArea = meterPanel.reduced( innerWindowPadding * 3 );
    
    float meterAreaSpacing = meterArea.getWidth() / 5.f;
    
    inputMeter.setBounds( meterArea.removeFromLeft(meterAreaSpacing) );
    
    dbScale.ticks   = inputMeter.ticks;
    dbScale.yOffset = inputMeter.getY();
    dbScale.setBounds( meterArea.removeFromLeft(meterAreaSpacing) );
    
    outputMeter.setBounds( meterArea );
    
//=====================================================================================================
    
    controlPanel = r.reduced( 0, innerWindowPadding )
                    .translated( 0, innerWindowPadding );
    
    auto knobPanel = controlPanel.reduced( innerWindowPadding );
    
    gainKnob.setBounds(  knobPanel.removeFromLeft(knobPanel.getCentreX()) );
    
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
