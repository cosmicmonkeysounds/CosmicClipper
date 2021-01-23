/*
  ==============================================================================

    ScopeComponent.h
    Created: 23 Jan 2021 1:14:01pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <array>


template <typename SampleType>
class AudioBufferQueue
{
public:

    static constexpr size_t order = 9;
    static constexpr size_t bufferSize = 1U << order;
    static constexpr size_t numBuffers = 5;
    
    void push(const SampleType* dataToPush, size_t numSamples)
    {

        jassert(numSamples <= bufferSize);

        int start1, size1, start2, size2;

        abstractFifo.prepareToWrite(1, start1, size1, start2, size2);

        jassert(size1 <= 1);
        jassert(size2 == 0);

        if (size1 > 0)
        {
            juce::FloatVectorOperations::copy( buffers[(size_t)start1].data(), dataToPush,
                                               (int)juce::jmin(bufferSize, numSamples) );
        }

        abstractFifo.finishedWrite(size1);
    }


    void pop(SampleType* outputBuffer)
    {

        int start1, size1, start2, size2;

        abstractFifo.prepareToRead(1, start1, size1, start2, size2);

        jassert(size1 <= 1);
        jassert(size2 == 0);

        if (size1 > 0)
        {
            juce::FloatVectorOperations::copy( outputBuffer,
                                               buffers[(size_t)start1].data(),
                                               (int)bufferSize );
        }

        abstractFifo.finishedRead(size1);
    }

private:

    std::array <std::array<SampleType, bufferSize>, numBuffers> buffers;
    juce::AbstractFifo abstractFifo{ numBuffers };

};


template<typename SampleType>
class ScopeDataCollector
{
public:

    ScopeDataCollector(AudioBufferQueue<SampleType>& queueToUse) : audioBufferQueue(queueToUse)
    {}

    void process( const SampleType* data, size_t numSamples )
    {
        size_t index = 0;

        if( currentState == State::WaitingForTrigger )
        {
            while( index++ < numSamples )
            {
                auto currentSample = *data++;

                if( currentSample >= triggerLevel && prevSample < triggerLevel )
                {
                    numCollected = 0;
                    currentState = State::Collecting;
                    break;
                }
                prevSample = currentSample;

            }
        }

        if( currentState == State::Collecting )
        {
            while (index++ < numSamples)
            {
                buffer[numCollected++] = *data++;

                if (numCollected == buffer.size())
                {
                    audioBufferQueue.push(buffer.data(), buffer.size());
                    currentState = State::WaitingForTrigger;
                    prevSample = SampleType(100);
                    break;
                }
            }
        }

    }    // >> void process(const SampleType* data, size_t numSamples)

private:

    enum class State
    {
        WaitingForTrigger,
        Collecting
    };

    static constexpr auto triggerLevel = SampleType(0.001);

    std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer;
    State currentState{ State::WaitingForTrigger };
    AudioBufferQueue<SampleType>& audioBufferQueue;
    size_t numCollected;
    SampleType prevSample = SampleType(100);
    
};


template<typename SampleType>
class ScopeComponent : public juce::Component, private juce::Timer
{
public:

    using Queue = AudioBufferQueue<SampleType>;

    ScopeComponent(Queue& queueuToUse) : audioBufferQueue(queueuToUse)
    {
        sampleData.fill(SampleType(0));
        setFramePerSecond(30);
    }

    void setFramePerSecond(int framePerSecond)
    {
        jassert(framePerSecond > 0 && framePerSecond < 1000);
        startTimerHz(framePerSecond);
    }

    void paint(juce::Graphics& g) override
    {

        juce::Rectangle<int> drawArea = getLocalBounds();

        // Oxford Blue
        g.setColour( juce::Colour(12, 27, 51) );
        g.fillRect(drawArea);

        SampleType drawX = (SampleType)drawArea.getX();
        SampleType drawY = (SampleType)drawArea.getY();
        SampleType drawH = (SampleType)drawArea.getHeight();
        SampleType drawW = (SampleType)drawArea.getWidth();
        juce::Rectangle<SampleType> scopeRect = juce::Rectangle<SampleType>{ drawX, drawY, drawW, drawH };

        // Cotton Candy
        g.setColour( juce::Colour(255, 190, 230) );

        plot( sampleData.data(), sampleData.size(),
              g, scopeRect,
              SampleType(0.4), scopeRect.getHeight() / 2 );
        
    }

    void resized() override {}

private:

    void timerCallback() override
    {
        audioBufferQueue.pop(sampleData.data());
        repaint();
    }


    static void plot( const SampleType* data, size_t numSamples,
                      juce::Graphics& g, juce::Rectangle<SampleType> rect,
                      SampleType scaler = SampleType(1), SampleType offset = SampleType(0) )
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        auto right = rect.getRight();
        auto alignedCentre = rect.getBottom() - offset;
        auto gain = h * scaler;

        for( size_t i = 1; i < numSamples; ++i )
        {

            const float x1 = juce::jmap( SampleType(i - 1),
                                         SampleType(0), SampleType(numSamples - 1),
                                         SampleType(right - w), SampleType(right) );
            
            const float y1 = alignedCentre - gain
                                           * juce::jmax( SampleType(-1.0),
                                                         juce::jmin(SampleType(1.0), data[i - 1]) );
            
            const float x2 = juce::jmap( SampleType(i),
                                         SampleType(0), SampleType(numSamples - 1),
                                         SampleType(right - w), SampleType(right) );
            
            const float y2 = alignedCentre - gain
                                           * juce::jmax( SampleType(-1.0),
                                                         juce::jmin(SampleType(1.0), data[i]) );
            
            g.drawLine( x1, y1, x2, y2, 3.f );
        }
    }

    Queue& audioBufferQueue;
    std::array<SampleType, Queue::bufferSize> sampleData;          

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeComponent)
};
