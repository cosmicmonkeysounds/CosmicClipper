/*
  ==============================================================================

    LevelMeter.h
    Created: 25 Jan 2021 5:52:36pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

#define NEGATIVE_INFINITY_DB -66.f
#define MAX_DB                12.f


//==============================================================================


struct Tick
{
    int y{0};
    float db{0.f};
};


//==============================================================================


class Meter : public juce::Component
{
public:
    Meter() = default;
    ~Meter() override = default;

    void paint( juce::Graphics& g ) override
    {
        g.fillAll( bgColour );
        
        auto bounds = this->getLocalBounds();
        float heightOfWindow = bounds.getHeight();
        
        auto levelMappedHeight = juce::jmap( currentLevel,
                                             NEGATIVE_INFINITY_DB, MAX_DB,
                                             heightOfWindow, 0.f );
        
        g.setColour( meterColour );
        g.fillRect( bounds.withHeight(heightOfWindow)
                          .withY(levelMappedHeight) );
        
    }
    
    void resized() override
    {
        ticks.clear();
        
        int meterBoxHeight = getHeight();
        float y = NEGATIVE_INFINITY_DB;
        
        for( int i = 0; i <= (int)numberOfSteps; ++i )
        {
            Tick tick;
            tick.db = y;
            tick.y = juce::jmap( (int)y,
                                 (int)NEGATIVE_INFINITY_DB, (int)MAX_DB,
                                 meterBoxHeight, 0 );
            
            ticks.push_back( tick );
            y += dbStepSize;
        }
    }
    
    void update( float newLevel )
    {
        currentLevel = newLevel;
        repaint();
    }
    
    std::vector<Tick> ticks;
    
    Meter& withBackgroundColour( juce::Colour bg )
    {
        bgColour = bg;
        return *this;
    }
    
    Meter& withMeterColour( juce::Colour mc )
    {
        meterColour = mc;
        return *this;
    }

private:
    
    juce::Colour bgColour, meterColour;
    float currentLevel{0.f};
    
    const float dbStepSize = 6.f;
    const float numberOfSteps = (MAX_DB - NEGATIVE_INFINITY_DB) / dbStepSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Meter )
};


//==============================================================================


class DB_Scale : public juce::Component
{
public:
    DB_Scale() = default;
    ~DB_Scale() override = default;
    
    void paint( juce::Graphics& g ) override
    {
        g.setColour( juce::Colours::white );
        
        for( auto tick : ticks )
            g.drawSingleLineText( juce::String(tick.db), 5, tick.y + yOffset );
    }
    
    void resized() override {}
    
    std::vector<Tick> ticks;
    int yOffset{0};
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( DB_Scale )
};
