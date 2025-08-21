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
    hpSlopeSlider,
    p1GainSlider,
    p1FreqSlider,
    p1QSlider,
    p2GainSlider,
    p2FreqSlider,
    p2QSlider,
    p3GainSlider,
    p3FreqSlider,
    p3QSlider,
    lpFreqSlider,
    lpSlopeSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    Attachment hpFreqSliderAttachment,
    hpSlopeSliderAttachment,
    p1GainSliderAttachment,
    p1FreqSliderAttachment,
    p1QSliderAttachment,
    p2GainSliderAttachment,
    p2FreqSliderAttachment,
    p2QSliderAttachment,
    p3GainSliderAttachment,
    p3FreqSliderAttachment,
    p3QSliderAttachment,
    lpFreqSliderAttachment,
    lpSlopeSliderAttachment;

    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQ5bAudioProcessorEditor)
};
