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
  slope_36,
  slope_48
};

struct ChainSettings{
  struct CutFilter{
    float cutf{0};
    Slope slope{slope_12};
  };
  CutFilter lpFilter, hpFilter;
  struct PeakFilter{
    float gain{0}, q{0}, freq{0};
  };
  PeakFilter loPeak, midPeak, hiPeak;
};

  using Filter = juce::dsp::IIR::Filter<float>;
  using CutFreq = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
  using MonoChain = juce::dsp::ProcessorChain<CutFreq, Filter, Filter, Filter, CutFreq>;
  MonoChain leftChain, rightChain;

  enum ChainPositions{
  HiPass,
  LoPeak,
  MidPeak,
  HiPeak,
  LoPass
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
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients (Coefficients& oldCoeff, const Coefficients& newCoeff);

    template<int Index, typename ChainType, typename CoeffincientType>
    void update(ChainType& chain, const CoeffincientType& coefficients)
    {
      updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
      chain.template setBypassed<Index>(false);  
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFiltersSlope(ChainType& chain,
                        const CoefficientType& cutCoefficients,
                        const Slope& slope)
    {
      chain.template setBypassed<0>(true);
      chain.template setBypassed<1>(true);
      chain.template setBypassed<2>(true);
      chain.template setBypassed<3>(true);

      switch ( slope )
      {
      case slope_48:
        {
          update<3>(chain, cutCoefficients);
        }
      case slope_36:
        {
          update<2>(chain, cutCoefficients);
        }
      case slope_24:
        {
          update<1>(chain, cutCoefficients);
        }
      case slope_12:
        {
          update<0>(chain, cutCoefficients);
        }
      }
    }

    void updatePeakFilters(int position, const ChainSettings::PeakFilter& filter);
    void updateCutFilters(int position, const ChainSettings::CutFilter& filter);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ5bAudioProcessor)
};
