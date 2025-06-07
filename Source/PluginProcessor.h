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
  struct HPfilter{
    float fCut{0};
    Slope slope{Slope::slope_12};
  };
  HPfilter hpFilter;
  struct LPfilter{
    float fCut{0};
    Slope slope{Slope::slope_12};
  };
  LPfilter lpFilter;
  struct LowPeak{
    float gain{0}, freq{0}, q{0};
  };
  LowPeak lowPeak;
  struct MidPeak{
    float gain{0}, freq{0}, q{0};
  };
  MidPeak midPeak;
  struct HighPeak{
    float gain{0}, freq{0}, q{0};
  };
  HighPeak highPeak;
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
    using MonoChain = juce::dsp::ProcessorChain<CutFreq, Filter, Filter, Filter, CutFreq>;
    MonoChain leftChain, rightChain;

    enum ChainPositions{
    HiPass,
    LoPeak,
    MidPeak,
    HiPeak,
    LoPass
    };
    
    template<typename PeakFilterSettings>
    void updatePeakFilters(const PeakFilterSettings& settings,
                          ChainPositions whichPeak)
    {
      auto peakFilterCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                        settings.freq,
                                                                                        settings.q,
                                                                                        juce::Decibels::decibelsToGain(settings.gain));
      
      switch (whichPeak)
      {
      case ChainPositions::LoPeak:
        {
          updateCoefficients(leftChain.get<ChainPositions::LoPeak>().coefficients, peakFilterCoefficients);
          updateCoefficients(rightChain.get<ChainPositions::LoPeak>().coefficients, peakFilterCoefficients);
        }
        break;
      case ChainPositions::MidPeak:
        {
          updateCoefficients(leftChain.get<ChainPositions::MidPeak>().coefficients, peakFilterCoefficients);
          updateCoefficients(rightChain.get<ChainPositions::MidPeak>().coefficients, peakFilterCoefficients);
        }
      case ChainPositions::HiPeak:
        {
          updateCoefficients(leftChain.get<ChainPositions::HiPeak>().coefficients, peakFilterCoefficients);
          updateCoefficients(rightChain.get<ChainPositions::HiPeak>().coefficients, peakFilterCoefficients);
        }
        break;
      }
    };

    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients (Coefficients& oldCoeff, Coefficients& newCoeff);

    template<typename ChainType, typename CoefficientType>
    void updateEndFiltersParameters(ChainType& channelHpf,
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
