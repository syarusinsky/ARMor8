/*
   ==============================================================================

   This file was auto-generated!

   ==============================================================================
   */

#include "MainComponent.h"

#include "CPPFile.hpp"
#include "ARMor8PresetUpgrader.hpp"

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
	freqSldr(),
	freqLbl(),
	detuneSldr(),
	detuneLbl(),
	ratioBtn( "Ratio" ),
	editLbl(),
	op1Btn( "Op 1" ),
	op2Btn( "Op 2" ),
	op3Btn( "Op 3" ),
	op4Btn( "Op 4" ),
	waveLbl(),
	sineBtn( "Sine" ),
	triangleBtn( "Triangle" ),
	squareBtn( "Square" ),
	sawBtn( "Saw" ),
	attackSldr(),
	attackLbl(),
	attackExpoSldr(),
	attackExpoLbl(),
	decaySldr(),
	decayLbl(),
	decayExpoSldr(),
	decayExpoLbl(),
	sustainSldr(),
	sustainLbl(),
	releaseSldr(),
	releaseLbl(),
	releaseExpoSldr(),
	releaseExpoLbl(),
	egDestLbl(),
	amplitudeDestBtn( "Amplitude" ),
	frequencyDestBtn( "Frequency" ),
	filterDestBtn( "Filter" ),
	amplitudeSldr(),
	amplitudeLbl(),
	filterFreqSldr(),
	filterFreqLbl(),
	filterResSldr(),
	filterResLbl(),
	ampVelSldr(),
	ampVelLbl(),
	filtVelSldr(),
	filtVelLbl(),
	pitchBendSldr(),
	pitchBendLbl(),
	glideSldr(),
	glideLbl(),
	egRetriggerBtn( "Glide Retrigger" ),
	audioSettingsBtn( "Audio Settings" ),
	midiInputList(),
	midiInputListLbl(),
	monoBtn( "Global: Monophonic" ),
	prevPresetBtn( "Prev Preset" ),
	presetNumLbl( "Preset Number", "1" ),
	nextPresetBtn( "Next Preset" ),
	writePresetBtn( "Write Preset" ),
	audioSettingsComponent( deviceManager, 2, 2, &audioSettingsBtn ),
	screenRep( Image::RGB, 256, 128, true ) // this is actually double the size so we can actually see it
{
	// connecting to event system
	this->bindToARMor8PresetEventSystem();
	armor8VoiceManager.bindToKeyEventSystem();
	armor8VoiceManager.bindToPitchEventSystem();
	armor8VoiceManager.bindToPotEventSystem();
	armor8VoiceManager.bindToButtonEventSystem();

	// this is to avoid unwanted audio on startup
	armor8VoiceManager.setOperatorAmplitude(0, 0.0f);
	armor8VoiceManager.setOperatorAmplitude(1, 0.0f);
	armor8VoiceManager.setOperatorAmplitude(2, 0.0f);
	armor8VoiceManager.setOperatorAmplitude(3, 0.0f);

	// Some platforms require permissions to open input channels so request that here
	if ( RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
			&& ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio) )
	{
		RuntimePermissions::request( RuntimePermissions::recordAudio,
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
	AudioDeviceManager::AudioDeviceSetup deviceSetup = AudioDeviceManager::AudioDeviceSetup();
	deviceSetup.sampleRate = 44100;
	deviceManager.initialise( 2, 2, 0, true, String(), &deviceSetup );

	// basic juce logging
	Logger* log = Logger::getCurrentLogger();
	int sampleRate = deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();
	log->writeToLog( String(sampleRate) );
	log->writeToLog( String(deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples()) );

	// optionally we can write wav files for debugging
	// WavAudioFormat wav;
	// File tempFile( "TestAudio.wav" );
	// OutputStream* outStream( tempFile.createOutputStream() );
	// writer = wav.createWriterFor( outStream, sampleRate, 2, wav.getPossibleBitDepths().getLast(), NULL, 0 );

	// log->writeToLog( String(wav.getPossibleBitDepths().getLast()) );
	// log->writeToLog( tempFile.getFullPathName() );

	// this file can also be used for debugging
	testFile.open( "JuceMainComponentOutput.txt" );

	// adding all child components
	addAndMakeVisible( freqSldr );
	freqSldr.setRange( armor8VoiceManager.FREQUENCY_MIN, armor8VoiceManager.FREQUENCY_MAX );
	freqSldr.setTextValueSuffix( "Hz" );
	freqSldr.setSkewFactorFromMidPoint( 500 );
	freqSldr.addListener( this );
	addAndMakeVisible( freqLbl );
	freqLbl.setText( "Frequency", dontSendNotification );
	freqLbl.attachToComponent( &freqSldr, true );

	addAndMakeVisible( detuneSldr );
	detuneSldr.setRange( armor8VoiceManager.DETUNE_MIN, armor8VoiceManager.DETUNE_MAX, 1 );
	detuneSldr.setTextValueSuffix( "Cents" );
	detuneSldr.addListener( this );
	addAndMakeVisible( detuneLbl );
	detuneLbl.setText( "Detune", dontSendNotification );
	detuneLbl.attachToComponent( &detuneSldr, true );

	addAndMakeVisible( ratioBtn );
	ratioBtn.onClick = [this] { updateToggleState(&ratioBtn); };
	ratioBtn.addListener( this );

	addAndMakeVisible( editLbl );
	editLbl.setText( "Editing:", dontSendNotification );

	addAndMakeVisible( op1Btn );
	op1Btn.setRadioGroupId( OpRadioId );
	op1Btn.onClick = [this] { updateToggleState(&op1Btn); };
	addAndMakeVisible( op2Btn );
	op2Btn.setRadioGroupId( OpRadioId );
	op2Btn.onClick = [this] { updateToggleState(&op2Btn); };
	addAndMakeVisible( op3Btn );
	op3Btn.setRadioGroupId( OpRadioId );
	op3Btn.onClick = [this] { updateToggleState(&op3Btn); };
	addAndMakeVisible( op4Btn );
	op4Btn.setRadioGroupId( OpRadioId );
	op4Btn.onClick = [this] { updateToggleState(&op4Btn); };

	addAndMakeVisible( waveLbl );
	waveLbl.setText( "Waveform:", dontSendNotification );

	addAndMakeVisible( sineBtn );
	sineBtn.setRadioGroupId( WaveRadioId );
	sineBtn.onClick = [this] { updateToggleState(&sineBtn); };
	addAndMakeVisible( triangleBtn );
	triangleBtn.setRadioGroupId( WaveRadioId );
	triangleBtn.onClick = [this] { updateToggleState(&triangleBtn); };
	addAndMakeVisible( squareBtn );
	squareBtn.setRadioGroupId( WaveRadioId );
	squareBtn.onClick = [this] { updateToggleState(&squareBtn); };
	addAndMakeVisible( sawBtn );
	sawBtn.setRadioGroupId( WaveRadioId );
	sawBtn.onClick = [this] { updateToggleState(&sawBtn); };

	addAndMakeVisible( attackSldr );
	attackSldr.setRange( armor8VoiceManager.ATTACK_MIN, armor8VoiceManager.ATTACK_MAX );
	attackSldr.setTextValueSuffix( "Seconds" );
	attackSldr.addListener( this );
	addAndMakeVisible( attackLbl );
	attackLbl.setText( "Attack", dontSendNotification );
	attackLbl.attachToComponent( &attackSldr, true );

	addAndMakeVisible( attackExpoSldr );
	attackExpoSldr.setRange( armor8VoiceManager.EXPO_MIN, armor8VoiceManager.EXPO_MAX );
	attackExpoSldr.setTextValueSuffix( "%" );
	attackExpoSldr.addListener( this );
	addAndMakeVisible( attackExpoLbl );
	attackExpoLbl.setText( "Attack Expo Amount", dontSendNotification );
	attackExpoLbl.attachToComponent( &attackExpoSldr, true );

	addAndMakeVisible( decaySldr );
	decaySldr.setRange( armor8VoiceManager.DECAY_MIN, armor8VoiceManager.DECAY_MAX );
	decaySldr.setTextValueSuffix( "Seconds" );
	decaySldr.addListener( this );
	addAndMakeVisible( decayLbl );
	decayLbl.setText( "Decay", dontSendNotification );
	decayLbl.attachToComponent( &decaySldr, true );

	addAndMakeVisible( decayExpoSldr );
	decayExpoSldr.setRange( armor8VoiceManager.EXPO_MIN, armor8VoiceManager.EXPO_MAX );
	decayExpoSldr.setTextValueSuffix( "%" );
	decayExpoSldr.addListener( this );
	addAndMakeVisible( decayExpoLbl );
	decayExpoLbl.setText( "Decay Expo Amount", dontSendNotification );
	decayExpoLbl.attachToComponent( &decayExpoSldr, true );

	addAndMakeVisible( sustainSldr );
	sustainSldr.setRange( armor8VoiceManager.SUSTAIN_MIN, armor8VoiceManager.SUSTAIN_MAX );
	sustainSldr.setTextValueSuffix( "%" );
	sustainSldr.addListener( this );
	addAndMakeVisible( sustainLbl );
	sustainLbl.setText( "Sustain", dontSendNotification );
	sustainLbl.attachToComponent( &sustainSldr, true );

	addAndMakeVisible( releaseSldr );
	releaseSldr.setRange( armor8VoiceManager.RELEASE_MIN, armor8VoiceManager.RELEASE_MAX );
	releaseSldr.setTextValueSuffix( "Seconds" );
	releaseSldr.addListener( this );
	addAndMakeVisible( releaseLbl );
	releaseLbl.setText( "Release", dontSendNotification );
	releaseLbl.attachToComponent( &releaseSldr, true );

	addAndMakeVisible( releaseExpoSldr );
	releaseExpoSldr.setRange( armor8VoiceManager.EXPO_MIN, armor8VoiceManager.EXPO_MAX );
	releaseExpoSldr.setTextValueSuffix( "%" );
	releaseExpoSldr.addListener( this );
	addAndMakeVisible( releaseExpoLbl );
	releaseExpoLbl.setText( "Release Expo Amount", dontSendNotification );
	releaseExpoLbl.attachToComponent( &releaseExpoSldr, true );

	addAndMakeVisible( egDestLbl );
	egDestLbl.setText( "EG Destinations:", dontSendNotification );

	addAndMakeVisible( amplitudeDestBtn );
	amplitudeDestBtn.onClick = [this] { updateToggleState(&amplitudeDestBtn); };
	addAndMakeVisible( frequencyDestBtn );
	frequencyDestBtn.onClick = [this] { updateToggleState(&frequencyDestBtn); };
	addAndMakeVisible( filterDestBtn );
	filterDestBtn.onClick = [this] { updateToggleState(&filterDestBtn); };

	addAndMakeVisible( op1ModAmountSldr );
	op1ModAmountSldr.setRange( armor8VoiceManager.OP_MOD_MIN, armor8VoiceManager.OP_MOD_MAX );
	op1ModAmountSldr.addListener( this );
	addAndMakeVisible( op1ModAmountLbl );
	op1ModAmountLbl.setText( "Op1 Mod Amount", dontSendNotification );
	op1ModAmountLbl.attachToComponent( &op1ModAmountSldr, true );

	addAndMakeVisible( op2ModAmountSldr );
	op2ModAmountSldr.setRange( armor8VoiceManager.OP_MOD_MIN, armor8VoiceManager.OP_MOD_MAX );
	op2ModAmountSldr.addListener( this );
	addAndMakeVisible( op2ModAmountLbl );
	op2ModAmountLbl.setText( "Op2 Mod Amount", dontSendNotification );
	op2ModAmountLbl.attachToComponent( &op2ModAmountSldr, true );

	addAndMakeVisible( op3ModAmountSldr );
	op3ModAmountSldr.setRange( armor8VoiceManager.OP_MOD_MIN, armor8VoiceManager.OP_MOD_MAX );
	op3ModAmountSldr.addListener( this );
	addAndMakeVisible( op3ModAmountLbl );
	op3ModAmountLbl.setText( "Op3 Mod Amount", dontSendNotification );
	op3ModAmountLbl.attachToComponent( &op3ModAmountSldr, true );

	addAndMakeVisible( op4ModAmountSldr );
	op4ModAmountSldr.setRange( armor8VoiceManager.OP_MOD_MIN, armor8VoiceManager.OP_MOD_MAX );
	op4ModAmountSldr.addListener( this );
	addAndMakeVisible( op4ModAmountLbl );
	op4ModAmountLbl.setText( "Op4 Mod Amount", dontSendNotification );
	op4ModAmountLbl.attachToComponent( &op4ModAmountSldr, true );

	addAndMakeVisible( amplitudeSldr );
	amplitudeSldr.setRange( armor8VoiceManager.AMPLITUDE_MIN, armor8VoiceManager.AMPLITUDE_MAX );
	amplitudeSldr.addListener( this );
	addAndMakeVisible( amplitudeLbl );
	amplitudeLbl.setText( "Amplitude", dontSendNotification );
	amplitudeLbl.attachToComponent( &amplitudeSldr, true );

	addAndMakeVisible( filterFreqSldr );
	filterFreqSldr.setRange( armor8VoiceManager.FILT_FREQ_MIN, armor8VoiceManager.FILT_FREQ_MAX );
	filterFreqSldr.setSkewFactorFromMidPoint( 500 );
	filterFreqSldr.addListener( this );
	addAndMakeVisible( filterFreqLbl );
	filterFreqLbl.setText( "Filter Freq", dontSendNotification );
	filterFreqLbl.attachToComponent( &filterFreqSldr, true );

	addAndMakeVisible( filterResSldr );
	filterResSldr.setRange( armor8VoiceManager.FILT_RES_MIN, armor8VoiceManager.FILT_RES_MAX );
	filterResSldr.addListener( this );
	addAndMakeVisible( filterResLbl );
	filterResLbl.setText( "Filter Res", dontSendNotification );
	filterResLbl.attachToComponent( &filterResSldr, true );

	addAndMakeVisible( ampVelSldr );
	ampVelSldr.setRange( armor8VoiceManager.VELOCITY_MIN, armor8VoiceManager.VELOCITY_MAX );
	ampVelSldr.addListener( this );
	addAndMakeVisible( ampVelLbl );
	ampVelLbl.setText( "Vel Sens Amplitude", dontSendNotification );
	ampVelLbl.attachToComponent( &ampVelSldr, true );

	addAndMakeVisible( filtVelSldr );
	filtVelSldr.setRange( armor8VoiceManager.VELOCITY_MIN, armor8VoiceManager.VELOCITY_MAX );
	filtVelSldr.addListener( this );
	addAndMakeVisible( filtVelLbl );
	filtVelLbl.setText( "Vel Sens Filter", dontSendNotification );
	filtVelLbl.attachToComponent( &filtVelSldr, true );

	addAndMakeVisible( pitchBendSldr );
	pitchBendSldr.setRange( armor8VoiceManager.PITCH_BEND_MIN, armor8VoiceManager.PITCH_BEND_MAX, 1);
	pitchBendSldr.addListener( this );
	addAndMakeVisible( pitchBendLbl );
	pitchBendLbl.setText( "Pitch Bend", dontSendNotification );
	pitchBendLbl.attachToComponent( &pitchBendSldr, true );

	addAndMakeVisible( glideSldr );
	glideSldr.setRange( armor8VoiceManager.GLIDE_TIME_MIN, armor8VoiceManager.GLIDE_TIME_MAX );
	glideSldr.addListener( this );
	addAndMakeVisible( glideLbl );
	glideLbl.setText( "Glide Time", dontSendNotification );
	glideLbl.attachToComponent( &glideSldr, true );

	addAndMakeVisible( egRetriggerBtn );
	egRetriggerBtn.onClick = [this] { updateToggleState(&egRetriggerBtn); };

	addAndMakeVisible( audioSettingsBtn );
	audioSettingsBtn.addListener( this );

	addAndMakeVisible( midiInputList );
	midiInputList.setTextWhenNoChoicesAvailable( "No MIDI Inputs Enabled" );
	auto midiInputs = MidiInput::getDevices();
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
	midiInputListLbl.setText( "Midi Input Device", dontSendNotification );
	midiInputListLbl.attachToComponent( &midiInputList, true );

	addAndMakeVisible( monoBtn );
	monoBtn.onClick = [this] { updateToggleState(&monoBtn); };
	monoBtn.addListener( this );

	addAndMakeVisible( prevPresetBtn );
	prevPresetBtn.addListener( this );

	addAndMakeVisible( presetNumLbl );

	addAndMakeVisible( nextPresetBtn );
	nextPresetBtn.addListener( this );

	addAndMakeVisible( writePresetBtn );
	writePresetBtn.addListener( this );

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

	// set preset to first preset
	armor8VoiceManager.setState( presetManager.retrievePreset<ARMor8VoiceState>(0) );

	// force UI to refresh
	op1Btn.triggerClick();
}

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	shutdownAudio();
	delete writer;
	testFile.close();
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

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
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
	}
	catch ( std::exception& e )
	{
		std::cout << "Exception caught in getNextAudioBlock: " << e.what() << std::endl;
	}

	// TODO remove this, just for testing
	this->copyFrameBufferToImage();
}

void MainComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.
	//
	// For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll( getLookAndFeel().findColour (ResizableWindow::backgroundColourId) );

	// You can add your drawing code here!
	g.drawImageWithin( screenRep, 0, 120, getWidth(), 120, RectanglePlacement::centred | RectanglePlacement::doNotResize );
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	int sliderLeft = 120;
	freqSldr.setBounds        (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
	detuneSldr.setBounds      (sliderLeft, 60, getWidth() - sliderLeft - 10, 20);
	ratioBtn.setBounds        (sliderLeft, 90, getWidth() - sliderLeft - 10, 20);
	editLbl.setBounds         (sliderLeft, 120, (getWidth() / 3) - sliderLeft, 20);
	waveLbl.setBounds         ((getWidth() / 3) * 2, 120, (getWidth() / 3) - sliderLeft, 20);
	op1Btn.setBounds          (sliderLeft, 150, (getWidth() / 2) - sliderLeft, 20);
	sineBtn.setBounds         ((getWidth() / 3) * 2, 150, (getWidth() / 3) - sliderLeft, 20);
	op2Btn.setBounds          (sliderLeft, 180, (getWidth() / 2) - sliderLeft, 20);
	triangleBtn.setBounds     ((getWidth() / 3) * 2, 180, (getWidth() / 3) - sliderLeft, 20);
	op3Btn.setBounds          (sliderLeft, 210, (getWidth() / 2) - sliderLeft, 20);
	squareBtn.setBounds       ((getWidth() / 3) * 2, 210, (getWidth() / 3) - sliderLeft, 20);
	op4Btn.setBounds          (sliderLeft, 240, (getWidth() / 2) - sliderLeft, 20);
	sawBtn.setBounds          ((getWidth() / 3) * 2, 240, (getWidth() / 3) - sliderLeft, 20);
	attackSldr.setBounds      (sliderLeft, 280, getWidth() - sliderLeft - 10, 20);
	attackExpoSldr.setBounds  (sliderLeft, 310, getWidth() - sliderLeft - 10, 20);
	decaySldr.setBounds       (sliderLeft, 340, getWidth() - sliderLeft - 10, 20);
	decayExpoSldr.setBounds   (sliderLeft, 370, getWidth() - sliderLeft - 10, 20);
	sustainSldr.setBounds     (sliderLeft, 400, getWidth() - sliderLeft - 10, 20);
	releaseSldr.setBounds     (sliderLeft, 430, getWidth() - sliderLeft - 10, 20);
	releaseExpoSldr.setBounds (sliderLeft, 460, getWidth() - sliderLeft - 10, 20);
	egDestLbl.setBounds       (sliderLeft, 490, getWidth() - sliderLeft - 10, 20);
	amplitudeDestBtn.setBounds(sliderLeft, 520, getWidth() - sliderLeft - 10, 20);
	frequencyDestBtn.setBounds(sliderLeft, 550, getWidth() - sliderLeft - 10, 20);
	filterDestBtn.setBounds   (sliderLeft, 580, getWidth() - sliderLeft - 10, 20);
	op1ModAmountSldr.setBounds(sliderLeft, 620, getWidth() - sliderLeft - 10, 20);
	op2ModAmountSldr.setBounds(sliderLeft, 650, getWidth() - sliderLeft - 10, 20);
	op3ModAmountSldr.setBounds(sliderLeft, 680, getWidth() - sliderLeft - 10, 20);
	op4ModAmountSldr.setBounds(sliderLeft, 710, getWidth() - sliderLeft - 10, 20);
	amplitudeSldr.setBounds   (sliderLeft, 740, getWidth() - sliderLeft - 10, 20);
	filterFreqSldr.setBounds  (sliderLeft, 770, getWidth() - sliderLeft - 10, 20);
	filterResSldr.setBounds   (sliderLeft, 800, getWidth() - sliderLeft - 10, 20);
	ampVelSldr.setBounds      (sliderLeft, 830, getWidth() - sliderLeft - 10, 20);
	filtVelSldr.setBounds     (sliderLeft, 860, getWidth() - sliderLeft - 10, 20);
	pitchBendSldr.setBounds   (sliderLeft, 890, getWidth() - sliderLeft - 10, 20);
	glideSldr.setBounds       (sliderLeft + (getWidth() / 5) * 0, 920, (getWidth() / 5) * 4 - sliderLeft - 10, 20);
	egRetriggerBtn.setBounds  (sliderLeft + (getWidth() / 5) * 4, 920, (getWidth() / 5) * 1 - sliderLeft - 10, 20);
	audioSettingsBtn.setBounds(sliderLeft, 950, getWidth() - sliderLeft - 10, 20);
	midiInputList.setBounds   (sliderLeft, 980, getWidth() - sliderLeft - 10, 20);
	monoBtn.setBounds         (sliderLeft + (getWidth() / 5) * 0, 1010, ((getWidth() - sliderLeft - 10) / 5), 20);
	prevPresetBtn.setBounds   (sliderLeft + (getWidth() / 5) * 1, 1010, ((getWidth() - sliderLeft - 10) / 5), 20);
	presetNumLbl.setBounds    (sliderLeft + (getWidth() / 5) * 2, 1010, ((getWidth() - sliderLeft - 10) / 5), 20);
	nextPresetBtn.setBounds   ((getWidth() / 5) * 3, 1010, ((getWidth() - sliderLeft - 10) / 5), 20);
	writePresetBtn.setBounds  ((getWidth() / 5) * 4, 1010, ((getWidth() - sliderLeft - 10) / 5), 20);
}

