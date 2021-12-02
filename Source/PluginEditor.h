/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class StereoPanAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    StereoPanAudioProcessorEditor (StereoPanAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~StereoPanAudioProcessorEditor() override;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StereoPanAudioProcessor& audioProcessor;

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;
    juce::ToggleButton widthBypassButton{"Width"};
    std::unique_ptr<ButtonAttachment> widthBypassAttachment;
    juce::Slider widthSlider;
    std::unique_ptr<SliderAttachment> widthAttachment;
    juce::ToggleButton rotationBypassButton;
    std::unique_ptr<ButtonAttachment> rotationBypassAttachment;
    juce::Slider rotationSlider;
    std::unique_ptr<SliderAttachment> rotationAttachment;
    juce::ToggleButton lpfLinkButton{"LPFLink"};
    std::unique_ptr<ButtonAttachment> lpfLinkAttachment;
    juce::Slider lpfFreqSlider;
    std::unique_ptr<SliderAttachment> lpfFreqAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoPanAudioProcessorEditor)
};
