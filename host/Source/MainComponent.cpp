/*
   ==============================================================================

   This file was auto-generated!

   ==============================================================================
   */

#ifdef __unix__
#include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#endif

#include "MainComponent.h"

#include "CPPFile.hpp"
#include "ARMor8PresetUpgrader.hpp"
#include "ARMor8Constants.hpp"
#include "ColorProfile.hpp"
#include "FrameBuffer.hpp"
#include "Font.hpp"
#include "Sprite.hpp"

// FLUSH DENORMALS TO ZERO!
#include <xmmintrin.h>
#include <pmmintrin.h>

const unsigned int FONT_FILE_SIZE = 779;
const unsigned int LOGO_FILE_SIZE = 119;

const int OpRadioId = 1001;
const int WaveRadioId = 1002;

//==============================================================================
MainComponent::MainComponent() :
	presetManager( sizeof(ARMor8PresetHeader), 20, new CPPFile("ARMor8Presets.spf") ),
	midiHandler(),
	lastInputIndex( 0 ),
	sAudioBuffer(),
	armor8VoiceManager( &midiHandler, &presetManager ),
	keyButtonRelease( false ),
	writer(),
	effect1Sldr(),
	effect1Lbl(),
	effect2Sldr(),
	effect2Lbl(),
	effect3Sldr(),
	effect3Lbl(),
	effect1Btn( "Effect 1" ),
	effect2Btn( "Effect 2" ),
	audioSettingsBtn( "Audio Settings" ),
	midiInputList(),
	midiInputListLbl(),
	audioSettingsComponent( deviceManager, 2, 2, &audioSettingsBtn ),
	uiSim( 128, 64, CP_FORMAT::MONOCHROME_1BIT ),
	screenRep( juce::Image::RGB, 256, 128, true ) // this is actually double the size so we can actually see it
{
	// FLUSH DENORMALS TO ZERO!
	_MM_SET_FLUSH_ZERO_MODE( _MM_FLUSH_ZERO_ON );
	_MM_SET_DENORMALS_ZERO_MODE( _MM_DENORMALS_ZERO_ON );

	// connecting to event system
	this->bindToARMor8PresetEventSystem();
	this->bindToARMor8LCDRefreshEventSystem();
	armor8VoiceManager.bindToKeyEventSystem();
	armor8VoiceManager.bindToPitchEventSystem();
	armor8VoiceManager.bindToButtonEventSystem();
	uiSim.bindToPotEventSystem();

	// load font and logo from file
	char* fontBytes = new char[FONT_FILE_SIZE];
	char* logoBytes = new char[LOGO_FILE_SIZE];

	const unsigned int pathMax = 1000;
#ifdef __unix__
	char result[pathMax];
	ssize_t count = readlink( "/proc/self/exe", result, pathMax );
	std::string assetsPath( result, (count > 0) ? count : 0 );
	std::string strToRemove( "host/Builds/LinuxMakefile/build/FMSynth" );

	std::string::size_type i = assetsPath.find( strToRemove );

	if ( i != std::string::npos )
	{
		assetsPath.erase( i, strToRemove.length() );
	}

	assetsPath += "assets/";

	std::string fontPath = assetsPath + "Smoll.sff";
	std::string logoPath = assetsPath + "theroomdisconnectlogo.sif";
#elif defined(_WIN32) || defined(WIN32)
	// TODO need to actually implement this for windows
#endif
	std::ifstream fontFile( fontPath, std::ifstream::binary );
	if ( ! fontFile )
	{
		std::cout << "FAILED TO OPEN FONT FILE!" << std::endl;
	}
	fontFile.read( fontBytes, FONT_FILE_SIZE );
	fontFile.close();

	Font* font = new Font( (uint8_t*)fontBytes );
	uiSim.setFont( font );

	std::ifstream logoFile( logoPath, std::ifstream::binary );
	if ( ! logoFile )
	{
		std::cout << "FAILED TO OPEN LOGO FILE!" << std::endl;
	}
	logoFile.read( logoBytes, LOGO_FILE_SIZE );
	logoFile.close();

	Sprite* logo = new Sprite( (uint8_t*)logoBytes );
	uiSim.setLogo( logo );

	// this is to avoid unwanted audio on startup
	armor8VoiceManager.setOperatorAmplitude(0, 0.0f);
	armor8VoiceManager.setOperatorAmplitude(1, 0.0f);
	armor8VoiceManager.setOperatorAmplitude(2, 0.0f);
	armor8VoiceManager.setOperatorAmplitude(3, 0.0f);

	// Some platforms require permissions to open input channels so request that here
	if ( juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
			&& ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio) )
	{
		juce::RuntimePermissions::request( juce::RuntimePermissions::recordAudio,
				[&] (bool granted) { if (granted)  setAudioChannels (2, 2); } );
	}
	else
	{
		// Specify the number of input and output channels that we want to open
		setAudioChannels (2, 2);
	}

	// connecting the audio buffer to the voice manager
	sAudioBuffer.registerCallback( &armor8VoiceManager );

	// juce audio device setup
	juce::AudioDeviceManager::AudioDeviceSetup deviceSetup = juce::AudioDeviceManager::AudioDeviceSetup();
	deviceSetup.sampleRate = 44100;
	deviceManager.initialise( 2, 2, 0, true, juce::String(), &deviceSetup );

	// basic juce logging
	// juce::Logger* log = juce::Logger::getCurrentLogger();
	// int sampleRate = deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();
	// log->writeToLog( juce::String(sampleRate) );
	// log->writeToLog( juce::String(deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples()) );

	// optionally we can write wav files for debugging
	// juce::WavAudioFormat wav;
	// juce::File tempFile( "TestAudio.wav" );
	// juce::OutputStream* outStream( tempFile.createOutputStream() );
	// writer = wav.createWriterFor( outStream, sampleRate, 2, wav.getPossibleBitDepths().getLast(), NULL, 0 );

	// log->writeToLog( juce::String(wav.getPossibleBitDepths().getLast()) );
	// log->writeToLog( tempFile.getFullPathName() );

	// this file can also be used for debugging
	testFile.open( "JuceMainComponentOutput.txt" );

	// adding all child components
	addAndMakeVisible( effect1Sldr );
	effect1Sldr.setRange( 0.0f, 1.0f );
	effect1Sldr.addListener( this );
	addAndMakeVisible( effect1Lbl );
	effect1Lbl.setText( "Effect 1 Pot", juce::dontSendNotification );
	effect1Lbl.attachToComponent( &effect1Sldr, true );

	addAndMakeVisible( effect2Sldr );
	effect2Sldr.setRange( 0.0f, 1.0f );
	effect2Sldr.addListener( this );
	addAndMakeVisible( effect2Lbl );
	effect2Lbl.setText( "Effect 2 Pot", juce::dontSendNotification );
	effect2Lbl.attachToComponent( &effect2Sldr, true );

	addAndMakeVisible( effect3Sldr );
	effect3Sldr.setRange( 0.0f, 1.0f );
	effect3Sldr.addListener( this );
	addAndMakeVisible( effect3Lbl );
	effect3Lbl.setText( "Effect 3 Pot", juce::dontSendNotification );
	effect3Lbl.attachToComponent( &effect3Sldr, true );

	addAndMakeVisible( effect1Btn );
	effect1Btn.addListener( this );

	addAndMakeVisible( effect2Btn );
	effect2Btn.addListener( this );

	addAndMakeVisible( audioSettingsBtn );
	audioSettingsBtn.addListener( this );

	addAndMakeVisible( midiInputList );
	midiInputList.setTextWhenNoChoicesAvailable( "No MIDI Inputs Enabled" );
	auto midiInputs = juce::MidiInput::getDevices();
	midiInputList.addItemList( midiInputs, 1 );
	midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
	// find the first enabled device and use that by default
	for ( auto midiInput : midiInputs )
	{
		if ( deviceManager.isMidiInputEnabled (midiInput) )
		{
			setMidiInput( midiInputs.indexOf (midiInput) );
			break;
		}
	}
	// if no enabled devices were found just use the first one in the list
	if ( midiInputList.getSelectedId() == 0 )
		setMidiInput( 0 );

	addAndMakeVisible( midiInputListLbl );
	midiInputListLbl.setText( "Midi Input Device", juce::dontSendNotification );
	midiInputListLbl.attachToComponent( &midiInputList, true );

	// Make sure you set the size of the component after
	// you add any child components.
	setSize( 800, 600 );

	// TODO this should be done somewhere else
	// upgrade presets if necessary
	ARMor8VoiceState initPreset =
	{
		// operator 1
		1000.0f,
		false,
		OscillatorMode::SINE,
		0.0f,
		2.0f,
		0.0f,
		2.0f,
		1.0f,
		0.0f,
		2.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// operator 2
		1000.0f,
		false,
		OscillatorMode::SINE,
		0.0f,
		2.0f,
		0.0f,
		2.0f,
		1.0f,
		0.0f,
		2.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// operator 3
		1000.0f,
		false,
		OscillatorMode::SINE,
		0.0f,
		2.0f,
		0.0f,
		2.0f,
		1.0f,
		0.0f,
		2.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// operator 4
		1000.0f,
		false,
		OscillatorMode::SINE,
		0.0f,
		2.0f,
		0.0f,
		2.0f,
		1.0f,
		0.0f,
		2.0f,
		false,
		false,
		false,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		20000.0f,
		0.0f,
		0.0f,
		0.0f,
		0,

		// global
		false,
		1,
		0.0f,
		false
	};
	ARMor8PresetUpgrader presetUpgrader( initPreset, armor8VoiceManager.getPresetHeader() );
	presetManager.upgradePresets( &presetUpgrader );

	// UI initialization
	uiSim.draw();

	// start timer for fake loading
	this->startTimer( 33 );

	// grab keyboard focus
	this->setWantsKeyboardFocus( true );
}

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	shutdownAudio();
	delete writer;
	testFile.close();
}

