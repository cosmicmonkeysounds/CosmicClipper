//
//  CurveDrawingPanel.hpp
//  Cosmic Clipper
//
//  Created by J P Janigan-Mills on 2021-12-21.
//  Copyright © 2021 Cosmic Monkey Sounds. All rights reserved.
//


#pragma once
#include <JuceHeader.h>
#include <memory>


class CurveDrawingPanel : public juce::Component
{
    
public:
    
    CurveDrawingPanel() { }
    ~CurveDrawingPanel()
    {

    }
    
    
private:
    
    juce::Point<int> m_CurrentMousePosition = juce::Point<int> {0, 0};
    int width = 0, height = 0;
    
    void resized() override
    {
        width  = getWidth();
        height = getHeight();
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::lightpink);
        
        
        //if (m_CurrentMousePosition)
        //{
            g.setColour (juce::Colours::white);
            g.drawSingleLineText (m_CurrentMousePosition.toString(), 50, 50);
        //}
        
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        m_CurrentMousePosition = event.getPosition();
        this->repaint();
    }
    
    void mouseUp (const juce::MouseEvent& event) override
    {
        //m_CurrentMousePosition = nullptr;
        this->repaint();
    }
    
    void mouseDrag (const juce::MouseEvent& event) override
    {
        m_CurrentMousePosition = event.getPosition();
        
        int mouseX = m_CurrentMousePosition.x;
        int mouseY = m_CurrentMousePosition.y;
        
        if (mouseX < 0 || mouseY < 0 || mouseX > width || mouseY > height)
        {
            
        }
        
        this->repaint();
    }
    
};



