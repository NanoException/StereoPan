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

    /*
    enum Parameters {
        MasterBypass = 0,
        Gain,
        Width,
        Rotation,
        LPFLink,
        LPFFreq,
        PanLaw,
        totalNumParam
    };
    
    enum class panLawAlgos :int
    {
        ConstantPower,
        ConstantGain,
        Log3,
        Log4_5,
        Log6,
        totalNumPan
    };
    */

    int getNumParameters() override;
    float getParameter(int index) override;
    void setParameter(int index, float value) override;
    const juce::String getParameterName(int index)  override;
    const juce::String getParameterText(int index) override;


private:
    //double UserParams[totalNumParam];
    //panLawAlgos panAlgoList = panLawAlgos::ConstantPower;

    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* gain = nullptr;
    std::atomic<float>* width = nullptr;
    std::atomic<float>* rotation = nullptr;
    std::atomic<float>* lpfLink = nullptr;
    std::atomic<float>* lpfFreq = nullptr;
    std::atomic<float>* panRule = nullptr;

    template<class sampleType>
    void processBlockWrapper(juce::AudioBuffer<sampleType>& buffer, juce::MidiBuffer& midiMessages);

    juce::dsp::IIR::Filter<double> LowPassL, LowPassR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoPanAudioProcessor)
};