bool MainComponent::keyPressed (const juce::KeyPress& k)
{
	// for holding both buttons down at the same time
	if ( k.getTextCharacter() == 'z' )
	{
		effect1Btn.setState( juce::Button::ButtonState::buttonDown );
		effect2Btn.setState( juce::Button::ButtonState::buttonDown );
	}

	return true;
}

bool MainComponent::keyStateChanged (bool isKeyDown)
{
	if ( ! isKeyDown ) // if a key has been released
	{
		effect1Btn.setState( juce::Button::ButtonState::buttonNormal );
		effect2Btn.setState( juce::Button::ButtonState::buttonNormal );
	}

	return true;
}

void MainComponent::timerCallback()
{
	static unsigned int fakeLoadingCounter = 0;

	if ( fakeLoadingCounter == 100 )
	{
		fakeLoadingCounter++;

		// set preset to first preset
		armor8VoiceManager.setState( presetManager.retrievePreset<ARMor8VoiceState>(0) );

		// force UI to refresh
		// op1Btn.triggerClick();
	}
	else if ( fakeLoadingCounter < 100 )
	{
		uiSim.drawLoadingLogo();
		fakeLoadingCounter++;
	}
	else
	{
		uiSim.tickForChangingBackToStatus();
		uiSim.processEffect1Btn( effect1Btn.isDown() );
		uiSim.processEffect2Btn( effect2Btn.isDown() );

		double effect1Val = effect1Sldr.getValue();
		float effect1Percentage = ( effect1Sldr.getValue() - effect1Sldr.getMinimum() )
						/ ( effect1Sldr.getMaximum() - effect1Sldr.getMinimum() );
		double effect2Val = effect2Sldr.getValue();
		float effect2Percentage = ( effect2Sldr.getValue() - effect2Sldr.getMinimum() )
						/ ( effect2Sldr.getMaximum() - effect2Sldr.getMinimum() );
		double effect3Val = effect3Sldr.getValue();
		float effect3Percentage = ( effect3Sldr.getValue() - effect3Sldr.getMinimum() )
						/ ( effect3Sldr.getMaximum() - effect3Sldr.getMinimum() );

		IPotEventListener::PublishEvent( PotEvent(effect1Percentage, static_cast<unsigned int>(POT_CHANNEL::EFFECT1)) );
		IPotEventListener::PublishEvent( PotEvent(effect2Percentage, static_cast<unsigned int>(POT_CHANNEL::EFFECT2)) );
		IPotEventListener::PublishEvent( PotEvent(effect3Percentage, static_cast<unsigned int>(POT_CHANNEL::EFFECT3)) );
	}
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
	// This function will be called when the audio device is started, or when
	// its settings (i.e. sample rate, block size, etc) are changed.

	// You can use this function to initialise any resources you might need,
	// but be careful - it will be called on the audio thread, not the GUI thread.

	// For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
	// Your audio-processing code goes here!

	// For more details, see the help for AudioProcessor::getNextAudioBlock()

	// Right now we are not producing any data, in which case we need to clear the buffer
	// (to prevent the output of random noise)
	// bufferToFill.clearActiveBufferRegion();
	try
	{
		float* writePtrR = bufferToFill.buffer->getWritePointer( 1 );
		float* writePtrL = bufferToFill.buffer->getWritePointer( 0 );
		const float* readPtrR = bufferToFill.buffer->getReadPointer( 0 );

		for ( int i = 0; i < bufferToFill.numSamples; i++ )
		{
			float value = sAudioBuffer.getNextSample();
			writePtrR[i] = value;
			writePtrL[i] = value;
			// testFile << readPtrR[i] << std::endl;
		}

		sAudioBuffer.pollToFillBuffers();
	}
	catch ( std::exception& e )
	{
		std::cout << "Exception caught in getNextAudioBlock: " << e.what() << std::endl;
	}
}

void MainComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.
	//
	// For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll( getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId) );

	// You can add your drawing code here!
	g.drawImageWithin( screenRep, 0, 120, getWidth(), 120, juce::RectanglePlacement::centred | juce::RectanglePlacement::doNotResize );
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	int sliderLeft = 120;
	effect1Sldr.setBounds     (sliderLeft + (getWidth() / 2), 300, (getWidth() / 2) - (sliderLeft * 2), 20);
	effect2Sldr.setBounds     (sliderLeft + (getWidth() / 2), 340, (getWidth() / 2) - (sliderLeft * 2), 20);
	effect3Sldr.setBounds     (sliderLeft + (getWidth() / 2), 380, (getWidth() / 2) - (sliderLeft * 2), 20);
	effect1Btn.setBounds      (sliderLeft, 300, (getWidth() / 2) - sliderLeft - 10, 20);
	effect2Btn.setBounds      (sliderLeft, 340, (getWidth() / 2) - sliderLeft - 10, 20);
	audioSettingsBtn.setBounds(sliderLeft, 950, getWidth() - sliderLeft - 10, 20);
	midiInputList.setBounds   (sliderLeft, 980, getWidth() - sliderLeft - 10, 20);
}

void MainComponent::sliderValueChanged (juce::Slider* slider)
{
	try
	{
		/*
		double val = slider->getValue();
		float percentage = (slider->getValue() - slider->getMinimum()) / (slider->getMaximum() - slider->getMinimum());

		if (slider == &freqSldr)
		{
			uiSim.processFreqOrDetunePot( percentage );
		}
		else if (slider == &detuneSldr)
		{
			uiSim.processAlt1Btn( true ); // pressed
			uiSim.processAlt1Btn( true ); // held
			uiSim.processFreqOrDetunePot( percentage );
			uiSim.processAlt1Btn( false ); // released
			uiSim.processAlt1Btn( false ); // floating
		}
		else if (slider == &attackSldr)
		{
			uiSim.processAtkOrAtkExpoOrOp1ModPot( percentage );
		}
		else if (slider == &attackExpoSldr)
		{
			uiSim.processAlt1Btn( true ); // pressed
			uiSim.processAlt1Btn( true ); // held
			uiSim.processAtkOrAtkExpoOrOp1ModPot( percentage );
			uiSim.processAlt1Btn( false ); // released
			uiSim.processAlt1Btn( false ); // floating
		}
		else if (slider == &decaySldr)
		{
			uiSim.processDecOrDecExpoOrOp2ModPot( percentage );
		}
		else if (slider == &decayExpoSldr)
		{
			uiSim.processAlt1Btn( true ); // pressed
			uiSim.processAlt1Btn( true ); // held
			uiSim.processDecOrDecExpoOrOp2ModPot( percentage );
			uiSim.processAlt1Btn( false ); // released
			uiSim.processAlt1Btn( false ); // floating
		}
		else if (slider == &sustainSldr)
		{
			uiSim.processSusOrOp3ModPot( percentage );
		}
		else if (slider == &releaseSldr)
		{
			uiSim.processRelOrRelExpoOrOp4ModPot( percentage );
		}
		else if (slider == &releaseExpoSldr)
		{
			uiSim.processAlt1Btn( true ); // pressed
			uiSim.processAlt1Btn( true ); // held
			uiSim.processRelOrRelExpoOrOp4ModPot( percentage );
			uiSim.processAlt1Btn( false ); // released
			uiSim.processAlt1Btn( false ); // floating
		}
		else if (slider == &op1ModAmountSldr)
		{
			uiSim.processAlt2Btn( true ); // pressed
			uiSim.processAlt2Btn( true ); // held
			uiSim.processAtkOrAtkExpoOrOp1ModPot( percentage );
			uiSim.processAlt2Btn( false ); // released
			uiSim.processAlt2Btn( false ); // floating
		}
		else if (slider == &op2ModAmountSldr)
		{
			uiSim.processAlt2Btn( true ); // pressed
			uiSim.processAlt2Btn( true ); // held
			uiSim.processDecOrDecExpoOrOp2ModPot( percentage );
			uiSim.processAlt2Btn( false ); // released
			uiSim.processAlt2Btn( false ); // floating
		}
		else if (slider == &op3ModAmountSldr)
		{
			uiSim.processAlt2Btn( true ); // pressed
			uiSim.processAlt2Btn( true ); // held
			uiSim.processSusOrOp3ModPot( percentage );
			uiSim.processAlt2Btn( false ); // released
			uiSim.processAlt2Btn( false ); // floating
		}
		else if (slider == &op4ModAmountSldr)
		{
			uiSim.processAlt2Btn( true ); // pressed
			uiSim.processAlt2Btn( true ); // held
			uiSim.processRelOrRelExpoOrOp4ModPot( percentage );
			uiSim.processAlt2Btn( false ); // released
			uiSim.processAlt2Btn( false ); // floating
		}
		else if (slider == &amplitudeSldr)
		{
			uiSim.processAmpOrAmpVelPot( percentage );
		}
		else if (slider == &filterFreqSldr)
		{
			uiSim.processFiltFreqOrFiltResOrFiltVelPot( percentage );
		}
		else if (slider == &filterResSldr)
		{
			uiSim.processAlt1Btn( true ); // pressed
			uiSim.processAlt1Btn( true ); // held
			uiSim.processFiltFreqOrFiltResOrFiltVelPot( percentage );
			uiSim.processAlt1Btn( false ); // released
			uiSim.processAlt1Btn( false ); // floating
		}
		else if (slider == &ampVelSldr)
		{
			uiSim.processAlt2Btn( true ); // pressed
			uiSim.processAlt2Btn( true ); // held
			uiSim.processAmpOrAmpVelPot( percentage );
			uiSim.processAlt2Btn( false ); // released
			uiSim.processAlt2Btn( false ); // floating
		}
		else if (slider == &filtVelSldr)
		{
			uiSim.processAlt2Btn( true ); // pressed
			uiSim.processAlt2Btn( true ); // held
			uiSim.processFiltFreqOrFiltResOrFiltVelPot( percentage );
			uiSim.processAlt2Btn( false ); // released
			uiSim.processAlt2Btn( false ); // floating
		}
		else if (slider == &pitchBendSldr)
		{
			uiSim.processAlt1Btn( true ); // pressed
			uiSim.processAlt1Btn( true ); // held
			uiSim.processPitchBendOrGlidePot( percentage );
			uiSim.processAlt1Btn( false ); // released
			uiSim.processAlt1Btn( false ); // floating
		}
		else if (slider == &glideSldr)
		{
			uiSim.processPitchBendOrGlidePot( percentage );
		}
		*/
	}
	catch (std::exception& e)
	{
		std::cout << "Exception caught in slider handler: " << e.what() << std::endl;
	}
}

