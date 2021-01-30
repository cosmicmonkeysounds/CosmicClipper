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
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), // end of AudioProcessor initialization

        parametersTreeState( *this, nullptr, juce::Identifier("CosmicClipper"), //createParameters() )
                   {
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "positive threshold", // parameter ID
                            "Positive Threshold", // parameter name
                            0.f,                  // min value
                            1.f,                  // max value
                            1.f                   // default value
                        ),

                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "negative threshold",
                            "Negative Threshold",
                            0.f,
                            1.f,
                            1.f
                        ),

                        std::make_unique<juce::AudioParameterBool>
                        (
                            "unlinked thresholds",
                            "Unlinked Thresholds",
                            true
                        ),
            
                        std::make_unique<juce::AudioParameterBool>
                        (
                            "absolute",
                            "Absolute",
                            false
                        ),
        
                         std::make_unique<juce::AudioParameterBool>
                         (
                             "relative",
                             "Relative",
                             false
                         ),
            
                        std::make_unique<juce::AudioParameterBool>
                        (
                            "link algorithms",
                            "Link Algorithms",
                            true
                        ),
            
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "positive algorithm modifier",
                            "Positive Algorithm Modifier",
                            0.f, 1.f, 0.f
                        ),
            
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "negative algorithm modifier",
                            "Negative Algorithm Modifier",
                            0.f, 1.f, 0.f
                        ),
                        
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "positive algorithm",
                            "Positive Algorithm",
                            juce::NormalisableRange<float>( 0.f, 1.f, 1.f ),
                            0.f
                        ),
            
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "negative algorithm",
                            "Negative Algorithm",
                            juce::NormalisableRange<float>( 0.f, 1.f, 1.f ),
                            0.f
                        ),
            
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "gain",
                            "Gain",
                            1.f,
                            10.f,
                            1.f
                        ),
            
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "input level",
                            "Input Level",
                            0.f,
                            10.f,
                            1.f
                        ),
            
                        std::make_unique<juce::AudioParameterFloat>
                        (
                            "output level",
                            "Output Level",
                            0.f,
                            10.f,
                            1.f
                        )
            
                   }) // end of parameter (AudioValueTree) initialization
#endif
                // for oscilloscope
            , scopeDataCollector( scopeDataQueue )
{
    posThreshParam       = parametersTreeState.getRawParameterValue( "positive threshold" );
    negThreshParam       = parametersTreeState.getRawParameterValue( "negative threshold" );
    
    linkThreshParam      = parametersTreeState.getRawParameterValue( "unlinked thresholds" );
    absoluteParam        = parametersTreeState.getRawParameterValue( "absolute" );
    relativeParam        = parametersTreeState.getRawParameterValue( "relative" );
    algoLinkParam        = parametersTreeState.getRawParameterValue( "link algorithms" );
    
    inputLevelParam      = parametersTreeState.getRawParameterValue( "input level" );
    outputLevelParam     = parametersTreeState.getRawParameterValue( "output level" );
    gainParam            = parametersTreeState.getRawParameterValue( "gain" );
    
    posAlgoModifierParam = parametersTreeState.getRawParameterValue( "positive algorithm modifier" );
    negAlgoModifierParam = parametersTreeState.getRawParameterValue( "negative algorithm modifier" );
    
    posAlgoParam         = parametersTreeState.getRawParameterValue( "positive algorithm" );
    negAlgoParam         = parametersTreeState.getRawParameterValue( "negative algorithm" );
    
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

    testOscillator.prepare({ sampleRate,
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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int numSamples = buffer.getNumSamples();
    
#if SINE_TEST == 1
    testOscillator.setFrequency( 60.f );
    
    auto* leftBuffer  = buffer.getWritePointer(0);
    auto* rightBuffer = buffer.getWritePointer(1);
    
    for( int sample = 0; sample < numSamples; ++sample )
    {
        auto currentSample = (float) std::sin(currentAngle);
        currentAngle += angleDelta;
        rightBuffer[sample] = leftBuffer[sample] = currentSample;
    }
#endif
    
    buffer.applyGain( currInputLevel );
    inputFifo.push( buffer );
    buffer.applyGain( currGain );
    
    posClippingType = (ClippingTypes) posAlgoParam->load();
    setPosAlgo(posClippingType);
    
    negClippingType = (ClippingTypes) negAlgoParam->load();
    setNegAlgo(negClippingType);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        for( int samplePos = 0; samplePos < numSamples; ++samplePos )
        {
            currSampleRatio = samplePos / numSamples;
            

            rampParameter( *posThreshParam, currPosThresh, prevPosThresh );
            rampParameter( -(*negThreshParam), currNegThresh, prevNegThresh );
            rampParameter( *inputLevelParam, currInputLevel, prevInputLevel );
            rampParameter( *outputLevelParam, currOutputLevel, prevOutputLevel );
            rampParameter( *gainParam, currGain, prevGain );
            
            //=======================================================================================
            // Transfer Function gets applied to sample
            
            float& sample = channelData[samplePos];
            
            if( sample >= 0.f )
                posAlgo( sample );
            
            else
                negAlgo( sample );
            //transferFuncs[ClippingTypes::HardClipping]( sample );
            
            //=======================================================================================
            
        } // end of sample for loop
    } // end of channel for loop
    
    scopeFifo.push( buffer );
    scopeDataCollector.process( buffer.getReadPointer(0), (size_t)buffer.getNumSamples() );
    
    buffer.applyGain( currOutputLevel );
    outputFifo.push( buffer );
    
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
    auto state = parametersTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml( state.createXml() );
    copyXmlToBinary( *xml, destData );
}

void CosmicClipperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState( getXmlFromBinary(data, sizeInBytes) );
    
    if( xmlState.get() != nullptr )
    {
        if( xmlState->hasTagName(parametersTreeState.state.getType()) )
        {
            parametersTreeState.replaceState( juce::ValueTree::fromXml(*xmlState) );
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
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
