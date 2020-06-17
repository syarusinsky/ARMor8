#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSettingsComponent.h"

AudioSettingsComponent::AudioSettingsComponent (juce::AudioDeviceManager& deviceManager, unsigned int inputs, unsigned int outputs,
						juce::TextButton* audioSettingsBtn) :
	m_AudioSettingsButton( audioSettingsBtn ),
	m_AudioDeviceSelector( new juce::AudioDeviceSelectorComponent(deviceManager, inputs, inputs, outputs, outputs, false, false, false, false) )
{
	m_AudioSettingsButton->addListener( this );
}

AudioSettingsComponent::~AudioSettingsComponent()
{
	delete m_AudioDeviceSelector;
	m_AudioSettingsButton->removeListener (this);
}

void AudioSettingsComponent::showAudioDeviceSelectorWindow()
{
	juce::DialogWindow::LaunchOptions options;
	options.content.setNonOwned( m_AudioDeviceSelector );

	const int width = 300;
	const int height = 500;

	juce::Rectangle<int> area (0, 0, width, height);

	options.content->setSize ( area.getWidth(), area.getHeight() );

	options.dialogTitle                   = "Audio Settings";
	options.dialogBackgroundColour        = juce::Colours::lightgrey;
	options.escapeKeyTriggersCloseButton  = true;
	options.useNativeTitleBar             = true;
	options.resizable                     = false;

	m_AudioDeviceSelectorWindow = options.launchAsync();

	if ( m_AudioDeviceSelectorWindow != nullptr )
	{
		m_AudioDeviceSelectorWindow->centreWithSize( width, height );
	}
}

void AudioSettingsComponent::paint (juce::Graphics& g) {}

void AudioSettingsComponent::resized()
{
	int buttonWidth = 100;
	juce::Rectangle<int> area = getLocalBounds();
	area.removeFromLeft( getWidth() - buttonWidth );
	m_AudioSettingsButton->setBounds( area );
}

void AudioSettingsComponent::buttonClicked (juce::Button* button)
{
	if ( button == m_AudioSettingsButton )
	{
		this->showAudioDeviceSelectorWindow();
	}
}
