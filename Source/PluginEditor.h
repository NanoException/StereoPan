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
    juce::Slider rotationSlider;
    std::unique_ptr<SliderAttachment> rotationAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoPanAudioProcessorEditor)
};
