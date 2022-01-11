#pragma once
#include <JuceHeader.h>
#include "Colours.h"

struct CosmicKnob : juce::Component
{
    MyColours myColours;
    
    juce::Slider knob;
    juce::Label nameLabel, valueLabel;
    
    const juce::String& name;
    juce::Rectangle<int> knobArea;
    juce::Rectangle<int> labelArea;
    const int knobPadding = 25;
    
    CosmicKnob (const juce::String& n) : name(n)
    {
        knob.setSliderStyle  (juce::Slider::SliderStyle::Rotary);
        knob.setTextBoxStyle (juce::Slider::NoTextBox, true, 100, 50);
        
        addAndMakeVisible (knob);
        
        juce::Font nameFont {juce::Font::getDefaultMonospacedFontName(), 18.f, juce::Font::FontStyleFlags::bold};
        
        nameLabel.setText (name, juce::NotificationType::dontSendNotification);
        nameLabel.setFont (nameFont);
        nameLabel.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (nameLabel);
        
        valueLabel.setText (juce::String (knob.getValue()) += "%", juce::NotificationType::dontSendNotification);
        
        juce::Font valueFont = {juce::Font::getDefaultMonospacedFontName(), 13.f, juce::Font::FontStyleFlags::bold};
        valueLabel.setFont (valueFont);
        valueLabel.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (valueLabel);

        knob.onValueChange = [this]
        {
            double percentage = juce::jmap ( knob.getValue(),
                                             knob.getMinimum(), knob.getMaximum(),
                                             0.0, 100.0 );
            
            percentage = std::ceil (percentage * 10.0) / 10.0;
            juce::String p = juce::String( percentage ) += "%";
            valueLabel.setText (p, juce::NotificationType::dontSendNotification);
        };
        
        valueLabel.setEditable (true, true);
        
        valueLabel.onTextChange = [this]
        {
            double valueFromText = valueLabel.getText().getDoubleValue();
            
            double newKnobValue = juce::jmap ( valueFromText,
                                               0.0, 100.0,
                                               knob.getMinimum(), knob.getMaximum() );
            
            knob.setValue (newKnobValue);
        };
        
    }
    
    void paint (juce::Graphics& g) override
    {

    }
    
    void resized() override
    {
        knobArea  = getLocalBounds();
        
        labelArea = knobArea.removeFromBottom( knobPadding )
                            .translated( 0, -knobPadding/2 );
        
        knobArea  = knobArea.reduced( knobPadding/2 )
                            .translated( 0, -knobPadding/5 );
        
        knob.setBounds( knobArea );
        valueLabel.setBounds( knobArea.reduced(knobArea.getWidth()/2.2f,
                                                knobArea.getHeight()/3) );
        nameLabel.setBounds( labelArea );
    }
};
