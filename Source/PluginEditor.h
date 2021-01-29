/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"

enum Colours
{
    BLUE_DARK,
    BLUE_MID,
    BLUE_LIGHT,
    BLUE_NEON,
    PINK_DARK,
    PINK_MID,
    PINK_LIGHT,
    PINK_NEON
};

struct MyColours
{

    std::vector<juce::Colour> myColours
    {

        // BLUE_DARK, Rich Black FOGRA 29
        juce::Colour{ 7, 14, 31 },
        
        // BLUE_MID, Oxford Blue
        juce::Colour{ 12, 27, 64 },

        // BLUE_LIGHT, Lavender Blue
        juce::Colour{ 197, 198, 255 },
        
        // BLUE_NEON, Sky Blue Crayola
        juce::Colour{ 87, 213, 231 },
        
        // PINK_DARK, Twighlight Lavender
        juce::Colour{ 129, 55, 100 },
        
        // PINK_MID, Hot Pink
        juce::Colour{ 255, 109, 189 },
        
        // PINK_LIGHT, Cotton Candy
        juce::Colour{ 255, 190, 230 },
        
        // PINK_NEON, Paradise Pink
        juce::Colour{ 238, 66, 102 }
        
    };
    

    juce::Colour operator[] (Colours c)
    {
        return myColours[c];
    }
};

//================================================================================================

struct MyLookAndFeel : juce::LookAndFeel_V4
{
    
    MyColours myColours;
    
    MyLookAndFeel()
    {
        setColour( juce::Slider::backgroundColourId, myColours[Colours::PINK_DARK]  );
        setColour( juce::Slider::trackColourId,      myColours[Colours::PINK_LIGHT] );
        setColour( juce::Slider::thumbColourId,      myColours[Colours::PINK_NEON]  );
        
        setColour( juce::Slider::rotarySliderFillColourId,    myColours[Colours::BLUE_NEON] );
        setColour( juce::Slider::rotarySliderOutlineColourId, myColours[Colours::BLUE_DARK] );
    }
    
    void drawRotarySlider( juce::Graphics& g, int x, int y, int w, int h, float pos,
                           const float startAngle, const float endAngle, juce::Slider& slider )
    {
        float radius  = (float) juce::jmin( w/2, h/2 ) - 4.f;
        float centreX = (float) x + (float) w * 0.5f;
        float centreY = (float) y + (float) h * 0.5f;
        float rx      = centreX - radius;
        float ry      = centreY - radius;
        float rw      = radius * 2.f;
        float angle   = startAngle + pos * (endAngle - startAngle);
        
        juce::Rectangle<float> area{ rx, ry, rw, rw };
        float outlineThickness = 4.f;
        
        g.setColour( myColours[Colours::BLUE_MID] );
        g.fillEllipse( area );
        
        g.setColour( myColours[Colours::PINK_NEON] );
        g.drawEllipse( area, outlineThickness );
        
        juce::Path pointer;
        float pointerThickness = 3.f;
        float pointerLength    = radius * 0.33f;

        pointer.addRectangle( -pointerThickness * 0.5f, -radius,
                               pointerThickness, pointerLength );
        
        pointer.applyTransform( juce::AffineTransform::rotation(angle).translated(centreX, centreY) );
        
        g.setColour( myColours[Colours::BLUE_NEON] );
        g.fillPath( pointer );
    }
    
};

//================================================================================================

class CosmicClipperAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer
{
public:
    CosmicClipperAudioProcessorEditor(CosmicClipperAudioProcessor&);
    ~CosmicClipperAudioProcessorEditor() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    
private:
    
    CosmicClipperAudioProcessor& audioProcessor;
    
    MyLookAndFeel customColour;
    MyColours myColours;
    
    const int innerWindowPadding = 5;
    
    //==============================================================================
    // Oscilloscope
    //==============================================================================
    
    juce::Rectangle<int> scopePanel;
    
    juce::AudioBuffer<float> scopeGraphicsBuffer;
    ScopeComponent<float> scopeComponent;
    
    //==============================================================================
    // GUI Elements
    //==============================================================================
    
    struct CosmicSlider : juce::Component
    {
        bool flipped = false;
        juce::Slider slider;
        
        CosmicSlider()
        {
            slider.setSliderStyle( juce::Slider::SliderStyle::LinearVertical );
            slider.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
            addAndMakeVisible( slider );
        }
        
        void paint( juce::Graphics& g )
        {
            slider.repaint();
        }
        
        void resized()
        {
            slider.setBounds( getLocalBounds() );
            
            if( flipped )
            {
                slider.setTransform( juce::AffineTransform::verticalFlip(slider.getHeight()) );
            }
        }
    };
    
    struct CosmicKnob : juce::Component
    {
        MyColours myColours;
        
        juce::Slider knob;
        juce::Label nameLabel, valueLabel;
        
        const juce::String& name;
        juce::Rectangle<int> knobArea;
        juce::Rectangle<int> labelArea;
        const int knobPadding = 25;
        
