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
    unlinkedThresholdsAttachment = std::make_unique<ButtonAttachment>( tree, "unlinked thresholds", unlinkedRadio );
    
    unlinkedRadio.onClick = [&]
    {
        updateToggleState( &unlinkedRadio, "unlinked thresholds" );
        posSlider.onValueChange = [&] { };
        negSlider.onValueChange = [&] { };
    };
    
    addAndMakeVisible( absoluteRadio );
    absoluteRadio.setRadioGroupId( LinkedButtons );
    
    absoluteRadio.onClick = [&]
    {
        updateToggleState( &absoluteRadio, "absolute" );
        posSlider.onValueChange = [&] { negSlider.setValue(posSlider.getValue()); };
        negSlider.onValueChange = [&] { posSlider.setValue(negSlider.getValue()); };
    };
    
    addAndMakeVisible( relativeRadio );
    relativeRadio.setRadioGroupId( LinkedButtons );
    relativeRadio.setToggleState( audioProcessor.isThreshRelative(), juce::NotificationType::sendNotification );
    
    relativeRadio.onClick = [&]
    {
        updateToggleState( &relativeRadio, "relative" );
        
        posSlider.onValueChange = [&]
        {
            auto val = negSlider.getValue();
            negSlider.setValue( val - posSlider.getValue() );
        };
        
        negSlider.onValueChange = [&]
        {
            auto val = posSlider.getValue();
            posSlider.setValue( val - negSlider.getValue() );
        };
    };
    
    addAndMakeVisible( algoLinkRadio );
    algoLinkAttachment = std::make_unique<ButtonAttachment>( tree, "link algorithms", algoLinkRadio );
    algoLinkRadio.setEnabled( audioProcessor.isAlgoLinked() );
    
    addAndMakeVisible( posAlgoModKnob );
    posAlgoModAttachment = std::make_unique<SliderAttachment>( tree, "positive algorithm modifier", posAlgoModKnob.knob );
    
    posAlgoModKnob.knob.onValueChange = [this]
    {
        if( algoLinkRadio.getToggleState() )
            negAlgoModKnob.knob.setValue( posAlgoModKnob.knob.getValue() );
    };
    
    addAndMakeVisible( negAlgoModKnob );
    negAlgoModAttachment = std::make_unique<SliderAttachment>( tree, "negative algorithm modifier", negAlgoModKnob.knob );
    
    negAlgoModKnob.knob.onValueChange = [this]
    {
        if( algoLinkRadio.getToggleState() )
            posAlgoModKnob.knob.setValue( negAlgoModKnob.knob.getValue() );
    };
    
    addAndMakeVisible( posAlgoLabel );
    posAlgoLabel.setFont( {12.f} );
    
    addAndMakeVisible( posAlgoMenu );
    posAlgoMenu.addItem( "Hard Clipping", 1 );
    posAlgoMenu.addItem( "Tanh", 2 );
    posAlgoMenu.addItem( "Sinx", 3 );
    posAlgoMenu.addItem( "Cosx", 4 );
    
    posAlgoMenuAttachment = std::make_unique<ComboBoxAttachment>( tree, "positive algorithm", posAlgoMenu );
    posAlgoMenu.setSelectedId( audioProcessor.getPosAlgoType() + 1 );

    posAlgoMenu.onChange = [this]
    {
        if( algoLinkRadio.getToggleState() )
            negAlgoMenu.setSelectedId( posAlgoMenu.getSelectedId() );
    };
    
    addAndMakeVisible( negAlgoLabel );
    negAlgoLabel.setFont( {12.f} );
    
    addAndMakeVisible( negAlgoMenu );
    negAlgoMenu.addItem( "Hard Clipping", 1 );
    negAlgoMenu.addItem( "Tanh", 2 );
    negAlgoMenu.addItem( "Sinx", 3 );
    negAlgoMenu.addItem( "Cosx", 4 );
    
    negAlgoMenuAttachment = std::make_unique<ComboBoxAttachment>( tree, "negative algorithm", negAlgoMenu );
    negAlgoMenu.setSelectedId( audioProcessor.getNegAlgoType() + 1 );
    
    negAlgoMenu.onChange = [this]
    {
        if( algoLinkRadio.getToggleState() )
            posAlgoMenu.setSelectedId( negAlgoMenu.getSelectedId() );
    };
    
//=====================================================================================================
// METER PANEL
    
    addAndMakeVisible( inputMeter );
    inputMeter.withBackgroundColour( myColours[Colours::PINK_DARK] )
              .withMeterColour( myColours[Colours::PINK_LIGHT] );
    
    
    addAndMakeVisible( inputLevelSlider );
    inputLevelSlider.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
    inputLevelSlider.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
    inputLevelSlider.setSkewFactor( 0.1 );
    inputLevelAttachment = std::make_unique<SliderAttachment>( tree, "input level", inputLevelSlider );
    
    addAndMakeVisible( dbScale );
    
    addAndMakeVisible( outputLevelSlider );
    outputLevelSlider.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
    outputLevelSlider.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
    outputLevelSlider.setRange( juce::Decibels::decibelsToGain(NEGATIVE_INFINITY_DB),
                                juce::Decibels::decibelsToGain(MAX_DB) );
    
    outputLevelAttachment = std::make_unique<SliderAttachment>( tree, "output level", outputLevelSlider );
    
    addAndMakeVisible( outputMeter );
    outputMeter.withBackgroundColour( myColours[Colours::PINK_DARK] )
               .withMeterColour( myColours[Colours::PINK_LIGHT] );
    
