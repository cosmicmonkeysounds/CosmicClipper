/*
  ==============================================================================

    CurvePoint.h
    Created: 22 Dec 2021 6:32:23pm
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CurvePoint : public juce::Component
{
    
public:
    
    CurvePoint();
    CurvePoint (juce::Point<int>);
    ~CurvePoint();
    
    CurvePoint (const CurvePoint&);
    CurvePoint (CurvePoint&&);
    
    CurvePoint& operator= (const CurvePoint&);
    CurvePoint& operator= (const CurvePoint&&);
    
    static juce::Point<int> Size;

private:
    
    juce::Point<int> m_Point;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit  (const juce::MouseEvent&) override;
    void mouseUp    (const juce::MouseEvent&) override;
    void mouseDown  (const juce::MouseEvent&) override;
    void mouseDrag  (const juce::MouseEvent&) override;
    
};
