/*
  ==============================================================================

    DrawnPath.h
    Created: 22 Dec 2021 4:42:58pm
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DrawnPath
{

public:
    
    void AddPoint          (juce::Point<int> point);
    void RemovePoint       (juce::Point<int> point);
    void StartHoverOnPoint (juce::Point<int> point);
    void EndHoverOnPoint   (juce::Point<int> point);
    
    juce::Path GetPath();
    
    
private:
    
    juce::Path m_CurrentPath;
    
};
