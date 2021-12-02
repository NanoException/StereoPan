/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StereoPanAudioProcessorEditor::StereoPanAudioProcessorEditor (StereoPanAudioProcessor& p, juce::AudioProcessorValueTreeState & vts)
    : AudioProcessorEditor (&p), valueTreeState(vts), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(gainSlider);
    gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);

    addAndMakeVisible(widthBypassButton);
    widthBypassAttachment.reset(new ButtonAttachment(valueTreeState, "widthbypass", widthBypassButton));

    addAndMakeVisible(widthSlider);
    widthAttachment.reset(new SliderAttachment(valueTreeState, "width", widthSlider));

    addAndMakeVisible(rotationBypassButton);
    rotationBypassAttachment.reset(new ButtonAttachment(valueTreeState, "rotationbypass", rotationBypassButton));

    addAndMakeVisible(rotationSlider);
    rotationAttachment.reset(new SliderAttachment(valueTreeState, "rotation", rotationSlider));

    addAndMakeVisible(lpfLinkButton);
    lpfLinkAttachment.reset(new ButtonAttachment(valueTreeState, "lpflink", lpfLinkButton));

    addAndMakeVisible(lpfFreqSlider);
    lpfFreqAttachment.reset(new SliderAttachment(valueTreeState, "lpffreq", lpfFreqSlider));

    setSize (700, 350);
}

StereoPanAudioProcessorEditor::~StereoPanAudioProcessorEditor()
{
}

//==============================================================================
void StereoPanAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void StereoPanAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    gainSlider.setBounds(650, 25, 25, 275);
    widthBypassButton.setBounds(250, 10, 50, 30);
    widthSlider.setBounds(10, 10, 200, 30);
    rotationBypassButton.setBounds(250, 40, 50, 30);
    rotationSlider.setBounds(10, 40, 200, 30);
    lpfFreqSlider.setBounds(10, 70, 200, 30);
    lpfLinkButton.setBounds(10, 100, 50, 30);
}
