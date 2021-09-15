/*
   ==============================================================================

   This file was auto-generated!

   ==============================================================================
   */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioBuffer.hpp"
#include "ARMor8VoiceManager.hpp"
#include "IARMor8PresetEventListener.hpp"
#include "IARMor8LCDRefreshEventListener.hpp"
#include "MidiHandler.hpp"
#include "PresetManager.hpp"
#include "AudioSettingsComponent.h"
#include "ARMor8UiManager.hpp"

#include <iostream>
#include <fstream>

//==============================================================================
/*
   This component lives inside our window, and this is where you should put all
   your controls and content.
   */
class MainComponent   : public juce::AudioAppComponent, public juce::Slider::Listener, public juce::Button::Listener,
			public juce::MidiInputCallback, public IARMor8PresetEventListener, public juce::Timer,
			public IARMor8LCDRefreshEventListener
{
	public:
		//==============================================================================
		MainComponent();
		~MainComponent();

		//==============================================================================
		void timerCallback() override;

		//==============================================================================
		void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
		void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
		void releaseResources() override;

		//==============================================================================
		void paint (juce::Graphics& g) override;
		void resized() override;

		//==============================================================================
		bool keyPressed (const juce::KeyPress& k) override;
		bool keyStateChanged (bool isKeyDown) override;

		void sliderValueChanged (juce::Slider* slider) override;
		void buttonClicked (juce::Button* button) override;
		void updateToggleState (juce::Button* button);

		void onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent) override;

		void onARMor8LCDRefreshEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent) override;

		void setMidiInput (int index);
		void handleIncomingMidiMessage (juce::MidiInput *source, const juce::MidiMessage &message) override;

	private:
		//==============================================================================
		// Your private member variables go here...
		PresetManager presetManager;
		MidiHandler midiHandler;
		int lastInputIndex;
		::AudioBuffer<float> sAudioBuffer;
		ARMor8VoiceManager armor8VoiceManager;
		bool keyButtonRelease;

		juce::AudioFormatWriter* writer;

		juce::Slider effect1Sldr;
		juce::Label effect1Lbl;
		juce::Slider effect2Sldr;
		juce::Label effect2Lbl;
		juce::Slider effect3Sldr;
		juce::Label effect3Lbl;

		juce::TextButton effect1Btn;
		juce::TextButton effect2Btn;

		juce::TextButton audioSettingsBtn;

		juce::ComboBox midiInputList;
		juce::Label midiInputListLbl;

		AudioSettingsComponent audioSettingsComponent;

		ARMor8UiManager uiSim;

		juce::Image screenRep;

		std::ofstream testFile;

		void setFromARMor8VoiceState (const ARMor8VoiceState& state, unsigned int opToEdit, unsigned int presetNum);
		void copyFrameBufferToImage (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