void MainComponent::buttonClicked (juce::Button* button)
{
	try
	{
		/*
		if (button == &prevPresetBtn)
		{
			uiSim.processPrevPresetBtn( true ); // pressed
			uiSim.processPrevPresetBtn( false ); // released
			uiSim.processPrevPresetBtn( false ); // floating
		}
		else if (button == &nextPresetBtn)
		{
			uiSim.processNextPresetBtn( true ); // pressed
			uiSim.processNextPresetBtn( false ); // released
			uiSim.processNextPresetBtn( false ); // floating
		}
		else if (button == &writePresetBtn)
		{
			uiSim.processWritePresetBtn( true ); // pressed
			uiSim.processWritePresetBtn( false ); // released
			uiSim.processWritePresetBtn( false ); // floating
		}
		*/
	}
	catch (std::exception& e)
	{
		std::cout << "Exception caught in buttonClicked(): " << e.what() << std::endl;
	}
}

void MainComponent::updateToggleState (juce::Button* button)
{
	try
	{
		/*
		bool isPressed = button->getToggleState();

		if (button == &op1Btn)
		{
			if (isPressed)
			{
				unsigned int opToEdit = armor8VoiceManager.getOperatorToEdit() + 1;

				if ( opToEdit == 1 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 2 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 3 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 4 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
			}
		}
		else if (button == &op2Btn)
		{
			if (isPressed)
			{
				unsigned int opToEdit = armor8VoiceManager.getOperatorToEdit() + 1;

				if ( opToEdit == 1 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 2 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 3 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 4 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
			}
		}
		else if (button == &op3Btn)
		{
			if (isPressed)
			{
				unsigned int opToEdit = armor8VoiceManager.getOperatorToEdit() + 1;

				if ( opToEdit == 1 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 2 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 3 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 4 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
			}
		}
		else if (button == &op4Btn)
		{
			if (isPressed)
			{
				unsigned int opToEdit = armor8VoiceManager.getOperatorToEdit() + 1;

				if ( opToEdit == 1 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 2 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 3 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
				else if ( opToEdit == 4 )
				{
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( true ); // pressed
					uiSim.processNextOpBtn( false ); // released
					uiSim.processNextOpBtn( false ); // floating
				}
			}
		}
		else if (button == &sineBtn)
		{
			if (isPressed)
			{
				unsigned int waveNum = armor8VoiceManager.getCurrentWaveNum() + 1;

				if ( waveNum == 1 ) // SINE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 2 ) // TRIANGLE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 3 ) // SQUARE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 4 ) // SAWTOOTH
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
			}
		}
		else if (button == &triangleBtn)
		{
			if (isPressed)
			{
				unsigned int waveNum = armor8VoiceManager.getCurrentWaveNum() + 1;

				if ( waveNum == 1 ) // SINE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 2 ) // TRIANGLE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 3 ) // SQUARE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 4 ) // SAWTOOTH
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
			}
		}
		else if (button == &squareBtn)
		{
			if (isPressed)
			{
				unsigned int waveNum = armor8VoiceManager.getCurrentWaveNum() + 1;

				if ( waveNum == 1 ) // SINE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 2 ) // TRIANGLE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 3 ) // SQUARE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 4 ) // SAWTOOTH
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
			}
		}
		else if (button == &sawBtn)
		{
			if (isPressed)
			{
				unsigned int waveNum = armor8VoiceManager.getCurrentWaveNum() + 1;

				if ( waveNum == 1 ) // SINE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 2 ) // TRIANGLE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 3 ) // SQUARE
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
				else if ( waveNum == 4 ) // SAWTOOTH
				{
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( true ); // pressed
					uiSim.processNextWaveBtn( false ); // released
					uiSim.processNextWaveBtn( false ); // floating
				}
			}
		}
		else if (button == &amplitudeDestBtn)
		{
			if ( isPressed )
			{
				uiSim.setEGDestAmplitude( true );
			}
			else
			{
				uiSim.setEGDestAmplitude( false );
			}
		}
		else if (button == &frequencyDestBtn)
		{
			if ( isPressed )
			{
				uiSim.setEGDestFrequency( true );
			}
			else
			{
				uiSim.setEGDestFrequency( false );
			}
		}
		else if (button == &filterDestBtn)
		{
			if ( isPressed )
			{
				uiSim.setEGDestFiltrFreq( true );
			}
			else
			{
				uiSim.setEGDestFiltrFreq( false );
			}
		}
		else if (button == &ratioBtn)
		{
			uiSim.processRatioOrFixedBtn( true ); // pressed
			uiSim.processRatioOrFixedBtn( false ); // released
			uiSim.processRatioOrFixedBtn( false ); // floating
		}
		else if (button == &monoBtn)
		{
			uiSim.processMonoBtn( true ); // pressed
			uiSim.processMonoBtn( false ); // released
			uiSim.processMonoBtn( false ); // floating
		}
		else if (button == &egRetriggerBtn)
		{
			uiSim.processGlideRetrigBtn( true ); // pressed
			uiSim.processGlideRetrigBtn( false ); // released
			uiSim.processGlideRetrigBtn( false ); // floating
		}
		*/
	}
	catch (std::exception& e)
	{
		std::cout << "Exception in toggle button shit: " << e.what() << std::endl;
	}
}

