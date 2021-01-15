/*
  ==============================================================================

    KnobPanel.h
    Created: 12 Jan 2021 10:17:50pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

#include <memory>
#include <JuceHeader.h>

//==============================================================================
/*
*/
class KnobPanel : public juce::Component
{
    
public:
    KnobPanel();
    ~KnobPanel();

    void paint( juce::Graphics& ) override;
    void resized() override;
    
    juce::Slider* distortionThresholdKnob = new juce::Slider( "Distortion Threshold" );
    juce::Slider* distortionCeilingKnob   = new juce::Slider( "Distortion Ceiling" );
    juce::Slider* outputVolumeKnob        = new juce::Slider( "Output Volume" );

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobPanel)
    
    juce::OwnedArray<juce::Slider> knobs{ 
                                          distortionThresholdKnob,
                                          distortionCeilingKnob,
                                          outputVolumeKnob };
    
    juce::Colour backgroundColour{12, 27, 51};
    
//    std::array<Slider&, 3> knobs = [inputVolumeKnob, distortionThresholdKnob, outputVolumeKnob];
};
