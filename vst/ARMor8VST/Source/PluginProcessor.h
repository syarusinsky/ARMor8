/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "AudioBuffer.hpp"
#include "ARMor8VoiceManager.hpp"
#include "ARMor8UiManager.hpp"
#include "MidiHandler.hpp"
#include "PresetManager.hpp"
#include "Font.hpp"
#include "Sprite.hpp"
#include "SampleRateConverter.hpp"

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ARMor8VSTAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ARMor8VSTAudioProcessor();
    ~ARMor8VSTAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    ARMor8VoiceManager& getARMor8VoiceManager() { return armor8VoiceManager; }
    ARMor8UiManager& getARMor8UiManager() { return armor8UiManager; }

    AudioProcessorValueTreeState& getVTS() { return apvts; }

    // this is a workaround for the fact that vst plugins share memory across multiple
    // instances, so the static member variables of the event listeners end up sending
    // events to all instances of the plugin
    void dispatchEventsForIds (unsigned int processorId, const unsigned int processorEditorId);
    unsigned int getProcessorId() { return processorId; }

private:
    ::AudioBuffer<float> sAudioBuffer;

    PresetManager presetManager;
    MidiHandler midiHandler;

    ::Font font;
    Sprite logo;

    ARMor8VoiceManager armor8VoiceManager;
    ARMor8UiManager armor8UiManager;

    SampleRateConverter<float, float> sampleRateConverter;

    UndoManager undoManager;
    AudioProcessorValueTreeState apvts;

    unsigned int processorId;
    unsigned int processorEditorId = 0 - 1; // this must be correctly initialized on the createEditor function

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ARMor8VSTAudioProcessor)
};
