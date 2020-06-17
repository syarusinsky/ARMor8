#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// The AudioSettingsComponent class is used to launch a window that displays audio settings when a button is pressed.
class AudioSettingsComponent 	: public juce::Component, public juce::Button::Listener
{
	public:
		AudioSettingsComponent (juce::AudioDeviceManager& deviceManager, unsigned int inputs, unsigned int outputs,
					juce::TextButton* audioSettingsBtn);
		~AudioSettingsComponent();

		void showAudioDeviceSelectorWindow();
		
		// component virtual functions
		void paint (juce::Graphics& g) override;
		void resized() override;
		
		// button::listener virtual functions
		void buttonClicked (juce::Button* button) override;

	private:
		juce::TextButton* m_AudioSettingsButton;
		
		juce::AudioDeviceSelectorComponent* m_AudioDeviceSelector;
		
		SafePointer<juce::DialogWindow> m_AudioDeviceSelectorWindow;	
		
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSettingsComponent)
};
