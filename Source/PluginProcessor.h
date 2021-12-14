/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class StereoPanAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    StereoPanAudioProcessor();
    ~StereoPanAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

    template <class sampleType>
    void processBufferSamples(juce::AudioBuffer<sampleType>&, juce::MidiBuffer&);

    bool supportsDoublePrecisionProcessing() const override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* masterBypass = nullptr;
    std::atomic<float>* gain = nullptr;
    std::atomic<float>* width = nullptr;
    std::atomic<float>* widthAlgos = nullptr;
    std::atomic<float>* widthBypass = nullptr;
    std::atomic<float>* rotation = nullptr;
    std::atomic<float>* rotationBypass = nullptr;
    std::atomic<float>* lpfLink = nullptr;
    std::atomic<float>* lpfFreq = nullptr;

    std::vector<double> history;
    std::vector<double> history2;
    int pos = 0;
    int pos2 = 0;

    juce::dsp::IIR::Filter<double> LowPassL, LowPassR;

    juce::dsp::Oversampling<float> oversamplingProcessorFloat{ 2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true };
    juce::dsp::Oversampling<double> oversamplingProcessorDouble{ 2, 1, juce::dsp::Oversampling<double>::filterHalfBandPolyphaseIIR, true, true };

    juce::dsp::Gain<float> postGainProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoPanAudioProcessor)
};
