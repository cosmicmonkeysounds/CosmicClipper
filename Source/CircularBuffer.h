/*
  ==============================================================================

    CircularBuffer.h
    Created: 6 Feb 2021 12:12:39am
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

template <typename T>
struct CircularBuffer
{
    using DataType = std::vector<T>;
    
    CircularBuffer( std::size_t newSize, T fillValue )
    {
        resize( newSize, fillValue );
    }
    
    void resize( std::size_t newSize, T fillValue )
    {
        dataHolder.resize( newSize );
        clear( fillValue );
    }
    
    void clear( T fillValue )
    {
        dataHolder.assign( getSize(), fillValue );
        writeIndex.store(0);
    }
    
    void write( T itemToAdd )
    {
        std::size_t writeInd = writeIndex.load();
        dataHolder[writeInd] = itemToAdd;
        
        if( ++writeInd > getSize() - 1 )
            writeInd = 0;
        
        writeIndex.store( writeInd );
    }
    
    DataType& getData() { return dataHolder; }
    std::size_t getSize() const { return dataHolder.size(); }
    std::size_t getReadIndex() const { return writeIndex.load(); }
    
private:
    DataType dataHolder;
    std::atomic<std::size_t> writeIndex;
};
