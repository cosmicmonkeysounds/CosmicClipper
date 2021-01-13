/*
  ==============================================================================

    KnobPanel.cpp
    Created: 12 Jan 2021 10:17:50pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#include <JuceHeader.h>
#include "KnobPanel.h"

KnobPanel::KnobPanel()
{
    
    for( auto* knob : knobs )
    {
        knob->setSliderStyle( juce::Slider::SliderStyle::Rotary );
        knob->setTextBoxStyle( juce::Slider::TextBoxBelow, false, 0, 0 );
        knob->setAlwaysOnTop( true );
        addAndMakeVisible( knob );
    }

}

KnobPanel::~KnobPanel()
{
}

void KnobPanel::paint( juce::Graphics& g )
{
    g.fillAll(backgroundColour);
}

void KnobPanel::resized()
{
    
    int heightPos = getHeight() / 9;
    int widthPos  = static_cast<float>(getWidth()) / static_cast<float>(knobs.size());
    int i = 1;
    
    for( auto* knob : knobs )
    {
        knob->setSize( 100, 100 );
        //DBG( "X: " + std::to_string(widthPos * i) );
        knob->setTopLeftPosition( (widthPos * i) - 130, heightPos );
        i++;
    }

}
