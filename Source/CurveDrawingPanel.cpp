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
    m_xyPairs  = std::vector<int>();
    m_PointMap = std::map<juce::String, CurvePoint>();
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
    std::vector<int> xyPairs (width, -1);
    
    for (int i = 0; i < juce::jmin (m_xyPairs.size(), xyPairs.size()); ++i)
    {
        xyPairs[i] = m_xyPairs[i];
    }
    
    m_xyPairs = std:: move (xyPairs);
    
    //userDrawnCurve.setBounds (bounds);
}

void CurveDrawingPanel::paint (juce::Graphics& g)
{
    g.fillAll (CustomColours::DarkGrey());
    
    
    if (contains (m_CurrentMousePosition))
    {
        m_xyPairs[m_CurrentMousePosition.x] = m_CurrentMousePosition.y;
    }

    m_DrawnPath.clear();

    for (int i = 0; i < m_xyPairs.size(); ++i)
    {
        int y = m_xyPairs[i];

        if (y > 0)
        {
            m_DrawnPath.lineTo (i, m_xyPairs[i]);
        }
    }

    g.setColour  (juce::Colours::white);
    g.strokePath (m_DrawnPath, juce::PathStrokeType (1.0f));
    
}



//-----------------------------------------------------------
// MOUSE EVENTS
//-----------------------------------------------------------


void CurveDrawingPanel::mouseDown (const juce::MouseEvent& event)
{
    m_CurrentMousePosition = event.getPosition();
    juce::String key = m_CurrentMousePosition.toString();
                
    cp = CurvePoint (m_CurrentMousePosition);
    m_PointMap[key] = std::move (cp);
    
    addAndMakeVisible (m_PointMap[key]);
    
    int xPos = m_CurrentMousePosition.x - (CurvePoint::Size.x * 0.5);
    int yPos = m_CurrentMousePosition.y - (CurvePoint::Size.y * 0.5);
    
    m_PointMap[key].setBounds (xPos, yPos, CurvePoint::Size.x, CurvePoint::Size.y);
    
    this->repaint();
}


void CurveDrawingPanel::mouseUp (const juce::MouseEvent& event)
{
    m_CurrentMousePosition = juce::Point<int> (-1, -1);
    //this->repaint();
}


void CurveDrawingPanel::mouseDrag (const juce::MouseEvent& event)
{
    m_CurrentMousePosition = event.getPosition();
    
    if (! contains (m_CurrentMousePosition))
    {
        mouseUp (event);
        return;
    }
    
    //this->repaint();
}
