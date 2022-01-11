#pragma once

#include <JuceHeader.h>

struct CosmicSlider : juce::Component
{
    bool flipped = false;
    juce::Slider slider;
    
    CosmicSlider()
    {
        slider.setSliderStyle  (juce::Slider::SliderStyle::LinearVertical);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 100, 50);
        
        addAndMakeVisible (slider);
    }
    
    void paint (juce::Graphics& g) override
    {
        slider.repaint();
    }
    
    void resized() override
    {
        slider.setBounds (getLocalBounds());
        
        if (flipped)
        {
            slider.setTransform (juce::AffineTransform::verticalFlip(slider.getHeight()));
        }

    }
};