//=====================================================================================================
// CONTROL PANEL
    
    addAndMakeVisible( posGainKnob );
    posGainAttachment = std::make_unique<SliderAttachment>( tree, "pos gain", posGainKnob.knob );
    
    addAndMakeVisible( negGainKnob );
    negGainAttachment = std::make_unique<SliderAttachment>( tree, "neg gain", negGainKnob.knob );
    
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
    
    g.drawText( "Positive Threshold Algorithm",
                algoControlArea.getX(), posSliderTop,
                algoControlArea.getWidth(), 20,
                juce::Justification::centred );
    
    int negSliderBottom = negThreshSlider.getBounds().getY() + negThreshSlider.getBounds().getHeight() - 30;
    
    g.drawText( "Negative Threshold Algorithm",
                algoControlArea.getX(), negSliderBottom,
                algoControlArea.getWidth(), 20,
                juce::Justification::centred );
    
    int inputTextX = inputMeter.getX() + ((inputLevelSlider.getX() - inputMeter.getX()) / 2) - 20;
    int inputTextY = inputMeter.getY() - 20;
    
    g.drawText( "Input Level", inputTextX - 10, inputTextY, 100, 10, juce::Justification::centred );
    
    int outputTextX = outputMeter.getX() + ((outputLevelSlider.getX() - outputMeter.getX()) / 2) - 20;
    int outputTextY = outputMeter.getY() - 20;
    
    g.drawText( "Output Level", outputTextX + 10, outputTextY, 100, 10, juce::Justification::centred );
    
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
    
    radioButtonArea = algoControlArea.withSizeKeepingCentre( topBox.getWidth()/1.5f, topBox.getHeight()/8 );
    
    auto btnArea = radioButtonArea;
    const int divisions = 4;
    const int divWidth  = btnArea.getWidth() / divisions;
    
    unlinkedRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    absoluteRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    relativeRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    algoLinkRadio.setBounds( btnArea.removeFromLeft(divWidth) );
    
    auto posArea = algoControlArea.withTrimmedBottom( radioButtonArea.getY()+radioButtonArea.getHeight() );
    posAlgoMenu.setBounds( posArea.withSizeKeepingCentre(radioButtonArea.getWidth()/1.5f, btnArea.getHeight()/3)
                                  .translated(0, -40) );
    
    posAlgoModKnob.setBounds( posArea.reduced(12)
                                     .translated(0, 50) );
    
    auto negArea = algoControlArea.withTrimmedTop( radioButtonArea.getY()+radioButtonArea.getHeight() );
    negAlgoMenu.setBounds( negArea.withSizeKeepingCentre(radioButtonArea.getWidth()/1.5f, btnArea.getHeight()/3)
                                  .translated(0, 32) );
    
    negAlgoModKnob.setBounds( negArea.reduced(12)
                                     .translated(0, -30) );

//=====================================================================================================

    meterPanel = r.removeFromRight( meterPanelWidth )
                  .reduced( 0, innerWindowPadding )
                  .translated( -innerWindowPadding/2 , innerWindowPadding );
    
    auto meterArea = meterPanel.reduced(innerWindowPadding * 3).withTrimmedTop(20);
    
    float meterAreaSpacing = meterArea.getWidth() / 5.f;
    
    inputMeter.setBounds( meterArea.removeFromLeft(meterAreaSpacing) );
    
    inputLevelSlider.setBounds( meterArea.removeFromLeft(meterAreaSpacing)
                                         .withHeight(meterArea.getHeight() * 1.1)
                                         .translated(0, -meterArea.getHeight() * 0.05) );
    
    
    meterArea.removeFromLeft(meterAreaSpacing);
    
    dbScale.ticks   = inputMeter.ticks;
    dbScale.yOffset = inputMeter.getY();
    dbScale.setBounds( inputLevelSlider.getRight() + innerWindowPadding, 0, 30, getHeight() );
    
    dbScale.toBack();
    
    outputLevelSlider.setBounds( meterArea.removeFromLeft(meterAreaSpacing)
                                          .withHeight(meterArea.getHeight() * 1.1)
                                          .translated(0, -meterArea.getHeight() * 0.05) );
    
    outputMeter.setBounds( meterArea );
    
//=====================================================================================================
    
    controlPanel = r.reduced( 0, innerWindowPadding )
                    .translated( 0, innerWindowPadding );
    
    auto knobPanel = controlPanel.reduced( innerWindowPadding );
    
    int knobPanelDiv = knobPanel.getWidth() / 2;
    
    posGainKnob.setBounds( knobPanel.removeFromLeft(knobPanelDiv) );
    negGainKnob.setBounds( knobPanel.removeFromLeft(knobPanelDiv) );
    
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
