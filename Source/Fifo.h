/*
  ==============================================================================

    Fifo.h
    Created: 21 Jan 2021 3:58:31pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

template<typename T>
struct Fifo
{
    
    void prepare( int numberOfChannels, int numberOfSamples )
    {
        for( auto& buffer : buffers )
            buffer.setSize( numberOfChannels, numberOfSamples );
    }
    
    bool push( const T& itemToAdd )
    {
        auto scopedWriter = fifo.write(1);
        
        if( scopedWriter.blockSize1 >= 1 )
        {
            auto& buffer = buffers[scopedWriter.startIndex1];
            buffer = itemToAdd;
            return true;
        }
        
        return false;
    }
    
    bool pull( T& itemToPull )
    {
        auto scopedReader = fifo.read(1);
        
        if( scopedReader.blockSize1 >= 1 )
        {
            auto& buffer = buffers[scopedReader.startIndex1];
            itemToPull = buffer;
            return true;
        }
        
        return false;
    }
    
private:
    
    static constexpr int Size = 5;
    std::array<T, Size> buffers;
    juce::AbstractFifo fifo{Size};
    
};
