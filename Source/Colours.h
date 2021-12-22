#pragma once
#include <stdlib.h>
#include <JuceHeader.h>



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


struct CustomColours
{
    
    const static juce::Colour LightBlue()  { return juce::Colour {197, 198, 255}; }
    const static juce::Colour MediumBlue() { return juce::Colour {12,  27,  64};  }
    const static juce::Colour DarkBlue()   { return juce::Colour {7,   14,  31};  }
    
    const static juce::Colour LightPink()  { return juce::Colour {255, 190, 230}; }
    const static juce::Colour MediumPink() { return juce::Colour {255, 109, 189}; }
    const static juce::Colour DarkPink()   { return juce::Colour {129, 55,  100}; }

};




struct MyColours
{

    std::vector<juce::Colour> myColours
    {

        // BLUE_DARK, Rich Black FOGRA 29
        juce::Colour {7, 14, 31},
        
        // BLUE_MID, Oxford Blue
        juce::Colour {12, 27, 64},

        // BLUE_LIGHT, Lavender Blue
        juce::Colour {197, 198, 255},
        
        // BLUE_NEON, Sky Blue Crayola
        juce::Colour {87, 213, 231},
        
        // PINK_DARK, Twighlight Lavender
        juce::Colour {129, 55, 100},
        
        // PINK_MID, Hot Pink
        juce::Colour {255, 109, 189},
        
        // PINK_LIGHT, Cotton Candy
        juce::Colour {255, 190, 230},
        
        // PINK_NEON, Paradise Pink
        juce::Colour {238, 66, 102}
        
    };
    
    

    juce::Colour operator[] (Colours c)
    {
        return myColours[c];
    }


    
};
