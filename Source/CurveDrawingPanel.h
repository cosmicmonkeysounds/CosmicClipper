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
#include "Colours.h"
#include "UserDrawnCurve.h"
#include <vector>
#include <utility>


class CurveDrawingPanel : public juce::Component
{
    
public:
    
    CurveDrawingPanel();
    ~CurveDrawingPanel();
    
    
private:
    
    juce::Point<int> m_CurrentMousePosition = juce::Point<int> (-1, -1);
    int width = 0, height = 0;
    
    std::vector<int> m_xyPairs;
    juce::Path m_DrawnPath;
    
    UserDrawnCurve userDrawnCurve;
    
    void resized() override;
    void paint (juce::Graphics& g) override;
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    
};



