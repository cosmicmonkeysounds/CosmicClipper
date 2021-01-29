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
    
    addAndMakeVisible( unlinkedRadio );
    unlinkedRadio.setRadioGroupId( LinkedButtons );
    unlikedThresholdsAttachment = std::make_unique<ButtonAttachment>( tree, "unlinked thresholds", unlinkedRadio );
    unlinkedRadio.onClick = [&]
    {
        updateToggleState( &unlinkedRadio, "unlinked thresholds" );
        posSlider.onValueChange = [&] { };//posSlider.setValue(posSlider.getValue()); };
        negSlider.onValueChange = [&] { };//negSlider.setValue(negSlider.getValue()); };
    };
    
    addAndMakeVisible( absoluteRadio );
    absoluteRadio.setRadioGroupId( LinkedButtons );
    //absoluteAttachment = std::make_unique<ButtonAttachment>( tree, "absolute", absoluteRadio );
    absoluteRadio.onClick = [&]
    {
        updateToggleState( &absoluteRadio, "absolute" );
        posSlider.onValueChange = [&] { negSlider.setValue(posSlider.getValue()); };
        negSlider.onValueChange = [&] { posSlider.setValue(negSlider.getValue()); };
    };
    
    addAndMakeVisible( relativeRadio );
    relativeRadio.setRadioGroupId( LinkedButtons );
    //relativeAttachment = std::make_unique<ButtonAttachment>( tree, "relative", relativeRadio );
    relativeRadio.onClick = [&]
    {
        updateToggleState( &relativeRadio, "relative" );
        
        posSlider.onValueChange = [&]
        {
            DBG( "negSlider: " << negSlider.getValue() );
            DBG( "posSlider: " << posSlider.getValue() );
            auto val = negSlider.getValue() - (1.0 - posSlider.getValue());
            negSlider.setValue( val );
        };
        
        negSlider.onValueChange = [&]
        {
            auto val = posSlider.getValue() - (1.0 - negSlider.getValue());
            posSlider.setValue( val );
        };
    };
    
    addAndMakeVisible( algoLinkRadio );
    algoLinkAttachment = std::make_unique<ButtonAttachment>( tree, "link algorithms", algoLinkRadio );
    
    addAndMakeVisible( posAlgoLabel );
    posAlgoLabel.setFont( {12.f} );
    
    addAndMakeVisible( posAlgoMenu );
    posAlgoMenu.addItem( "Hard Clipping", 1 );
    posAlgoMenu.addItem( "Tanh", 2);
    posAlgoMenu.setSelectedId(1);
    
    posAlgoMenuAttachment = std::make_unique<ComboBoxAttachment>( tree, "positive algorithm", posAlgoMenu );

    //posAlgoMenu.onChange = [this] { audioProcessor.setPosAlgo(posAlgoMenu.getSelectedId()); };
    
    addAndMakeVisible( negAlgoLabel );
    negAlgoLabel.setFont( {12.f} );
    
    addAndMakeVisible( negAlgoMenu );
    negAlgoMenu.addItem( "Hard Clipping", 1 );
    negAlgoMenu.addItem( "Tanh", 2 );
    negAlgoMenu.setSelectedId(1);
    
    negAlgoMenuAttachment = std::make_unique<ComboBoxAttachment>( tree, "positive algorithm", negAlgoMenu );
    
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
    const float w               = posThreshSlider.getWidth()/2;
    const float yCentre         = thresholdBackgroundArea.getCentreY();
    const float centreDashes[2] = { 8.f, 5.f };
    
    juce::Line<float> l{ x, yCentre, x+w, yCentre };
    
    g.setColour( juce::Colours::whitesmoke );
    g.drawDashedLine( l, centreDashes, 2 );
    
    //g.fillRect( algoControlArea );

    g.setColour( myColours[Colours::PINK_DARK] );
    g.fillRect( radioButtonArea );
    
    g.setColour( juce::Colours::whitesmoke );
    g.drawText( "free", unlinkedRadio.getBounds().translated(0, -radioButtonArea.getHeight()/4), juce::Justification::left );
    g.drawText( "absolute", absoluteRadio.getBounds().translated(0, -radioButtonArea.getHeight()/4), juce::Justification::left );
    g.drawText( "relative", relativeRadio.getBounds().translated(0, -radioButtonArea.getHeight()/4), juce::Justification::left );
    g.drawText( "algo link", algoLinkRadio.getBounds().translated(0, -radioButtonArea.getHeight()/4), juce::Justification::left );
    
    int posSliderTop = posThreshSlider.getBounds().getY();
    
    g.drawText( "Positive Threshold Control",
                algoControlArea.getX(), posSliderTop,
                algoControlArea.getWidth(), 20,
                juce::Justification::centred );
    
    int negSliderBottom = negThreshSlider.getBounds().getY() + negThreshSlider.getBounds().getHeight() - 30;
    
    g.drawText( "Negative Threshold Control",
                algoControlArea.getX(), negSliderBottom,
                algoControlArea.getWidth(), 20,
                juce::Justification::centred );
    
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
    
    
    
//=====================================================================================================
    
    algoControlArea = topBox.withTrimmedLeft( posThreshSlider.getWidth() )
                            .reduced( innerWindowPadding );
    
    radioButtonArea = algoControlArea.withSizeKeepingCentre( topBox.getWidth()/1.5f, topBox.getHeight()/5 );
    
    auto btnArea = radioButtonArea;
    const int divisions = 4;
    const int divWidth  = btnArea.getWidth() / divisions;
    
    unlinkedRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    absoluteRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    relativeRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    algoLinkRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    
    auto posArea = algoControlArea.withTrimmedBottom( radioButtonArea.getY()+radioButtonArea.getHeight() );
    posAlgoMenu.setBounds( posArea.withSizeKeepingCentre(radioButtonArea.getWidth()/1.5f, btnArea.getHeight()/2) );
    
    auto negArea = algoControlArea.withTrimmedTop( radioButtonArea.getY()+radioButtonArea.getHeight() );
    negAlgoMenu.setBounds( negArea.withSizeKeepingCentre(radioButtonArea.getWidth()/1.5f, btnArea.getHeight()/2) );

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
