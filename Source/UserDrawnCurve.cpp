/*
  ==============================================================================

    UserDrawnCurve.cpp
    Created: 22 Dec 2021 9:16:22am
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#include "UserDrawnCurve.h"
#include "Colours.h"


//----------------------------------------------------------------------------------------------
// CONSTRUCTORS
//----------------------------------------------------------------------------------------------


UserDrawnCurve::UserDrawnCurve()
{
    m_Curve = juce::Path();
}


UserDrawnCurve::UserDrawnCurve (const UserDrawnCurve& otherCurve)
{
    m_Curve = otherCurve.m_Curve;
}



//----------------------------------------------------------------------------------------------
// GETTERS 'N' SETTERS
//----------------------------------------------------------------------------------------------


juce::Path UserDrawnCurve::getCurve() { return m_Curve; }


//----------------------------------------------------------------------------------------------
// DRAW STUFF
//----------------------------------------------------------------------------------------------


//void UserDrawnCurve::paint (juce::Graphics& g)
//{
//    //g.fillAll (CustomColours::MediumBlue());
//}
//
//
//void UserDrawnCurve::resized()
//{
//    
//}
