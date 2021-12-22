/*
  ==============================================================================

    MyLookAndFeel.h
    Created: 21 Dec 2021 10:01:17pm
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Colours.h"

struct MyLookAndFeel : juce::LookAndFeel_V4
{
    
    MyColours myColours;
    
    MyLookAndFeel()
    {
        setColour (juce::Slider::backgroundColourId, myColours[Colours::PINK_DARK]);
        setColour (juce::Slider::trackColourId,      myColours[Colours::PINK_LIGHT]);
        setColour (juce::Slider::thumbColourId,      myColours[Colours::PINK_NEON]);
        
        setColour (juce::Slider::rotarySliderFillColourId,    myColours[Colours::BLUE_NEON]);
        setColour (juce::Slider::rotarySliderOutlineColourId, myColours[Colours::BLUE_DARK]);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           const float startAngle, const float endAngle, juce::Slider& slider)
    {
        float radius  = (float) juce::jmin (w / 2, h / 2 ) - 4.0f;
        float centreX = (float) x + (float) w * 0.5f;
        float centreY = (float) y + (float) h * 0.5f;
        float rx      = centreX - radius;
        float ry      = centreY - radius;
        float rw      = radius * 2.f;
        float angle   = startAngle + pos * (endAngle - startAngle);
        
        juce::Rectangle<float> area{ rx, ry, rw, rw };
        float outlineThickness = 4.f;
        
        g.setColour (myColours[Colours::BLUE_MID]);
        g.fillEllipse (area);
        
        g.setColour( myColours[Colours::PINK_NEON] );
        g.drawEllipse( area, outlineThickness );
        
        juce::Path pointer;
        float pointerThickness = 3.f;
        float pointerLength    = radius * 0.33f;

        pointer.addRectangle( -pointerThickness * 0.5f, -radius,
                               pointerThickness, pointerLength );
        
        pointer.applyTransform( juce::AffineTransform::rotation(angle).translated(centreX, centreY) );
        
        g.setColour( myColours[Colours::BLUE_NEON] );
        g.fillPath( pointer );
    }
    
};