void MainComponent::sliderValueChanged (Slider* slider)
{
	try
	{
		double val = slider->getValue();
		float percentage = (slider->getValue() - slider->getMinimum()) / (slider->getMaximum() - slider->getMinimum());

		if (slider == &freqSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::FREQUENCY)) );
		}
		else if (slider == &detuneSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::DETUNE)) );
		}
		else if (slider == &attackSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::ATTACK)) );
		}
		else if (slider == &attackExpoSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::ATTACK_EXPO)) );
		}
		else if (slider == &decaySldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::DECAY)) );
		}
		else if (slider == &decayExpoSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::DECAY_EXPO)) );
		}
		else if (slider == &sustainSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::SUSTAIN)) );
		}
		else if (slider == &releaseSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::RELEASE)) );
		}
		else if (slider == &releaseExpoSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::RELEASE_EXPO)) );
		}
		else if (slider == &op1ModAmountSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::OP1_MOD_AMT)) );
		}
		else if (slider == &op2ModAmountSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::OP2_MOD_AMT)) );
		}
		else if (slider == &op3ModAmountSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::OP3_MOD_AMT)) );
		}
		else if (slider == &op4ModAmountSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::OP4_MOD_AMT)) );
		}
		else if (slider == &amplitudeSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::AMPLITUDE)) );
		}
		else if (slider == &filterFreqSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::FILT_FREQ)) );
		}
		else if (slider == &filterResSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::FILT_RES)) );
		}
		else if (slider == &ampVelSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::VEL_AMP)) );
		}
		else if (slider == &filtVelSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::VEL_FILT)) );
		}
		else if (slider == &pitchBendSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::PITCH_BEND)) );
		}
		else if (slider == &glideSldr)
		{
			IPotEventListener::PublishEvent( PotEvent(percentage,
						static_cast<unsigned int>(POT_CHANNEL::GLIDE_TIME)) );
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception caught in slider handler: " << e.what() << std::endl;
	}
}

