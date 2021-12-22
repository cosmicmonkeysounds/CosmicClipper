/*
  ==============================================================================

    UserDrawnCurve.h
    Created: 22 Dec 2021 9:16:22am
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>


class UserDrawnCurve
{
    
public:
    
    UserDrawnCurve();
    UserDrawnCurve (const UserDrawnCurve& otherCurve);
    
    juce::Path getCurve();
    
private:
    
    juce::Path m_Curve;
    

};
