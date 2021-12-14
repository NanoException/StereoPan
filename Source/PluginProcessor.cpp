/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "cmath"
#include "corecrt_math_defines.h"

//==============================================================================
StereoPanAudioProcessor::StereoPanAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
    ,
    parameters(*this, nullptr, juce::Identifier("StereoPan"),
        {
            std::make_unique<juce::AudioParameterBool>("masterbypass", "MasterBypass", false),
            std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -20.0f, 10.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("width", "Width", juce::NormalisableRange<float>(0.0f, 100.0f), 50.0f),
            std::make_unique<juce::AudioParameterChoice>("widthalgos", "widthAlgos", juce::StringArray("Sine", "Haas"), 0),
            std::make_unique<juce::AudioParameterBool>("widthbypass", "widthBypass", false),
            std::make_unique<juce::AudioParameterFloat>("rotation", "Rotation", juce::NormalisableRange<float>(-100.0f, 100.0f), 0.0f),
            std::make_unique<juce::AudioParameterBool>("rotationbypass", "rotationBypass", false),
            std::make_unique<juce::AudioParameterBool>("lpflink", "LPFLink", false),
            std::make_unique<juce::AudioParameterFloat>("lpffreq", "LPFFreq", juce::NormalisableRange<float>(1.0f, 20000.0f),20000.0f),
        })
{
    masterBypass = parameters.getRawParameterValue("masterbypass");
    gain = parameters.getRawParameterValue("gain");
    width = parameters.getRawParameterValue("width");
    widthAlgos = parameters.getRawParameterValue("widthalgos");
    widthBypass = parameters.getRawParameterValue("widthbypass");
    rotation = parameters.getRawParameterValue("rotation");
    rotationBypass = parameters.getRawParameterValue("rotationbypass");
    lpfLink = parameters.getRawParameterValue("lpflink");
    lpfFreq = parameters.getRawParameterValue("lpffreq");
}

StereoPanAudioProcessor::~StereoPanAudioProcessor()
{
}

//==============================================================================
const juce::String StereoPanAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StereoPanAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StereoPanAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StereoPanAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StereoPanAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StereoPanAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StereoPanAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StereoPanAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String StereoPanAudioProcessor::getProgramName (int index)
{
    return {};
}

void StereoPanAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void StereoPanAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock * 2;
    spec.numChannels = getTotalNumOutputChannels();

    history.resize(spec.sampleRate / 100);
    history2.resize(spec.sampleRate / 350);

    LowPassL.prepare(spec);
    LowPassL.reset();
    LowPassR.prepare(spec);
    LowPassR.reset();
}

void StereoPanAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StereoPanAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StereoPanAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    //master bypass
    if (*masterBypass != false) return;

    //prepare block and context
    auto block = juce::dsp::AudioBlock<double>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<double>(block);

    auto& inBlock = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    auto oversampledBlock = oversamplingProcessorDouble.processSamplesUp(inBlock);
    auto oversampledContext = juce::dsp::ProcessContextReplacing<double>(oversampledBlock);

    auto& oversampledInBlock = oversampledContext.getInputBlock();
    auto& oversampledoutBlock = oversampledContext.getOutputBlock();

    auto leftInBlock = oversampledInBlock.getSingleChannelBlock(0);
    auto rightInBlock = oversampledInBlock.getSingleChannelBlock(1);
    auto leftOutBlock = oversampledoutBlock.getSingleChannelBlock(0);
    auto rightOutBlock = oversampledoutBlock.getSingleChannelBlock(1);

    //get parameters
    double _samplerate = getSampleRate();

    float isWidthBypass = *widthBypass;
    float isRotationBypass = *rotationBypass;

    float valWidthAlgos = *widthAlgos;

    float valWidth = *width;
    float valRotation = *rotation;

    double Theta_w = M_PI / 200 * (valWidth - 50);
    if (isWidthBypass > 0.5f) {  //Bypass width
        Theta_w = 0.0;
    }

    double Theta_r = -M_PI / 400 * valRotation;
    if (isRotationBypass > 0.5f) {   //Bypass rotation
        Theta_r = 0.0;
    }

    float valLPFFreq = *lpfFreq;
    double LPFBias = abs(valRotation) / 100;
    double _frequency = LPFBias * valLPFFreq + (1 - LPFBias) * 20000.0f;
    double _Q = 0.7;
    bool isLPFBypass = *lpfLink;

    float valPostGain = *gain;

    if (valWidthAlgos < 1. / 2.)
    {
        for (int i = 0; i < inBlock.getNumSamples(); ++i)
        {
            auto midInput = (leftInBlock.getSample(0, i) + rightInBlock.getSample(0, i)) / sqrt(2);
            auto sideInput = (leftInBlock.getSample(0, i) - rightInBlock.getSample(0, i)) / sqrt(2);
            auto midWidth = midInput * sin(M_PI / 4 - Theta_w) * sqrt(2);
            auto sideWidth = sideInput * cos(M_PI / 4 - Theta_w) * sqrt(2);

            auto midRotation = midInput * cos(Theta_r) - sideInput * sin(Theta_r);
            auto sideRotation = midInput * sin(Theta_r) + sideInput * cos(Theta_r);

            double leftRotation = (midRotation + sideRotation) / sqrt(2);
            double rightRotation = (midRotation - sideRotation) / sqrt(2);

            if (isLPFBypass < 0.5f)
            {
                if (Theta_r > 0.0)
                {
                    LowPassR.reset();
                    LowPassR.coefficients = juce::dsp::IIR::Coefficients<double>::makeLowPass(_samplerate, _frequency, _Q);
                    float rightFiltered = LowPassR.processSample(rightRotation);
                    rightOutBlock.setSample(0, i, rightFiltered);
                }
                else if (Theta_r < 0.0)
                {
                    LowPassL.reset();
                    LowPassL.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(_samplerate, _frequency, _Q);
                    float leftFiltered = LowPassL.processSample(leftRotation);
                    leftOutBlock.setSample(0, i, leftFiltered);
                }
            }
            else
            {
                leftOutBlock.setSample(0, i, leftRotation);
                rightOutBlock.setSample(0, i, rightRotation);
            }
        }
    }
    else
    {
        for (int i = 0; i < inBlock.getNumSamples(); ++i)
        {
            double p = valWidth / 100.;
            auto midInput = (leftInBlock.getSample(0, i) + rightInBlock.getSample(0, i)) / sqrt(2);
            auto sideInput = (leftInBlock.getSample(0, i) - rightInBlock.getSample(0, i)) / sqrt(2);
            auto midWidth = midInput;
            double midDelay = history[pos];
            double midDelay2 = history2[pos2];

            double leftWidth = (midInput + sideInput) / sqrt(2) + p * midDelay;
            double rightWidth = (midInput - sideInput) / sqrt(2) + p * midDelay2;

            history[pos] = midWidth;
            history2[pos2] = midWidth;
            pos = (pos + 1) % history.size();
            pos2 = (pos2 + 1) % history2.size();

            double leftRotation = leftWidth * cos(Theta_r) - rightWidth * sin(Theta_r);
            double rightRotation = leftWidth * sin(Theta_r) + rightWidth * cos(Theta_r);

            if (isLPFBypass < 0.5f)
            {
                if (Theta_r > 0.0)
                {
                    LowPassR.reset();
                    LowPassR.coefficients = juce::dsp::IIR::Coefficients<double>::makeLowPass(_samplerate, _frequency, _Q);
                    float rightFiltered = LowPassR.processSample(rightRotation);
                    rightOutBlock.setSample(0, i, rightFiltered);
                }
                else if (Theta_r < 0.0)
                {
                    LowPassL.reset();
                    LowPassL.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(_samplerate, _frequency, _Q);
                    float leftFiltered = LowPassL.processSample(leftRotation);
                    leftOutBlock.setSample(0, i, leftFiltered);
                }
            }
            else
            {
                leftOutBlock.setSample(0, i, leftRotation);
                rightOutBlock.setSample(0, i, rightRotation);
            }
        }
    }
    postGainProcessor.setGainDecibels(valPostGain);
    postGainProcessor.process(oversampledContext);

    oversamplingProcessorDouble.processSamplesDown(outBlock);
}


void StereoPanAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    //master bypass
    if (*masterBypass != false) return;

    //prepare block and context
    auto block = juce::dsp::AudioBlock<double>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<double>(block);

    auto& inBlock = context.getInputBlock();
    auto& outBlock = context.getOutputBlock();

    auto oversampledBlock = oversamplingProcessorDouble.processSamplesUp(inBlock);
    auto oversampledContext = juce::dsp::ProcessContextReplacing<double>(oversampledBlock);

    auto& oversampledInBlock = oversampledContext.getInputBlock();
    auto& oversampledoutBlock = oversampledContext.getOutputBlock();

    auto leftInBlock = oversampledInBlock.getSingleChannelBlock(0);
    auto rightInBlock = oversampledInBlock.getSingleChannelBlock(1);
    auto leftOutBlock = oversampledoutBlock.getSingleChannelBlock(0);
    auto rightOutBlock = oversampledoutBlock.getSingleChannelBlock(1);

    //get parameters
    double _samplerate = getSampleRate();

    float isWidthBypass = *widthBypass;
    float isRotationBypass = *rotationBypass;

    float valWidthAlgos = *widthAlgos;

    float valWidth = *width;
    float valRotation = *rotation;

    double Theta_w = M_PI / 200 * (valWidth - 50);
    if (isWidthBypass > 0.5f) {  //Bypass width
        Theta_w = 0.0;
    }

    double Theta_r = -M_PI / 400 * valRotation;
    if (isRotationBypass > 0.5f) {   //Bypass rotation
        Theta_r = 0.0;
    }

    float valLPFFreq = *lpfFreq;
    double LPFBias = abs(valRotation) / 100;
    double _frequency = LPFBias * valLPFFreq + (1 - LPFBias) * 20000.0f;
    double _Q = 0.7;
    bool isLPFBypass = *lpfLink;

    float valPostGain = *gain;

    if (valWidthAlgos < 1. / 2.)
    {
        for (int i = 0; i < inBlock.getNumSamples(); ++i)
        {
            auto midInput = (leftInBlock.getSample(0, i) + rightInBlock.getSample(0, i)) / sqrt(2);
            auto sideInput = (leftInBlock.getSample(0, i) - rightInBlock.getSample(0, i)) / sqrt(2);
            auto midWidth = midInput * sin(M_PI / 4 - Theta_w) * sqrt(2);
            auto sideWidth = sideInput * cos(M_PI / 4 - Theta_w) * sqrt(2);

            auto midRotation = midInput * cos(Theta_r) - sideInput * sin(Theta_r);
            auto sideRotation = midInput * sin(Theta_r) + sideInput * cos(Theta_r);

            double leftRotation = (midRotation + sideRotation) / sqrt(2);
            double rightRotation = (midRotation - sideRotation) / sqrt(2);

            if (isLPFBypass < 0.5f)
            {
                if (Theta_r > 0.0)
                {
                    LowPassR.reset();
                    LowPassR.coefficients = juce::dsp::IIR::Coefficients<double>::makeLowPass(_samplerate, _frequency, _Q);
                    float rightFiltered = LowPassR.processSample(rightRotation);
                    rightOutBlock.setSample(0, i, rightFiltered);
                }
                else if (Theta_r < 0.0)
                {
                    LowPassL.reset();
                    LowPassL.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(_samplerate, _frequency, _Q);
                    float leftFiltered = LowPassL.processSample(leftRotation);
                    leftOutBlock.setSample(0, i, leftFiltered);
                }
            }
            else
            {
                leftOutBlock.setSample(0, i, leftRotation);
                rightOutBlock.setSample(0, i, rightRotation);
            }
        }
    }
    else
    {
        for (int i = 0; i < inBlock.getNumSamples(); ++i)
        {
            double p = valWidth / 100.;
            auto midInput = (leftInBlock.getSample(0, i) + rightInBlock.getSample(0, i)) / sqrt(2);
            auto sideInput = (leftInBlock.getSample(0, i) - rightInBlock.getSample(0, i)) / sqrt(2);
            auto midWidth = midInput;
            double midDelay = history[pos];
            double midDelay2 = history2[pos2];

            double leftWidth = (midInput + sideInput) / sqrt(2) + p * midDelay;
            double rightWidth = (midInput - sideInput) / sqrt(2) + p * midDelay2;

            history[pos] = midWidth;
            history2[pos2] = midWidth;
            pos = (pos + 1) % history.size();
            pos2 = (pos2 + 1) % history2.size();

            double leftRotation = leftWidth * cos(Theta_r) - rightWidth * sin(Theta_r);
            double rightRotation = leftWidth * sin(Theta_r) + rightWidth * cos(Theta_r);

            if (isLPFBypass < 0.5f)
            {
                if (Theta_r > 0.0) 
                {
                    LowPassR.reset();
                    LowPassR.coefficients = juce::dsp::IIR::Coefficients<double>::makeLowPass(_samplerate, _frequency, _Q);
                    float rightFiltered = LowPassR.processSample(rightRotation);
                    rightOutBlock.setSample(0, i, rightFiltered);
                }
                else if (Theta_r < 0.0) 
                {
                    LowPassL.reset();
                    LowPassL.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(_samplerate, _frequency, _Q);
                    float leftFiltered = LowPassL.processSample(leftRotation);
                    leftOutBlock.setSample(0, i, leftFiltered);
                }
            }
            else
            {
                leftOutBlock.setSample(0, i, leftRotation);
                rightOutBlock.setSample(0, i, rightRotation);
            }
        }
    }
    postGainProcessor.setGainDecibels(valPostGain);
    postGainProcessor.process(oversampledContext);

    oversamplingProcessorDouble.processSamplesDown(outBlock);
}

bool StereoPanAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return true;
}

//==============================================================================
bool StereoPanAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StereoPanAudioProcessor::createEditor()
{
    return new StereoPanAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void StereoPanAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void StereoPanAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StereoPanAudioProcessor();
}
