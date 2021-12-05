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
            std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 1.0f, 0.7f),
            std::make_unique<juce::AudioParameterFloat>("width", "Width", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f),
            std::make_unique<juce::AudioParameterChoice>("widthalgos", "widthAlgos", juce::StringArray("Sine", "Haas"), 0),
            std::make_unique<juce::AudioParameterBool>("widthbypass", "widthBypass", false),
            std::make_unique<juce::AudioParameterFloat>("rotation", "Rotation", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f),
            std::make_unique<juce::AudioParameterBool>("rotationbypass", "rotationBypass", false),
            std::make_unique<juce::AudioParameterBool>("lpflink", "LPFLink", false),
            std::make_unique<juce::AudioParameterFloat>("lpffreq", "LPFFreq", juce::NormalisableRange<float>(0.0f, 20000.0f),20000.0f),
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
    processBlockWrapper(buffer, midiMessages);
}


void StereoPanAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    processBlockWrapper(buffer, midiMessages);
}


template <class sampleType>
void StereoPanAudioProcessor::processBlockWrapper(juce::AudioBuffer<sampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (*masterBypass != false) return;

    for (int channel = 0; channel < totalNumInputChannels; ++channel){
        /**** Get LR channels ****/
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);

        /**** Caluculate angles of width and rotation ****/
        float valWidth = *width;
        float valRotation = *rotation;

        float isWidthBypass = *widthBypass;
        float isRotationBypass = *rotationBypass;

        double Theta_w = M_PI / 2 * valWidth - M_PI / 4;
        if (isWidthBypass > 0.5f){  //Bypass width
            Theta_w = 0.0;
        }

        double Theta_r = -(M_PI / 2 * valRotation - M_PI / 4);
        if (isRotationBypass > 0.5f){   //Bypass rotation
            Theta_r = 0.0;
        }

        float valLPFFreq = *lpfFreq;
        double LPFBias = 2 * abs(0.5f - valRotation);
        double _frequency = LPFBias * valLPFFreq + (1 - LPFBias) * 20000.0f;
        double _Q = 0.7;
        bool isLPFBypass = *lpfLink;

        //Get the sampling rate
        double _samplerate = getSampleRate();

        double prevMid[int(_samplerate / 100)] = {};

        /**** Apply stereo width and rotation ****/
        for (int i = 0; i < buffer.getNumSamples(); ++i){
            //Generate MS signals
            auto midInput = (leftChannel[i] + rightChannel[i]);
            auto sideInput = (leftChannel[i] - rightChannel[i]);
            //Processing stereo width
            int valWidthAlgos = *widthAlgos;

            auto midWidth = midInput;
            auto sideWidth = sideInput;

            switch (valWidthAlgos){
            case 0:
                midWidth = midInput * sin(M_PI / 4 - Theta_w) * sqrt(2);
                sideWidth = sideInput * cos(M_PI / 4 - Theta_w) * sqrt(2);
                break;
            case 1:
                sideWidth = sideWidth + 10 * valWidth * prevMid[i];
                prevMid[i] = midWidth;
                break;
            default:
                break;
            }
            //Processing rotation
            auto midRotation = midWidth * cos(Theta_r) - sideWidth * sin(Theta_r);
            auto sideRotation = midWidth * sin(Theta_r) + sideWidth * cos(Theta_r);
            //Revert to LR signals
            leftChannel[i] = (midRotation + sideRotation);
            rightChannel[i] = (midRotation - sideRotation);
            //Apply LPFLink
            if (isLPFBypass > 0.5f){
                if (Theta_r > 0.0){
                    LowPassR.reset();
                    LowPassR.coefficients = juce::dsp::IIR::Coefficients<double>::makeLowPass(_samplerate, _frequency, _Q);
                    rightChannel[i] = LowPassR.processSample(rightChannel[i]);
                }
                else if (Theta_r < 0.0){
                    LowPassL.reset();
                    LowPassL.coefficients = juce::dsp::IIR::Coefficients<double>::makeLowPass(_samplerate, _frequency, _Q);
                    leftChannel[i] = LowPassL.processSample(leftChannel[i]);
                }
            }
        }

        /**** Post Gain ****/
        float valGain = *gain;
        buffer.applyGain(pow(valGain, 2));
    }
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