        CosmicKnob( const juce::String& n ) : name(n)
        {
            knob.setSliderStyle( juce::Slider::SliderStyle::Rotary );
            knob.setTextBoxStyle( juce::Slider::NoTextBox, true, 100, 50 );
            addAndMakeVisible( knob );
            
            juce::Font nameFont{ juce::Font::getDefaultMonospacedFontName(), 18.f, juce::Font::FontStyleFlags::bold };
            
            nameLabel.setText( name, juce::NotificationType::dontSendNotification );
            nameLabel.setFont( nameFont );
            nameLabel.setJustificationType( juce::Justification::centred );
            addAndMakeVisible( nameLabel );
            
            valueLabel.setText( juce::String(knob.getValue()) += "%",
                                juce::NotificationType::dontSendNotification );
            
            juce::Font valueFont = { juce::Font::getDefaultMonospacedFontName(), 13.f, juce::Font::FontStyleFlags::bold };
            valueLabel.setFont( valueFont );
            valueLabel.setJustificationType( juce::Justification::centred );
            addAndMakeVisible( valueLabel );

            knob.onValueChange = [this]
            {
                double percentage = juce::jmap( knob.getValue(),
                                                knob.getMinimum(), knob.getMaximum(),
                                                0.0, 100.0 );
                
                percentage = std::ceil(percentage * 10.0) / 10.0;
                juce::String p = juce::String( percentage ) += "%";
                valueLabel.setText( p, juce::NotificationType::dontSendNotification );
            };
            
            valueLabel.setEditable( true, true );
            
            valueLabel.onTextChange = [this]
            {
                double valueFromText = valueLabel.getText().getDoubleValue();
                double newKnobValue = juce::jmap( valueFromText,
                                                  0.0, 100.0,
                                                  knob.getMinimum(), knob.getMaximum() );
                
                knob.setValue( newKnobValue );
            };
            
        }
        
        void paint( juce::Graphics& g ) override
        {

        }
        
        void resized() override
        {
            knobArea  = getLocalBounds();
            
            labelArea = knobArea.removeFromBottom( knobPadding )
                                .translated( 0, -knobPadding/2 );
            
            knobArea  = knobArea.reduced( knobPadding/2 )
                                .translated( 0, -knobPadding/5 );
            
            knob.setBounds( knobArea );
            valueLabel.setBounds( knobArea.reduced(knobArea.getWidth()/2.2f,
                                                   knobArea.getHeight()/3) );
            nameLabel.setBounds( labelArea );
        }
    };
    
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    juce::Rectangle<int> radioButtonPanel;
    
    juce::ToggleButton unlinkedRadio, absoluteRadio, relativeRadio, algoLinkRadio;
    std::unique_ptr<ButtonAttachment> unlikedThresholdsAttachment, absoluteAttachment, relativeAttachment, algoLinkAttachment;
    
    enum RadioButtons
    {
        LinkedButtons = 0111
    };
    
    void updateToggleState( juce::Button* btn, juce::String name )
    {
        auto state = btn->getToggleState();
        juce::String stateString = state ? "ON" : "OFF";
        DBG( name + " changed to " + stateString );
    }
    
    juce::Label posAlgoLabel{ {}, "Positive Algo" };
    juce::ComboBox posAlgoMenu;
    
    void posAlgoChanged()
    {
        switch( posAlgoMenu.getSelectedId() )
        {
            case 1: DBG("HARD"); break;
            case 2: DBG("TANH"); break;
            default: break;
        }
    }
    
    juce::Label negAlgoLabel{ {}, "Negative Algo" };
    juce::ComboBox negAlgoMenu;
    
    void negAlgoChanged()
    {
        switch( negAlgoMenu.getSelectedId() )
        {
            case 1: DBG("Neg HARD"); break;
            case 2: DBG("Neg TANH"); break;
            default: break;
        }
    }
    
//================================================================================================
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    juce::Rectangle<int> thresholdBackgroundArea;
    
    CosmicSlider posThreshSlider;
    std::unique_ptr<SliderAttachment> posThreshAttachment;
    
    CosmicSlider negThreshSlider;
    std::unique_ptr<SliderAttachment> negThreshAttachment;
    
//================================================================================================
    
    juce::Rectangle<int> algoControlArea;
    juce::Rectangle<int> radioButtonArea;
    juce::Rectangle<int> comboBoxArea;
    
//================================================================================================
    
    juce::Rectangle<int> meterPanel;
    
    juce::AudioBuffer<float> inputGraphicsBuffer;
    juce::AudioBuffer<float> outputGraphicsBuffer;
    
    Meter inputMeter, outputMeter;
    DB_Scale dbScale;
    
//================================================================================================
    
    juce::Rectangle<int> controlPanel;
    
    CosmicKnob gainKnob{ "GAIN" };
    std::unique_ptr<SliderAttachment> gainKnobAttachment;

//================================================================================================
        

    
//================================================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( CosmicClipperAudioProcessorEditor )
};





