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

    const auto& params = audioProcessor.getParameters();
    for (auto param : params) 
    {
      param->addListener(this);
    };
    startTimerHz(60);
    setSize (1200, 400);
}

EQ5bAudioProcessorEditor::~EQ5bAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params) 
    {
      param->removeListener(this);
    }
}

//==============================================================================
void EQ5bAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight()*0.33);
    auto w = responseArea.getWidth();

    auto& hpFilter = monoChain.get<ChainPositions::HiPass>();
    auto& peak1 = monoChain.get<ChainPositions::LoPeak>();
    auto& peak2 = monoChain.get<ChainPositions::MidPeak>();
    auto& peak3 = monoChain.get<ChainPositions::HiPeak>();
    auto& lpFilter = monoChain.get<ChainPositions::LoPass>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for ( int i = 0; i < w; ++i)
    {
      double mag = 1.f;
      auto freq = mapToLog10(double(i)/double(w), 20.0, 20000.0);

      if(! hpFilter.isBypassed<0>())
        mag *= hpFilter.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! hpFilter.isBypassed<1>())
        mag *= hpFilter.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! hpFilter.isBypassed<2>())
        mag *= hpFilter.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! hpFilter.isBypassed<3>())
        mag *= hpFilter.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

      if(! monoChain.isBypassed<ChainPositions::LoPeak>())
        mag *= peak1.coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! monoChain.isBypassed<ChainPositions::MidPeak>())
        mag *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! monoChain.isBypassed<ChainPositions::HiPeak>())
        mag *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate);

      if(! lpFilter.isBypassed<0>())
        mag *= lpFilter.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! lpFilter.isBypassed<1>())
        mag *= lpFilter.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! lpFilter.isBypassed<2>())
        mag *= lpFilter.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      if(! lpFilter.isBypassed<3>())
        mag *= lpFilter.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

      mags[i] = Decibels::gainToDecibels(mag);
    }

    Path responseCurve;
    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
      return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); ++i)
    {
      responseCurve.lineTo(responseArea.getX()+i, map(mags[i]));
    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f,1.f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));

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

void EQ5bAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
  parametersChanged.set(true);
}

void EQ5bAudioProcessorEditor::timerCallback()
{
  if(parametersChanged.compareAndSetBool(false,true))
  {
    auto chainSettings = getChainSettings(audioProcessor.processorParameters);
    double sampleRate = audioProcessor.getSampleRate();

    auto hpCoeff = makeHpFilter(chainSettings.hpFilter, sampleRate);
    updateCutFiltersSlope(monoChain.get<ChainPositions::HiPass>(), hpCoeff, chainSettings.hpFilter.slope);
    auto peak1Coeff = makePeakFilter(chainSettings.loPeak, sampleRate);
    updateCoefficients(monoChain.get<ChainPositions::LoPeak>().coefficients, peak1Coeff);
    auto peak2Coeff = makePeakFilter(chainSettings.midPeak, sampleRate);
    updateCoefficients(monoChain.get<ChainPositions::MidPeak>().coefficients, peak2Coeff);
    auto peak3Coeff = makePeakFilter(chainSettings.hiPeak, sampleRate);
    updateCoefficients(monoChain.get<ChainPositions::HiPeak>().coefficients, peak3Coeff);
    auto lpCoeff = makeLpFilter(chainSettings.lpFilter, sampleRate);
    updateCutFiltersSlope(monoChain.get<ChainPositions::LoPass>(), lpCoeff, chainSettings.lpFilter.slope);

    repaint();
  }
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
