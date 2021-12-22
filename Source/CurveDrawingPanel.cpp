/*
  ==============================================================================

    CurveDrawingPanel.cpp
    Created: 22 Dec 2021 9:15:58am
    Author:  J P Janigan-Mills

  ==============================================================================
*/

#include "CurveDrawingPanel.h"


//-----------------------------------------------------------
// CONSTRUCTORS
//-----------------------------------------------------------


CurveDrawingPanel::CurveDrawingPanel()
    : userDrawnCurve()
{
    //addAndMakeVisible (userDrawnCurve);
    m_xyPairs = std::vector<int>();
}


CurveDrawingPanel::~CurveDrawingPanel()
{
    
}



//-----------------------------------------------------------
// DRAWING
//-----------------------------------------------------------


void CurveDrawingPanel::resized() 
{
    width  = getWidth();
    height = getHeight();
    
    //juce::Rectangle<int> bounds = getLocalBounds();
    
    // fill a new dynamic array with as many pixels as the panel is wide with 0's
    std::vector<int> xyPairs (width, 0);
    
    for (int i = 0; i < juce::jmin (m_xyPairs.size(), xyPairs.size()); ++i)
    {
        xyPairs[i] = m_xyPairs[i];
    }
    
    m_xyPairs = std::move (xyPairs);
    
    //userDrawnCurve.setBounds (bounds);
}

void CurveDrawingPanel::paint (juce::Graphics& g)
{
    g.fillAll (CustomColours::DarkBlue());
    
    
    if (contains (m_CurrentMousePosition))
    {
        g.setColour (juce::Colours::white);
        g.drawSingleLineText (m_CurrentMousePosition.toString(), 50, 50);
        
        m_xyPairs[m_CurrentMousePosition.x] = m_CurrentMousePosition.y;
    }
    
    m_DrawnPath.clear();
    m_DrawnPath.startNewSubPath (0, m_xyPairs[0]);
    
    for (int i = 1; i < m_xyPairs.size(); ++i)
    {
        int y = m_xyPairs[i];
        
        if (y != 0)
        {
            m_DrawnPath.lineTo (i, m_xyPairs[i]);
        }
    }
    
    m_DrawnPath.closeSubPath();
    
    g.strokePath (m_DrawnPath, juce::PathStrokeType (5.0f));
    
}



//-----------------------------------------------------------
// MOUSE EVENTS
//-----------------------------------------------------------


void CurveDrawingPanel::mouseDown (const juce::MouseEvent& event)
{
    m_CurrentMousePosition = event.getPosition();
    
    
    
    this->repaint();
}


void CurveDrawingPanel::mouseUp (const juce::MouseEvent& event)
{
    m_CurrentMousePosition = juce::Point<int> (-1, -1);
    this->repaint();
}


void CurveDrawingPanel::mouseDrag (const juce::MouseEvent& event)
{
    m_CurrentMousePosition = event.getPosition();
    
    if (! contains (m_CurrentMousePosition))
    {
        mouseUp (event);
        return;
    }
    
    this->repaint();
}
