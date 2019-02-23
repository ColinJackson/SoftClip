/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SoftClipAudioProcessor::SoftClipAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

	addParameter(gain = new AudioParameterFloat("gain", // parameterID
		"Gain", // parameter name
		0.0f,   // minimum value
		1.0f,   // maximum value
		0.5f)); // default value
	//_inputGain = 1.0f;
	//state = new AudioProcessorValueTreeState(*this, nullptr, "PARAMETERS", { std::make_unique<AudioParameterFloat>("inputGain", "Input Gain", NormalisableRange<float>(0.0f, 10.0f), 0) });

	state = new AudioProcessorValueTreeState(*this, nullptr);
	state->createAndAddParameter("inputGain", "inputGain", "inputGain", NormalisableRange<float>(0, 10), 1, nullptr, nullptr);
	state->state = ValueTree("savedParams");
}

SoftClipAudioProcessor::~SoftClipAudioProcessor()
{
}

//==============================================================================
const String SoftClipAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SoftClipAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SoftClipAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SoftClipAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SoftClipAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SoftClipAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SoftClipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SoftClipAudioProcessor::setCurrentProgram (int index)
{
}

const String SoftClipAudioProcessor::getProgramName (int index)
{
    return {};
}

void SoftClipAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SoftClipAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SoftClipAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SoftClipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void SoftClipAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

		buffer.applyGain(*gain);

		for (int i = 0; i < buffer.getNumSamples(); i++) {

			float in = channelData[i];

			if (_comboChoice == _cubic) {

				float a = _inputGain / 10.f;
				channelData[i] = in - a * (1.f / 3.f) * powf(in, 3.f);
			}
			else if (_comboChoice == _arctan) {

				channelData[i] = (2.f / M_PI) * atanf(in * _inputGain);
			}
			else if (_comboChoice == _exponential){

				channelData[i] = signum(in) * (1.f - expf(-fabs(_inputGain * in)));
			}
		}
    }
}

// extracts the sign of a float number
int SoftClipAudioProcessor::signum(float x)
{

	/*if (x > 0)
	return 1;
	else if (x < 0)
	return -1;
	else
	return 0;*/

	// Same as the above but with the ternary operator
	return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

//==============================================================================
bool SoftClipAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SoftClipAudioProcessor::createEditor()
{
    return new SoftClipAudioProcessorEditor (*this);
}

//==============================================================================
void SoftClipAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	/*ScopedPointer<XmlElement> xml(state->state.createXml());
	copyXmlToBinary(*xml, destData);*/

	MemoryOutputStream(destData, true).writeFloat(*gain);

}

void SoftClipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	/*ScopedPointer<XmlElement> theParams(getXmlFromBinary(data, sizeInBytes));

	if (theParams != nullptr) {

		if (theParams->hasTagName(state->state.getType())) {
			state->state = ValueTree::fromXml(*theParams);
		}
	}*/

	*gain = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();

}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SoftClipAudioProcessor();
}

AudioProcessorValueTreeState& SoftClipAudioProcessor::getTreeState() {
	return *state;
}