void MainComponent::onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent)
{
	this->setFromARMor8VoiceState( presetEvent.getPreset(), presetEvent.getOpToEdit(), presetEvent.getPresetNum() );
}

void MainComponent::onARMor8LCDRefreshEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent)
{
	this->copyFrameBufferToImage( lcdRefreshEvent.getXStart(), lcdRefreshEvent.getYStart(),
					lcdRefreshEvent.getXEnd(), lcdRefreshEvent.getYEnd() );
	this->repaint();
}

void MainComponent::setFromARMor8VoiceState (const ARMor8VoiceState& state, unsigned int opToEdit, unsigned int presetNum)
{
	try
	{
		/*
		// set preset num label
		juce::String presetNumStr( presetNum + 1 );
		presetNumLbl.setText( presetNumStr, juce::dontSendNotification );

		// handle global states first
		pitchBendSldr.setValue( state.pitchBendSemitones, juce::dontSendNotification );
		glideSldr.setValue( state.glideTime, juce::dontSendNotification );
		if ( !egRetriggerBtn.getToggleState() )
		{
			if ( state.glideRetrigger )
			{
				egRetriggerBtn.setToggleState( true, juce::dontSendNotification );
			}
		}
		else
		{
			if ( !state.glideRetrigger )
			{
				egRetriggerBtn.setToggleState( false, juce::dontSendNotification );
			}
		}
		if ( !monoBtn.getToggleState() )
		{
			if ( state.monophonic )
			{
				monoBtn.setToggleState( true, juce::dontSendNotification );
			}
		}
		else
		{
			if ( !state.monophonic )
			{
				monoBtn.setToggleState( false, juce::dontSendNotification );
			}
		}

		switch ( opToEdit )
		{
			case 0:
				op1Btn.setToggleState( true, juce::dontSendNotification );

				freqSldr.setValue( state.frequency1, juce::dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio1 )
					{
						ratioBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio1 )
					{
						ratioBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				switch ( state.wave1 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, juce::dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack1, juce::dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo1, juce::dontSendNotification );
				decaySldr.setValue( state.decay1, juce::dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo1, juce::dontSendNotification );
				sustainSldr.setValue( state.sustain1, juce::dontSendNotification );
				releaseSldr.setValue( state.release1, juce::dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo1, juce::dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod1 )
					{
						amplitudeDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod1 )
					{
						amplitudeDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if (!state.egFrequencyMod1)
					{
						frequencyDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod1 )
					{
						frequencyDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod1 )
					{
						filterDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod1 )
					{
						filterDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount1, juce::dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount1, juce::dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount1, juce::dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount1, juce::dontSendNotification );
				amplitudeSldr.setValue( state.amplitude1, juce::dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq1, juce::dontSendNotification );
				filterResSldr.setValue( state.filterRes1, juce::dontSendNotification );
				ampVelSldr.setValue( state.ampVelSens1, juce::dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens1, juce::dontSendNotification );
				detuneSldr.setValue( state.detune1, juce::dontSendNotification );

				break;
			case 1:
				op2Btn.setToggleState( true, juce::dontSendNotification );

				freqSldr.setValue( state.frequency2, juce::dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio2 )
					{
						ratioBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio2 )
					{
						ratioBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				switch ( state.wave2 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, juce::dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack2, juce::dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo2, juce::dontSendNotification );
				decaySldr.setValue( state.decay2, juce::dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo2, juce::dontSendNotification );
				sustainSldr.setValue( state.sustain2, juce::dontSendNotification );
				releaseSldr.setValue( state.release2, juce::dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo2, juce::dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod2 )
					{
						amplitudeDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod2 )
					{
						amplitudeDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if ( !state.egFrequencyMod2 )
					{
						frequencyDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod2 )
					{
						frequencyDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod2 )
					{
						filterDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod2 )
					{
						filterDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount2, juce::dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount2, juce::dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount2, juce::dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount2, juce::dontSendNotification );
				amplitudeSldr.setValue( state.amplitude2, juce::dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq2, juce::dontSendNotification);
				filterResSldr.setValue( state.filterRes2, juce::dontSendNotification);
				ampVelSldr.setValue( state.ampVelSens2, juce::dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens2, juce::dontSendNotification );
				detuneSldr.setValue( state.detune2, juce::dontSendNotification );

				break;
			case 2:
				op3Btn.setToggleState( true, juce::dontSendNotification );

				freqSldr.setValue( state.frequency3, juce::dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio3 )
					{
						ratioBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio3 )
					{
						ratioBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				switch ( state.wave3 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, juce::dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack3, juce::dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo3, juce::dontSendNotification );
				decaySldr.setValue( state.decay3, juce::dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo3, juce::dontSendNotification );
				sustainSldr.setValue( state.sustain3, juce::dontSendNotification );
				releaseSldr.setValue( state.release3, juce::dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo3, juce::dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod3 )
					{
						amplitudeDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod3 )
					{
						amplitudeDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if ( !state.egFrequencyMod3 )
					{
						frequencyDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod3 )
					{
						frequencyDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod3 )
					{
						filterDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod3 )
					{
						filterDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount3, juce::dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount3, juce::dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount3, juce::dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount3, juce::dontSendNotification );
				amplitudeSldr.setValue( state.amplitude3, juce::dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq3, juce::dontSendNotification );
				filterResSldr.setValue( state.filterRes3, juce::dontSendNotification );
				ampVelSldr.setValue( state.ampVelSens3, juce::dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens3, juce::dontSendNotification );
				detuneSldr.setValue( state.detune3, juce::dontSendNotification );

				break;
			case 3:
				op4Btn.setToggleState( true, juce::dontSendNotification );

				freqSldr.setValue( state.frequency4, juce::dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio4 )
					{
						ratioBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio4 )
					{
						ratioBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				switch ( state.wave4 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, juce::dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, juce::dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack4, juce::dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo4, juce::dontSendNotification );
				decaySldr.setValue( state.decay4, juce::dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo4, juce::dontSendNotification );
				sustainSldr.setValue( state.sustain4, juce::dontSendNotification );
				releaseSldr.setValue( state.release4, juce::dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo4, juce::dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod4 )
					{
						amplitudeDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod4 )
					{
						amplitudeDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if ( !state.egFrequencyMod4 )
					{
						frequencyDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod4 )
					{
						frequencyDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod4 )
					{
						filterDestBtn.setToggleState( false, juce::dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod4 )
					{
						filterDestBtn.setToggleState( true, juce::dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount4, juce::dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount4, juce::dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount4, juce::dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount4, juce::dontSendNotification );
				amplitudeSldr.setValue( state.amplitude4, juce::dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq4, juce::dontSendNotification );
				filterResSldr.setValue( state.filterRes4, juce::dontSendNotification );
				ampVelSldr.setValue( state.ampVelSens4, juce::dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens4, juce::dontSendNotification );
				detuneSldr.setValue( state.detune4, juce::dontSendNotification );

				break;
			default:
				std::cout << "Something is terribly, terribly wrong in setFromARMor8VoiceState..." << std::endl;
		}
		*/
	}
	catch (std::exception& e)
	{
		std::cout << "Caught an exception in setFromState!!: " << e.what() << std::endl;
	}
}

