/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CosmicClipperAudioProcessor::CosmicClipperAudioProcessor()

#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (     BusesProperties()
                            #if ! JucePlugin_IsMidiEffect
                                #if ! JucePlugin_IsSynth
                                .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                #endif
                            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                            #endif
                       )
#endif
        
{

    
#if SINE_TEST == 1
    const std::function<float(float)> sineFunc = [](float deg){ return std::sin(deg); };
    testOscillator.initialise( sineFunc, 256 );
#endif
    
}

CosmicClipperAudioProcessor::~CosmicClipperAudioProcessor()
{
}

//==============================================================================
const juce::String CosmicClipperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CosmicClipperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CosmicClipperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CosmicClipperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CosmicClipperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CosmicClipperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CosmicClipperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CosmicClipperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CosmicClipperAudioProcessor::getProgramName (int index)
{
    return {};
}

void CosmicClipperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CosmicClipperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
#if SINE_TEST == 1

    testOscillator.prepare ({ sampleRate,
                             static_cast<juce::uint32>(samplesPerBlock),
                             static_cast<juce::uint32>(getMainBusNumOutputChannels())
                        });
    
    currentSampleRate = sampleRate;
    updateAngleDelta();
    
#endif
}

void CosmicClipperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CosmicClipperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CosmicClipperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels  = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    
    
#if SINE_TEST == 1
    testOscillator.setFrequency (60.f);
    
    auto* leftBuffer  = buffer.getWritePointer (0);
    auto* rightBuffer = buffer.getWritePointer (1);
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float currentSample = (float) std::sin (currentAngle);
        currentAngle += angleDelta;
        rightBuffer[sample] = leftBuffer[sample] = currentSample;
    }
#endif
    
    
    
    
    
#if SINE_TEST == 1
    buffer.clear();
#endif
    
} // end of process block

//==============================================================================
bool CosmicClipperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CosmicClipperAudioProcessor::createEditor()
{
    return new CosmicClipperAudioProcessorEditor( *this );
}

//==============================================================================
void CosmicClipperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
//    auto state = parametersTreeState.copyState();
//    std::unique_ptr<juce::XmlElement> xml( state.createXml() );
//    copyXmlToBinary( *xml, destData );
}

void CosmicClipperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
//    std::unique_ptr<juce::XmlElement> xmlState( getXmlFromBinary(data, sizeInBytes) );
//
//    if( xmlState.get() != nullptr )
//    {
//        if( xmlState->hasTagName(parametersTreeState.state.getType()) )
//        {
//            parametersTreeState.replaceState( juce::ValueTree::fromXml(*xmlState) );
//        }
//    }
}



//==============================================================================
// This creates new instances of the plugin..
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CosmicClipperAudioProcessor();
}




//juce::AudioProcessorValueTreeState::ParameterLayout CosmicClipperAudioProcessor::createParameters()
//{
//    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
//    
//    params.emplace_back( std::make_unique<juce::AudioParameterFloat>(
//                                                                     "positive threshold",
//                                                                     "Positive Threshold",
//                                                                     0.f, 1.f, 1.f
//                                                                     ));
//    
//    params.emplace_back( std::make_unique<juce::AudioParameterFloat>(
//                                                                     "negative threshold",
//                                                                     "Negative Threshold",
//                                                                     -1.f, 0.f, -1.f
//                                                                     ));
//    
//    params.emplace_back( std::make_unique<juce::AudioParameterBool>(
//                                                                     "link thresholds",
//                                                                     "Link Threshold",
//                                                                     true
//                                                                     ));
//    
//    return { params.begin(), params.end() };
//}
