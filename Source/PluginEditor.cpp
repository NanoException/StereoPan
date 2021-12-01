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

    addAndMakeVisible(rotationSlider);
    rotationAttachment.reset(new SliderAttachment(valueTreeState, "rotation", rotationSlider));

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

    rotationSlider.setBounds(10, 40, 200, 30);
}
