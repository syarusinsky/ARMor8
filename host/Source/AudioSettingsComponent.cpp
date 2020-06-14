#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSettingsComponent.h"

AudioSettingsComponent::AudioSettingsComponent (AudioDeviceManager& deviceManager, unsigned int inputs, unsigned int outputs,
						TextButton* audioSettingsBtn) :
	m_AudioSettingsButton( audioSettingsBtn ),
	m_AudioDeviceSelector( new AudioDeviceSelectorComponent(deviceManager, inputs, inputs, outputs, outputs, false, false, false, false) )
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
	DialogWindow::LaunchOptions options;
	options.content.setNonOwned( m_AudioDeviceSelector );

	const int width = 300;
	const int height = 500;

	Rectangle<int> area (0, 0, width, height);

	options.content->setSize ( area.getWidth(), area.getHeight() );

	options.dialogTitle                   = "Audio Settings";
	options.dialogBackgroundColour        = Colours::lightgrey;
	options.escapeKeyTriggersCloseButton  = true;
	options.useNativeTitleBar             = true;
	options.resizable                     = false;

	m_AudioDeviceSelectorWindow = options.launchAsync();

	if ( m_AudioDeviceSelectorWindow != nullptr )
	{
		m_AudioDeviceSelectorWindow->centreWithSize( width, height );
	}
}

void AudioSettingsComponent::paint (Graphics& g) {}

void AudioSettingsComponent::resized()
{
	int buttonWidth = 100;
	Rectangle<int> area = getLocalBounds();
	area.removeFromLeft( getWidth() - buttonWidth );
	m_AudioSettingsButton->setBounds( area );
}

void AudioSettingsComponent::buttonClicked (Button* button)
{
	if ( button == m_AudioSettingsButton )
	{
		this->showAudioDeviceSelectorWindow();
	}
}