void MainComponent::copyFrameBufferToImage (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd)
{
	ColorProfile* colorProfile = uiSim.getColorProfile();
	FrameBuffer* frameBuffer = uiSim.getFrameBuffer();
	unsigned int frameBufferWidth = frameBuffer->getWidth();

	for ( unsigned int pixelY = yStart; pixelY < yEnd + 1; pixelY++ )
	{
		for ( unsigned int pixelX = xStart; pixelX < xEnd + 1; pixelX++ )
		{
			if ( ! colorProfile->getPixel(frameBuffer->getPixels(), frameBuffer->getWidth() * frameBuffer->getHeight(),
						(pixelY * frameBufferWidth) + pixelX).m_M )
			{
				screenRep.setPixelAt( (pixelX * 2),     (pixelY * 2),     juce::Colour(0, 0, 0) );
				screenRep.setPixelAt( (pixelX * 2) + 1, (pixelY * 2),     juce::Colour(0, 0, 0) );
				screenRep.setPixelAt( (pixelX * 2),     (pixelY * 2) + 1, juce::Colour(0, 0, 0) );
				screenRep.setPixelAt( (pixelX * 2) + 1, (pixelY * 2) + 1, juce::Colour(0, 0, 0) );
			}
			else
			{
				screenRep.setPixelAt( (pixelX * 2),     (pixelY * 2),     juce::Colour(0, 97, 252) );
				screenRep.setPixelAt( (pixelX * 2) + 1, (pixelY * 2),     juce::Colour(0, 97, 252) );
				screenRep.setPixelAt( (pixelX * 2),     (pixelY * 2) + 1, juce::Colour(0, 97, 252) );
				screenRep.setPixelAt( (pixelX * 2) + 1, (pixelY * 2) + 1, juce::Colour(0, 97, 252) );
			}
		}
	}
}

void MainComponent::setMidiInput (int index)
{
	auto list = juce::MidiInput::getDevices();

	deviceManager.removeMidiInputCallback( list[lastInputIndex], this );

	auto newInput = list[index];

	if ( !deviceManager.isMidiInputEnabled(newInput) )
		deviceManager.setMidiInputEnabled( newInput, true );

	deviceManager.addMidiInputCallback( newInput, this );
	midiInputList.setSelectedId ( index + 1, juce::dontSendNotification );

	lastInputIndex = index;
}

void MainComponent::handleIncomingMidiMessage (juce::MidiInput *source, const juce::MidiMessage &message)
{
	for ( int byte = 0; byte < message.getRawDataSize(); byte++ )
	{
		midiHandler.processByte( message.getRawData()[byte] );
	}

	midiHandler.dispatchEvents();
}
