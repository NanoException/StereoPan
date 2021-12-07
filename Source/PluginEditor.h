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
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StereoPanAudioProcessor& audioProcessor;

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Label mainTitle;

    juce::ImageButton bypassButton;
    std::unique_ptr<ButtonAttachment> bypassAttachment;

    juce::Label gainTitle;
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton widthBypassButton{"Width"};
    std::unique_ptr<ButtonAttachment> widthBypassAttachment;

    /*
    juce::ComboBox widthAlgosBox;
    std::unique_ptr<ComboBoxAttachment> widthAlgosAttachment;
    */
    juce::Label  widthTitle;
    juce::Slider widthSlider;
    std::unique_ptr<SliderAttachment> widthAttachment;
    
    juce::ToggleButton rotationBypassButton{"Rotation"};
    std::unique_ptr<ButtonAttachment> rotationBypassAttachment;
    
    juce::Label rotationTitle;
    juce::Slider rotationSlider;
    std::unique_ptr<SliderAttachment> rotationAttachment;
    
    juce::ToggleButton lpfLinkButton;
    std::unique_ptr<ButtonAttachment> lpfLinkAttachment;
    
    juce::Label lpfTitle;
    juce::Slider lpfFreqSlider;
    std::unique_ptr<SliderAttachment> lpfFreqAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoPanAudioProcessorEditor)
};
