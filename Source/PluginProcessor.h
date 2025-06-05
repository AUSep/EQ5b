/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope {
  slope_12,
  slope_24,
  slope_32,
  slope_48
};

struct ChainSettings{
  
  float hpCutf{0}, lpCutf{0}; 
  Slope hpSlope{Slope::slope_12}, lpSlope{Slope::slope_12};
};

ChainSettings getChainSettings (juce::AudioProcessorValueTreeState& processorParameters);
//==============================================================================
/**
*/
class EQ5bAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EQ5bAudioProcessor();
    ~EQ5bAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState processorParameters{*this, nullptr, "Parameters", createParameterLayout()};
private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFreq = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFreq>;
    MonoChain leftChain, rightChain;

    enum ChainPositions{
    HiPass,
    LoPass
    };

    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients (Coefficients& oldCoeff, Coefficients& newCoeff);

    template<typename ChainType, typename CoefficientType>
    void updateFilterParameters(ChainType& channelHpf,
                        const CoefficientType& cutCoefficients,
                        const Slope& hpSlope)
    {
      channelHpf.template setBypassed<0>(true);
      channelHpf.template setBypassed<1>(true);
      channelHpf.template setBypassed<2>(true);
      channelHpf.template setBypassed<3>(true);

      switch ( hpSlope )
      {
      case slope_12:{
          *channelHpf.template get<0>().coefficients = *cutCoefficients[0]; 
          channelHpf.template setBypassed<0>(false);
          break;
          }
      
      case slope_24:{
          *channelHpf.template get<0>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<0>(false);
          *channelHpf.template get<1>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<1>(false);
          break;
          }
      
      case slope_32:{
          *channelHpf.template get<0>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<0>(false);
          *channelHpf.template get<1>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<1>(false);
          *channelHpf.template get<2>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<2>(false);
          break;
          }
      case slope_48:{
          *channelHpf.template get<0>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<0>(false);
          *channelHpf.template get<1>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<1>(false);
          *channelHpf.template get<2>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<2>(false);
          *channelHpf.template get<3>().coefficients = *cutCoefficients[0];
          channelHpf.template setBypassed<3>(false);
          break;
          }

      }
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ5bAudioProcessor)
};
