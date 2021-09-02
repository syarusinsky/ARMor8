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
		juce::Slider freqSldr;
		juce::Label freqLbl;
		juce::Slider detuneSldr;
		juce::Label detuneLbl;
		juce::ToggleButton ratioBtn;

		juce::Label editLbl;
		juce::ToggleButton op1Btn;
		juce::ToggleButton op2Btn;
		juce::ToggleButton op3Btn;
		juce::ToggleButton op4Btn;

		juce::Label waveLbl;
		juce::ToggleButton sineBtn;
		juce::ToggleButton triangleBtn;
		juce::ToggleButton squareBtn;
		juce::ToggleButton sawBtn;

		juce::Slider attackSldr;
		juce::Label attackLbl;
		juce::Slider attackExpoSldr;
		juce::Label attackExpoLbl;
		juce::Slider decaySldr;
		juce::Label decayLbl;
		juce::Slider decayExpoSldr;
		juce::Label decayExpoLbl;
		juce::Slider sustainSldr;
		juce::Label sustainLbl;
		juce::Slider releaseSldr;
		juce::Label releaseLbl;
		juce::Slider releaseExpoSldr;
		juce::Label releaseExpoLbl;

		juce::Label egDestLbl;
		juce::ToggleButton amplitudeDestBtn;
		juce::ToggleButton frequencyDestBtn;
		juce::ToggleButton filterDestBtn;

		juce::Slider op1ModAmountSldr;
		juce::Label op1ModAmountLbl;
		juce::Slider op2ModAmountSldr;
		juce::Label op2ModAmountLbl;
		juce::Slider op3ModAmountSldr;
		juce::Label op3ModAmountLbl;
		juce::Slider op4ModAmountSldr;
		juce::Label op4ModAmountLbl;

		juce::Slider amplitudeSldr;
		juce::Label amplitudeLbl;

		juce::Slider filterFreqSldr;
		juce::Label filterFreqLbl;

		juce::Slider filterResSldr;
		juce::Label filterResLbl;

		juce::Slider ampVelSldr;
		juce::Label ampVelLbl;

		juce::Slider filtVelSldr;
		juce::Label filtVelLbl;

		juce::Slider pitchBendSldr;
		juce::Label pitchBendLbl;

		juce::Slider glideSldr;
		juce::Label glideLbl;

		juce::ToggleButton egRetriggerBtn;

		juce::TextButton audioSettingsBtn;

		juce::ComboBox midiInputList;
		juce::Label midiInputListLbl;

		juce::ToggleButton monoBtn;

		juce::TextButton prevPresetBtn;
		juce::Label presetNumLbl;
		juce::TextButton nextPresetBtn;
		juce::TextButton writePresetBtn;

		AudioSettingsComponent audioSettingsComponent;

		ARMor8UiManager uiSim;

		juce::Image screenRep;

		std::ofstream testFile;

		void setFromARMor8VoiceState (const ARMor8VoiceState& state, unsigned int opToEdit, unsigned int presetNum);
		void copyFrameBufferToImage (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
