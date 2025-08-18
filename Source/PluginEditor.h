/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct rotaryKnob : juce::Slider
{
  rotaryKnob() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                            juce::Slider::TextEntryBoxPosition::NoTextBox)
  {

  }
};

//==============================================================================
/**
*/
class EQ5bAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EQ5bAudioProcessorEditor (EQ5bAudioProcessor&);
    ~EQ5bAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQ5bAudioProcessor& audioProcessor;
    rotaryKnob hpFreqSlider, 
    p1GainSlider,
    p1FreqSLider,
    p1QSlider,
    p2GainSlider,
    p2FreqSLider,
    p2QSlider,
    p3GainSlider,
    p3FreqSLider,
    p3QSlider,
    lpFreqSlider;

    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ5bAudioProcessorEditor)
};
