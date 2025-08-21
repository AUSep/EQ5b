/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQ5bAudioProcessorEditor::EQ5bAudioProcessorEditor (EQ5bAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    hpFreqSliderAttachment(audioProcessor.processorParameters,"hpFreq", hpFreqSlider),
    hpSlopeSliderAttachment(audioProcessor.processorParameters,"hpSlope", hpSlopeSlider),
    lpFreqSliderAttachment(audioProcessor.processorParameters,"lpFreq",lpFreqSlider),
    lpSlopeSliderAttachment(audioProcessor.processorParameters, "lpSlope", lpSlopeSlider),
    p1FreqSliderAttachment(audioProcessor.processorParameters,"peakFreq1", p1FreqSlider),
    p1GainSliderAttachment(audioProcessor.processorParameters, "peakGain1", p1GainSlider),
    p1QSliderAttachment(audioProcessor.processorParameters, "peakQ1", p1QSlider),
    p2FreqSliderAttachment(audioProcessor.processorParameters,"peakFreq2", p2FreqSlider),
    p2GainSliderAttachment(audioProcessor.processorParameters, "peakGain2", p2GainSlider),
    p2QSliderAttachment(audioProcessor.processorParameters, "peakQ2", p2QSlider),
    p3FreqSliderAttachment(audioProcessor.processorParameters,"peakFreq3", p3FreqSlider),
    p3GainSliderAttachment(audioProcessor.processorParameters, "peakGain3", p3GainSlider),
    p3QSliderAttachment(audioProcessor.processorParameters, "peakQ3", p3QSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps() )
    {
      addAndMakeVisible(comp); 
    }
    setSize (1200, 400);
}

EQ5bAudioProcessorEditor::~EQ5bAudioProcessorEditor()
{
}

//==============================================================================
void EQ5bAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void EQ5bAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight()*0.33);

    auto hpArea = bounds.removeFromLeft(bounds.getWidth()*0.2);
    auto lpArea = bounds.removeFromRight(bounds.getWidth()*0.25);
    auto p1Area = bounds.removeFromLeft(bounds.getWidth()*0.33);
    auto p3Area = bounds.removeFromRight(bounds.getWidth()*0.5);
    
    hpFreqSlider.setBounds(hpArea.removeFromTop(hpArea.getHeight()*0.5));
    hpSlopeSlider.setBounds(hpArea);

    p1FreqSlider.setBounds(p1Area.removeFromTop(p1Area.getHeight()*0.33));
    p1GainSlider.setBounds(p1Area.removeFromTop(p1Area.getHeight()*0.5));
    p1QSlider.setBounds(p1Area);

    p2FreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.33));
    p2GainSlider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.5));
    p2QSlider.setBounds(bounds);

    p3FreqSlider.setBounds(p3Area.removeFromTop(p3Area.getHeight()*0.33));
    p3GainSlider.setBounds(p3Area.removeFromTop(p3Area.getHeight()*0.5));
    p3QSlider.setBounds(p3Area);
            
    lpFreqSlider.setBounds(lpArea.removeFromTop(lpArea.getHeight()*0.5));
    lpSlopeSlider.setBounds(lpArea);
}

std::vector<juce::Component*> EQ5bAudioProcessorEditor::getComps()
{
  return
  {
    &hpFreqSlider,
    &hpSlopeSlider,
    &p1GainSlider,
    &p1FreqSlider,
    &p1QSlider,
    &p2GainSlider,
    &p2FreqSlider,
    &p2QSlider,
    &p3GainSlider,
    &p3FreqSlider,
    &p3QSlider,
    &lpFreqSlider,
    &lpSlopeSlider
  };
}
