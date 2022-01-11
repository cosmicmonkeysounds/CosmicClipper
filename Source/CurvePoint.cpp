/*
  ==============================================================================

    CurvePoint.cpp
    Created: 22 Dec 2021 6:32:23pm
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#include "CurvePoint.h"
#include "Colours.h"


juce::Point<int> CurvePoint::Size = {25, 25};


//----------------------------------------------------------------------------------------------
// CONSTRUCTORS / COPY / MOVE
//----------------------------------------------------------------------------------------------

CurvePoint::CurvePoint()
{
    m_Point = juce::Point<int>();
}

CurvePoint::CurvePoint (juce::Point<int> point)
{
    m_Point = point;
}

CurvePoint::~CurvePoint() {}

CurvePoint::CurvePoint (const CurvePoint&) {}
CurvePoint::CurvePoint (CurvePoint&&) {}

CurvePoint& CurvePoint::operator= (const CurvePoint&)
{
    return *this;
}

CurvePoint& CurvePoint::operator= (const CurvePoint&&)
{
    return *this;
}



//----------------------------------------------------------------------------------------------
// GRAPHICS
//----------------------------------------------------------------------------------------------


void CurvePoint::paint (juce::Graphics& g)
{
    juce::Rectangle<float> r = {Size.x * 0.8f, Size.y * 0.8f};
    
    g.setColour (CustomColours::DarkPink());
    g.fillEllipse (r);
    
    g.setColour (CustomColours::LightPink());
    g.drawEllipse (r, 2.0f);
}

void CurvePoint::resized()
{
    
}



//----------------------------------------------------------------------------------------------
// MOUSE EVENTS
//----------------------------------------------------------------------------------------------

void CurvePoint::mouseEnter (const juce::MouseEvent& event)
{
    DBG ("waz uh cracka");
}


void CurvePoint::mouseExit (const juce::MouseEvent& event)
{
    DBG ("c u lata cracka");
}


void CurvePoint::mouseUp (const juce::MouseEvent& event)
{
    
}


void CurvePoint::mouseDown (const juce::MouseEvent& event)
{
    
}


void CurvePoint::mouseDrag (const juce::MouseEvent& event)
{
    
}
