/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQ5bAudioProcessorEditor::EQ5bAudioProcessorEditor (EQ5bAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps() )
    {
      addAndMakeVisible(comp); 
    }
    setSize (600, 400);
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

    auto hpArea = bounds.removeFromLeft(bounds.getWidth()*0.33);
    auto lpArea = bounds.removeFromRight(bounds.getWidth()*0.5);
    
    hpFreqSlider.setBounds(hpArea);
    lpFreqSlider.setBounds(lpArea);

    p1FreqSLider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.33));
    p1GainSlider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.5));
    p1QSlider.setBounds(bounds);
}

std::vector<juce::Component*> EQ5bAudioProcessorEditor::getComps()
{
  return
  {
    &hpFreqSlider,
    &p1GainSlider,
    &p1FreqSLider,
    &p1QSlider,
    &p2GainSlider,
    &p2FreqSLider,
    &p2QSlider,
    &p3GainSlider,
    &p3FreqSLider,
    &p3QSlider,
    &lpFreqSlider
  };
}
