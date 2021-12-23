/*
  ==============================================================================

    CurvePoint.cpp
    Created: 22 Dec 2021 6:32:23pm
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#include "CurvePoint.h"
#include "Colours.h"


juce::Point<int> CurvePoint::Size = {50, 50};


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
    g.setColour (CustomColours::LightPink());
    g.fillAll();
    
    DBG ("CurvePoint::paint");
}

void CurvePoint::resized()
{
    
}



//----------------------------------------------------------------------------------------------
// MOUSE EVENTS
//----------------------------------------------------------------------------------------------

void CurvePoint::mouseEnter (const juce::MouseEvent& event)
{
    
}


void CurvePoint::mouseExit (const juce::MouseEvent& event)
{
    
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
