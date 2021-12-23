/*
  ==============================================================================

    Cursor.h
    Created: 22 Dec 2021 4:46:58pm
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Cursor
{
    
public:
    
    enum PointMode { Linear, Quadratic, Cubic };
    
    const juce::MouseEvent& GetEvent();
    void SetEvent (const juce::MouseEvent& event);
    
private:
    
    juce::MouseEvent m_MouseEvent;
    PointMode m_PointMode;
    
};