void MainComponent::buttonClicked (Button* button)
{
	try
	{
		if (button == &prevPresetBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::PREV_PRESET)) );
			op1Btn.triggerClick();
		}
		else if (button == &nextPresetBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::NEXT_PRESET)) );
			op1Btn.triggerClick();
		}
		else if (button == &writePresetBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::WRITE_PRESET)) );
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception caught in buttonClicked(): " << e.what() << std::endl;
	}
}

void MainComponent::updateToggleState (Button* button)
{
	try
	{
		bool isPressed = button->getToggleState();

		if (button == &op1Btn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::OP1)) );
			}
		}
		else if (button == &op2Btn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::OP2)) );
			}
		}
		else if (button == &op3Btn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::OP3)) );
			}
		}
		else if (button == &op4Btn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::OP4)) );
			}
		}
		else if (button == &sineBtn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::SINE)) );
			}
		}
		else if (button == &triangleBtn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::TRIANGLE)) );
			}
		}
		else if (button == &squareBtn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::SQUARE)) );
			}
		}
		else if (button == &sawBtn)
		{
			if (isPressed)
			{
				IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::SAWTOOTH)) );
			}
		}
		else if (button == &amplitudeDestBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::EG_AMP)) );
		}
		else if (button == &frequencyDestBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::EG_FREQ)) );
		}
		else if (button == &filterDestBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::EG_FILT)) );
		}
		else if (button == &ratioBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::RATIO)) );
		}
		else if (button == &monoBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::MONOPHONIC)) );
		}
		else if (button == &egRetriggerBtn)
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
						static_cast<unsigned int>(BUTTON_CHANNEL::GLIDE_RETRIG)) );
		}
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

