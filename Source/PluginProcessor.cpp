/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JucePluginDefines.h>

//==============================================================================
EQ5bAudioProcessor::EQ5bAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

EQ5bAudioProcessor::~EQ5bAudioProcessor()
{
}

//==============================================================================
void EQ5bAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(processorParameters);

    auto hpFilterCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.cutf,
                                                                                                    sampleRate,
                                                                                                    2*(chainSettings.slope)+1);
    auto& leftHP = leftChain.get<ChainPositions::HiPass>();
    auto& rightHP = rightChain.get<ChainPositions::HiPass>();

    leftHP.setBypassed<0>(true);
    leftHP.setBypassed<1>(true);
    leftHP.setBypassed<2>(true);
    leftHP.setBypassed<3>(true);

    switch ( chainSettings.slope )
    {
    case slope_12:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        break;
        }
    
    case slope_24:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        *leftHP.get<1>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<1>(false);
        break;
        }
    
    case slope_32:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        *leftHP.get<1>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<1>(false);
        *leftHP.get<3>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<3>(false);
        break;
        }
    case slope_48:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        *leftHP.get<1>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<1>(false);
        *leftHP.get<3>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<3>(false);
        *leftHP.get<4>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<4>(false);
        break;
        }

    }
}
    
const juce::String EQ5bAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EQ5bAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EQ5bAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EQ5bAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EQ5bAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EQ5bAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EQ5bAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EQ5bAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EQ5bAudioProcessor::getProgramName (int index)
{
    return {};
}

void EQ5bAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void EQ5bAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EQ5bAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void EQ5bAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    auto chainSettings = getChainSettings(processorParameters);

    auto hpFilterCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.cutf,
                                                                                                    getSampleRate(),
                                                                                                    2*(chainSettings.slope)+1);
    auto& leftHP = leftChain.get<ChainPositions::HiPass>();
    auto& rightHP = rightChain.get<ChainPositions::HiPass>();

    leftHP.setBypassed<0>(true);
    leftHP.setBypassed<1>(true);
    leftHP.setBypassed<2>(true);
    leftHP.setBypassed<3>(true);

    switch ( chainSettings.slope )
    {
    case slope_12:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        break;
        }
    
    case slope_24:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        *leftHP.get<1>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<1>(false);
        break;
        }
    
    case slope_32:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        *leftHP.get<1>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<1>(false);
        *leftHP.get<3>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<3>(false);
        break;
        }
    case slope_48:{
        *leftHP.get<0>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<0>(false);
        *leftHP.get<1>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<1>(false);
        *leftHP.get<3>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<3>(false);
        *leftHP.get<4>().coefficients = *hpFilterCoefficients[0];
        leftHP.setBypassed<4>(false);
        break;
        }

    }

}

//==============================================================================
bool EQ5bAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EQ5bAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void EQ5bAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EQ5bAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& processorParameters)
{
    ChainSettings settings;
    settings.cutf = processorParameters.getRawParameterValue("HP freq")->load();
    settings.slope = static_cast<Slope>(processorParameters.getRawParameterValue("HP slope")->load());
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout EQ5bAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    //High Pass Filter Parameters:

    layout.add(std::make_unique<juce::AudioParameterFloat>("HP freq", 
                                                    "HP cut frequency", 
                                                    juce::NormalisableRange<float>(20.f,300.f,1.f,3.f),
                                                    20.f));
    juce::StringArray strArray;
    for( int i = 0; i < 4; ++i ){
        juce::String str;
        str << (12+12*i);
        str << " db/Oct";
        strArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("HP slope", "HP Slope", strArray, 0));
    //
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQ5bAudioProcessor();
}
