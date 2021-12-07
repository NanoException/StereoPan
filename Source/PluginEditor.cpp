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
    addAndMakeVisible(mainTitle);
    mainTitle.setText("LPanner", juce::dontSendNotification);
    mainTitle.setFont(juce::Font(20.0f, juce::Font::bold));
    mainTitle.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(gainTitle);
    gainTitle.setText("Gain", juce::dontSendNotification);
    gainTitle.setFont(juce::Font(16.0f, juce::Font::bold));
    gainTitle.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(gainSlider);
    gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);

    addAndMakeVisible(widthBypassButton);
    widthBypassAttachment.reset(new ButtonAttachment(valueTreeState, "widthbypass", widthBypassButton));

    addAndMakeVisible(widthTitle);
    widthTitle.setText("Width", juce::dontSendNotification);
    widthTitle.setFont(juce::Font(16.0f, juce::Font::bold));
    widthTitle.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(widthSlider);
    widthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    widthAttachment.reset(new SliderAttachment(valueTreeState, "width", widthSlider));
    //widthAlgosBox.addItemList(juce::StringArray("Sine", "Haas"), 1);

    addAndMakeVisible(rotationBypassButton);
    rotationBypassAttachment.reset(new ButtonAttachment(valueTreeState, "rotationbypass", rotationBypassButton));

    addAndMakeVisible(rotationTitle);
    rotationTitle.setText("Rotation", juce::dontSendNotification);
    rotationTitle.setFont(juce::Font(16.0f, juce::Font::bold));
    rotationTitle.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(rotationSlider);
    rotationSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    rotationSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    rotationAttachment.reset(new SliderAttachment(valueTreeState, "rotation", rotationSlider));

    addAndMakeVisible(lpfLinkButton);
    lpfLinkAttachment.reset(new ButtonAttachment(valueTreeState, "lpflink", lpfLinkButton));

    addAndMakeVisible(lpfTitle);
    lpfTitle.setText("LPF", juce::dontSendNotification);
    lpfTitle.setFont(juce::Font(16.0f, juce::Font::bold));
    lpfTitle.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(lpfFreqSlider);
    lpfFreqSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    lpfFreqAttachment.reset(new SliderAttachment(valueTreeState, "lpffreq", lpfFreqSlider));

    setSize (260,580);
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
    
    int knobSide = 150;

    mainTitle.setBounds(35, 20, 80, 80);

    widthTitle.setBounds(35, 75, 80, 80);
    widthSlider.setBounds(0, 60, knobSide, knobSide);

    rotationTitle.setBounds(145, 195, 80, 80);
    rotationSlider.setBounds(110, 180, knobSide, knobSide);

    lpfTitle.setBounds(35, 315, 80, 80);
    lpfFreqSlider.setBounds(0, 300, knobSide, knobSide);

    gainTitle.setBounds(145, 435, 80, 80);
    gainSlider.setBounds(110, 420, knobSide, knobSide);
}