void MainComponent::setFromARMor8VoiceState (const ARMor8VoiceState& state, unsigned int opToEdit, unsigned int presetNum)
{
	try
	{
		// set preset num label
		String presetNumStr( presetNum + 1 );
		presetNumLbl.setText( presetNumStr, dontSendNotification );

		// handle global states first
		pitchBendSldr.setValue( state.pitchBendSemitones, dontSendNotification );
		glideSldr.setValue( state.glideTime, dontSendNotification );
		if ( !egRetriggerBtn.getToggleState() )
		{
			if ( state.glideRetrigger )
			{
				egRetriggerBtn.setToggleState( true, dontSendNotification );
			}
		}
		else
		{
			if ( !state.glideRetrigger )
			{
				egRetriggerBtn.setToggleState( false, dontSendNotification );
			}
		}
		if ( !monoBtn.getToggleState() )
		{
			if ( state.monophonic )
			{
				monoBtn.setToggleState( true, dontSendNotification );
			}
		}
		else
		{
			if ( !state.monophonic )
			{
				monoBtn.setToggleState( false, dontSendNotification );
			}
		}

		switch ( opToEdit )
		{
			case 0:
				freqSldr.setValue( state.frequency1, dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio1 )
					{
						ratioBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio1 )
					{
						ratioBtn.setToggleState( true, dontSendNotification );
					}
				}
				switch ( state.wave1 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack1, dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo1, dontSendNotification );
				decaySldr.setValue( state.decay1, dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo1, dontSendNotification );
				sustainSldr.setValue( state.sustain1, dontSendNotification );
				releaseSldr.setValue( state.release1, dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo1, dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod1 )
					{
						amplitudeDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod1 )
					{
						amplitudeDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if (!state.egFrequencyMod1)
					{
						frequencyDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod1 )
					{
						frequencyDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod1 )
					{
						filterDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod1 )
					{
						filterDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount1, dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount1, dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount1, dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount1, dontSendNotification );
				amplitudeSldr.setValue( state.amplitude1, dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq1, dontSendNotification );
				filterResSldr.setValue( state.filterRes1, dontSendNotification );
				ampVelSldr.setValue( state.ampVelSens1, dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens1, dontSendNotification );
				detuneSldr.setValue( state.detune1, dontSendNotification );

				break;
			case 1:
				freqSldr.setValue( state.frequency2, dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio2 )
					{
						ratioBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio2 )
					{
						ratioBtn.setToggleState( true, dontSendNotification );
					}
				}
				switch ( state.wave2 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack2, dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo2, dontSendNotification );
				decaySldr.setValue( state.decay2, dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo2, dontSendNotification );
				sustainSldr.setValue( state.sustain2, dontSendNotification );
				releaseSldr.setValue( state.release2, dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo2, dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod2 )
					{
						amplitudeDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod2 )
					{
						amplitudeDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if ( !state.egFrequencyMod2 )
					{
						frequencyDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod2 )
					{
						frequencyDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod2 )
					{
						filterDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod2 )
					{
						filterDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount2, dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount2, dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount2, dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount2, dontSendNotification );
				amplitudeSldr.setValue( state.amplitude2, dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq2, dontSendNotification);
				filterResSldr.setValue( state.filterRes2, dontSendNotification);
				ampVelSldr.setValue( state.ampVelSens2, dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens2, dontSendNotification );
				detuneSldr.setValue( state.detune2, dontSendNotification );

				break;
			case 2:
				freqSldr.setValue( state.frequency3, dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio3 )
					{
						ratioBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio3 )
					{
						ratioBtn.setToggleState( true, dontSendNotification );
					}
				}
				switch ( state.wave3 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack3, dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo3, dontSendNotification );
				decaySldr.setValue( state.decay3, dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo3, dontSendNotification );
				sustainSldr.setValue( state.sustain3, dontSendNotification );
				releaseSldr.setValue( state.release3, dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo3, dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod3 )
					{
						amplitudeDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod3 )
					{
						amplitudeDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if ( !state.egFrequencyMod3 )
					{
						frequencyDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod3 )
					{
						frequencyDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod3 )
					{
						filterDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod3 )
					{
						filterDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount3, dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount3, dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount3, dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount3, dontSendNotification );
				amplitudeSldr.setValue( state.amplitude3, dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq3, dontSendNotification );
				filterResSldr.setValue( state.filterRes3, dontSendNotification );
				ampVelSldr.setValue( state.ampVelSens3, dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens3, dontSendNotification );
				detuneSldr.setValue( state.detune3, dontSendNotification );

				break;
			case 3:
				freqSldr.setValue( state.frequency4, dontSendNotification );
				if ( ratioBtn.getToggleState() )
				{
					if ( !state.useRatio4 )
					{
						ratioBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.useRatio4 )
					{
						ratioBtn.setToggleState( true, dontSendNotification );
					}
				}
				switch ( state.wave4 )
				{
					case OscillatorMode::SINE:
						sineBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::TRIANGLE:
						triangleBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SQUARE:
						squareBtn.setToggleState( true, dontSendNotification );
						break;
					case OscillatorMode::SAWTOOTH:
						sawBtn.setToggleState( true, dontSendNotification );
						break;
					default:
						std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
				}
				attackSldr.setValue( state.attack4, dontSendNotification );
				attackExpoSldr.setValue( state.attackExpo4, dontSendNotification );
				decaySldr.setValue( state.decay4, dontSendNotification );
				decayExpoSldr.setValue( state.decayExpo4, dontSendNotification );
				sustainSldr.setValue( state.sustain4, dontSendNotification );
				releaseSldr.setValue( state.release4, dontSendNotification );
				releaseExpoSldr.setValue( state.releaseExpo4, dontSendNotification );
				if ( amplitudeDestBtn.getToggleState() )
				{
					if ( !state.egAmplitudeMod4 )
					{
						amplitudeDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egAmplitudeMod4 )
					{
						amplitudeDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( frequencyDestBtn.getToggleState() )
				{
					if ( !state.egFrequencyMod4 )
					{
						frequencyDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{

					if ( state.egFrequencyMod4 )
					{
						frequencyDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				if ( filterDestBtn.getToggleState() )
				{
					if ( !state.egFilterMod4 )
					{
						filterDestBtn.setToggleState( false, dontSendNotification );
					}
				}
				else
				{
					if ( state.egFilterMod4 )
					{
						filterDestBtn.setToggleState( true, dontSendNotification );
					}
				}
				op1ModAmountSldr.setValue( state.op1ModAmount4, dontSendNotification );
				op2ModAmountSldr.setValue( state.op2ModAmount4, dontSendNotification );
				op3ModAmountSldr.setValue( state.op3ModAmount4, dontSendNotification );
				op4ModAmountSldr.setValue( state.op4ModAmount4, dontSendNotification );
				amplitudeSldr.setValue( state.amplitude4, dontSendNotification );
				filterFreqSldr.setValue( state.filterFreq4, dontSendNotification );
				filterResSldr.setValue( state.filterRes4, dontSendNotification );
				ampVelSldr.setValue( state.ampVelSens4, dontSendNotification );
				filtVelSldr.setValue( state.filtVelSens4, dontSendNotification );
				detuneSldr.setValue( state.detune4, dontSendNotification );

				break;
			default:
				std::cout << "Something is terribly, terribly wrong in setFromARMor8VoiceState..." << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Caught an exception in setFromState!!: " << e.what() << std::endl;
	}
}

void MainComponent::copyFrameBufferToImage()
{
	// TODO the sizes shouldn't be hard-coded
	for ( unsigned int pixelY = 0; pixelY < 64; pixelY++ )
	{
		for ( unsigned int pixelX = 0; pixelX < 128; pixelX++ )
		{
			// TODO here we would get the pixel values from the actual frame buffer, then copy to image
			screenRep.setPixelAt( (pixelX * 2),     (pixelY * 2),     Colour(255, 255, 255) );
			screenRep.setPixelAt( (pixelX * 2) + 1, (pixelY * 2),     Colour(255, 255, 255) );
			screenRep.setPixelAt( (pixelX * 2),     (pixelY * 2) + 1, Colour(255, 255, 255) );
			screenRep.setPixelAt( (pixelX * 2) + 1, (pixelY * 2) + 1, Colour(255, 255, 255) );
		}
	}
}

void MainComponent::setMidiInput (int index)
{
	auto list = MidiInput::getDevices();

	deviceManager.removeMidiInputCallback( list[lastInputIndex], this );

	auto newInput = list[index];

	if ( !deviceManager.isMidiInputEnabled(newInput) )
		deviceManager.setMidiInputEnabled( newInput, true );

	deviceManager.addMidiInputCallback( newInput, this );
	midiInputList.setSelectedId ( index + 1, dontSendNotification );

	lastInputIndex = index;
}

void MainComponent::handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message)
{
	for ( int byte = 0; byte < message.getRawDataSize(); byte++ )
	{
		midiHandler.processByte( message.getRawData()[byte] );
	}

	midiHandler.dispatchEvents();
}
