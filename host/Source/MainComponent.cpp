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
	presetManager(sizeof(ARMor8PresetHeader), 30, new CPPFile("ARMor8Presets.spf")),
	midiHandler(),
	lastInputIndex(0),
	sAudioBuffer(),
	armor8VoiceManager(),
	keyButtonRelease(false),
	opToEdit(0),
	writer(),
	freqSldr(),
	freqLbl(),
	detuneSldr(),
	detuneLbl(),
	ratioBtn("Ratio"),
	editLbl(),
	op1Btn("Op 1"),
	op2Btn("Op 2"),
	op3Btn("Op 3"),
	op4Btn("Op 4"),
	waveLbl(),
	sineBtn("Sine"),
	triangleBtn("Triangle"),
	squareBtn("Square"),
	sawBtn("Saw"),
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
	amplitudeDestBtn("Amplitude"),
	frequencyDestBtn("Frequency"),
	filterDestBtn("Filter"),
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
	egRetriggerBtn("Glide Retrigger"),
	midiInputList(),
	midiInputListLbl(),
	monoBtn("Global: Monophonic"),
	prevPresetBtn("Prev Preset"),
	presetNumLbl("Preset Number", "0"),
	nextPresetBtn("Next Preset"),
	writePresetBtn("Write Preset")
{
    armor8VoiceManager.bindToKeyEventSystem();
    armor8VoiceManager.bindToPitchEventSystem();

    armor8VoiceManager.setOperatorAmplitude(0, 0.0f);
    armor8VoiceManager.setOperatorAmplitude(1, 0.0f);
    armor8VoiceManager.setOperatorAmplitude(2, 0.0f);
    armor8VoiceManager.setOperatorAmplitude(3, 0.0f);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    sAudioBuffer.registerCallback(&armor8VoiceManager);

    AudioDeviceManager::AudioDeviceSetup deviceSetup = AudioDeviceManager::AudioDeviceSetup();
    deviceSetup.sampleRate = 44100;
    deviceManager.initialise (2, 2, 0, true, String(), &deviceSetup);

    Logger* log = Logger::getCurrentLogger();
    int sampleRate = deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();
    log->writeToLog(String(sampleRate));
    log->writeToLog(String(deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples()));

    /* WavAudioFormat wav;
    File tempFile ("TestAudio.wav");
    OutputStream* outStream (tempFile.createOutputStream());
    writer = wav.createWriterFor (outStream, sampleRate, 2, wav.getPossibleBitDepths().getLast(), NULL, 0);

    log->writeToLog(String(wav.getPossibleBitDepths().getLast()));
    log->writeToLog(tempFile.getFullPathName()); */

    testFile.open("JuceMainComponentOutput.txt");

    addAndMakeVisible(freqSldr);
    freqSldr.setRange(1.0f, 20000.0f);
    freqSldr.setTextValueSuffix("Hz");
    freqSldr.setSkewFactorFromMidPoint(500);
    freqSldr.addListener(this);
    addAndMakeVisible(freqLbl);
    freqLbl.setText("Frequency", dontSendNotification);
    freqLbl.attachToComponent(&freqSldr, true);

    addAndMakeVisible(detuneSldr);
    detuneSldr.setRange(-1200, 1200, 1);
    detuneSldr.setTextValueSuffix("Cents");
    detuneSldr.addListener(this);
    addAndMakeVisible(detuneLbl);
    detuneLbl.setText("Detune", dontSendNotification);
    detuneLbl.attachToComponent(&detuneSldr, true);

    addAndMakeVisible(ratioBtn);
    ratioBtn.onClick = [this] { updateToggleState(&ratioBtn); };
    ratioBtn.addListener(this);

    addAndMakeVisible(editLbl);
    editLbl.setText("Editing:", dontSendNotification);

    addAndMakeVisible(op1Btn);
    op1Btn.setRadioGroupId(OpRadioId);
    op1Btn.onClick = [this] { updateToggleState(&op1Btn); };
    addAndMakeVisible(op2Btn);
    op2Btn.setRadioGroupId(OpRadioId);
    op2Btn.onClick = [this] { updateToggleState(&op2Btn); };
    addAndMakeVisible(op3Btn);
    op3Btn.setRadioGroupId(OpRadioId);
    op3Btn.onClick = [this] { updateToggleState(&op3Btn); };
    addAndMakeVisible(op4Btn);
    op4Btn.setRadioGroupId(OpRadioId);
    op4Btn.onClick = [this] { updateToggleState(&op4Btn); };

    addAndMakeVisible(waveLbl);
    waveLbl.setText("Waveform:", dontSendNotification);

    addAndMakeVisible(sineBtn);
    sineBtn.setRadioGroupId(WaveRadioId);
    sineBtn.onClick = [this] { updateToggleState(&sineBtn); };
    addAndMakeVisible(triangleBtn);
    triangleBtn.setRadioGroupId(WaveRadioId);
    triangleBtn.onClick = [this] { updateToggleState(&triangleBtn); };
    addAndMakeVisible(squareBtn);
    squareBtn.setRadioGroupId(WaveRadioId);
    squareBtn.onClick = [this] { updateToggleState(&squareBtn); };
    addAndMakeVisible(sawBtn);
    sawBtn.setRadioGroupId(WaveRadioId);
    sawBtn.onClick = [this] { updateToggleState(&sawBtn); };

    addAndMakeVisible(attackSldr);
    attackSldr.setRange(0.002f, 2.0f);
    attackSldr.setTextValueSuffix("Seconds");
    attackSldr.addListener(this);
    addAndMakeVisible(attackLbl);
    attackLbl.setText("Attack", dontSendNotification);
    attackLbl.attachToComponent(&attackSldr, true);

    addAndMakeVisible(attackExpoSldr);
    attackExpoSldr.setRange(0.1f, 100.0f);
    attackExpoSldr.setTextValueSuffix("%");
    attackExpoSldr.addListener(this);
    addAndMakeVisible(attackExpoLbl);
    attackExpoLbl.setText("Attack Expo Amount", dontSendNotification);
    attackExpoLbl.attachToComponent(&attackExpoSldr, true);

    addAndMakeVisible(decaySldr);
    decaySldr.setRange(0.0f, 2.0f);
    decaySldr.setTextValueSuffix("Seconds");
    decaySldr.addListener(this);
    addAndMakeVisible(decayLbl);
    decayLbl.setText("Decay", dontSendNotification);
    decayLbl.attachToComponent(&decaySldr, true);

    addAndMakeVisible(decayExpoSldr);
    decayExpoSldr.setRange(0.1f, 100.0f);
    decayExpoSldr.setTextValueSuffix("%");
    decayExpoSldr.addListener(this);
    addAndMakeVisible(decayExpoLbl);
    decayExpoLbl.setText("Decay Expo Amount", dontSendNotification);
    decayExpoLbl.attachToComponent(&decayExpoSldr, true);

    addAndMakeVisible(sustainSldr);
    sustainSldr.setRange(0.0f, 1.0f);
    sustainSldr.setTextValueSuffix("%");
    sustainSldr.addListener(this);
    addAndMakeVisible(sustainLbl);
    sustainLbl.setText("Sustain", dontSendNotification);
    sustainLbl.attachToComponent(&sustainSldr, true);

    addAndMakeVisible(releaseSldr);
    releaseSldr.setRange(0.002f, 3.0f);
    releaseSldr.setTextValueSuffix("Seconds");
    releaseSldr.addListener(this);
    addAndMakeVisible(releaseLbl);
    releaseLbl.setText("Release", dontSendNotification);
    releaseLbl.attachToComponent(&releaseSldr, true);

    addAndMakeVisible(releaseExpoSldr);
    releaseExpoSldr.setRange(0.1f, 100.0f);
    releaseExpoSldr.setTextValueSuffix("%");
    releaseExpoSldr.addListener(this);
    addAndMakeVisible(releaseExpoLbl);
    releaseExpoLbl.setText("Release Expo Amount", dontSendNotification);
    releaseExpoLbl.attachToComponent(&releaseExpoSldr, true);

    addAndMakeVisible(egDestLbl);
    egDestLbl.setText("EG Destinations:", dontSendNotification);

    addAndMakeVisible(amplitudeDestBtn);
    amplitudeDestBtn.onClick = [this] { updateToggleState(&amplitudeDestBtn); };
    addAndMakeVisible(frequencyDestBtn);
    frequencyDestBtn.onClick = [this] { updateToggleState(&frequencyDestBtn); };
    addAndMakeVisible(filterDestBtn);
    filterDestBtn.onClick = [this] { updateToggleState(&filterDestBtn); };

    addAndMakeVisible(op1ModAmountSldr);
    op1ModAmountSldr.setRange(0.0f, 20000.0f);
    op1ModAmountSldr.addListener(this);
    addAndMakeVisible(op1ModAmountLbl);
    op1ModAmountLbl.setText("Op1 Mod Amount", dontSendNotification);
    op1ModAmountLbl.attachToComponent(&op1ModAmountSldr, true);

    addAndMakeVisible(op2ModAmountSldr);
    op2ModAmountSldr.setRange(0.0f, 20000.0f);
    op2ModAmountSldr.addListener(this);
    addAndMakeVisible(op2ModAmountLbl);
    op2ModAmountLbl.setText("Op2 Mod Amount", dontSendNotification);
    op2ModAmountLbl.attachToComponent(&op2ModAmountSldr, true);

    addAndMakeVisible(op3ModAmountSldr);
    op3ModAmountSldr.setRange(0.0f, 20000.0f);
    op3ModAmountSldr.addListener(this);
    addAndMakeVisible(op3ModAmountLbl);
    op3ModAmountLbl.setText("Op3 Mod Amount", dontSendNotification);
    op3ModAmountLbl.attachToComponent(&op3ModAmountSldr, true);

    addAndMakeVisible(op4ModAmountSldr);
    op4ModAmountSldr.setRange(0.0f, 20000.0f);
    op4ModAmountSldr.addListener(this);
    addAndMakeVisible(op4ModAmountLbl);
    op4ModAmountLbl.setText("Op4 Mod Amount", dontSendNotification);
    op4ModAmountLbl.attachToComponent(&op4ModAmountSldr, true);

    addAndMakeVisible(amplitudeSldr);
    amplitudeSldr.setRange(0.0f, 5.0f);
    amplitudeSldr.addListener(this);
    addAndMakeVisible(amplitudeLbl);
    amplitudeLbl.setText("Amplitude", dontSendNotification);
    amplitudeLbl.attachToComponent(&amplitudeSldr, true);

    addAndMakeVisible(filterFreqSldr);
    filterFreqSldr.setRange(1.0f, 20000.0f);
    filterFreqSldr.setSkewFactorFromMidPoint(500);
    filterFreqSldr.addListener(this);
    addAndMakeVisible(filterFreqLbl);
    filterFreqLbl.setText("Filter Freq", dontSendNotification);
    filterFreqLbl.attachToComponent(&filterFreqSldr, true);

    addAndMakeVisible(filterResSldr);
    filterResSldr.setRange(0.0f, 3.5f);
    filterResSldr.addListener(this);
    addAndMakeVisible(filterResLbl);
    filterResLbl.setText("Filter Res", dontSendNotification);
    filterResLbl.attachToComponent(&filterResSldr, true);

    addAndMakeVisible(ampVelSldr);
    ampVelSldr.setRange(0.0f, 1.0f);
    ampVelSldr.addListener(this);
    addAndMakeVisible(ampVelLbl);
    ampVelLbl.setText("Vel Sens Amplitude", dontSendNotification);
    ampVelLbl.attachToComponent(&ampVelSldr, true);

    addAndMakeVisible(filtVelSldr);
    filtVelSldr.setRange(0.0f, 1.0f);
    filtVelSldr.addListener(this);
    addAndMakeVisible(filtVelLbl);
    filtVelLbl.setText("Vel Sens Filter", dontSendNotification);
    filtVelLbl.attachToComponent(&filtVelSldr, true);

    addAndMakeVisible(pitchBendSldr);
    pitchBendSldr.setRange(1, 12, 1);
    pitchBendSldr.addListener(this);
    addAndMakeVisible(pitchBendLbl);
    pitchBendLbl.setText("Pitch Bend", dontSendNotification);
    pitchBendLbl.attachToComponent(&pitchBendSldr, true);

    addAndMakeVisible(glideSldr);
    glideSldr.setRange(0.0f, 1.0f);
    glideSldr.addListener(this);
    addAndMakeVisible(glideLbl);
    glideLbl.setText("Glide Time", dontSendNotification);
    glideLbl.attachToComponent(&glideSldr, true);

    addAndMakeVisible(egRetriggerBtn);
    egRetriggerBtn.onClick = [this] { updateToggleState(&egRetriggerBtn); };

    addAndMakeVisible(midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiInputs = MidiInput::getDevices();
    midiInputList.addItemList (midiInputs, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    // find the first enabled device and use that by default
    for (auto midiInput : midiInputs)
    {
        if (deviceManager.isMidiInputEnabled (midiInput))
        {
            setMidiInput (midiInputs.indexOf (midiInput));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);

    addAndMakeVisible(midiInputListLbl);
    midiInputListLbl.setText("Midi Input Device", dontSendNotification);
    midiInputListLbl.attachToComponent(&midiInputList, true);

    addAndMakeVisible(monoBtn);
    monoBtn.onClick = [this] { updateToggleState(&monoBtn); };
    monoBtn.addListener(this);

    addAndMakeVisible(prevPresetBtn);
    prevPresetBtn.addListener(this);

    addAndMakeVisible(presetNumLbl);

    addAndMakeVisible(nextPresetBtn);
    nextPresetBtn.addListener(this);

    addAndMakeVisible(writePresetBtn);
    writePresetBtn.addListener(this);

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
    float* writePtrR = bufferToFill.buffer->getWritePointer(1);
    float* writePtrL = bufferToFill.buffer->getWritePointer(0);
    const float* readPtrR = bufferToFill.buffer->getReadPointer(0);
    
    for (int i = 0; i < bufferToFill.numSamples; i++)
    {
	    float value = sAudioBuffer.getNextSample();
	    writePtrR[i] = value;
	    writePtrL[i] = value;
	    // testFile << readPtrR[i] << std::endl;
    }
    }
    catch (std::exception& e)
    {
	    std::cout << "Exception caught in getNextAudioBlock: " << e.what() << std::endl;
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
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
    midiInputList.setBounds   (sliderLeft, 970, getWidth() - sliderLeft - 10, 20);
    monoBtn.setBounds         (sliderLeft + (getWidth() / 5) * 0, 1000, ((getWidth() - sliderLeft - 10) / 5), 20);
    prevPresetBtn.setBounds   (sliderLeft + (getWidth() / 5) * 1, 1000, ((getWidth() - sliderLeft - 10) / 5), 20);
    presetNumLbl.setBounds    (sliderLeft + (getWidth() / 5) * 2, 1000, ((getWidth() - sliderLeft - 10) / 5), 20);
    nextPresetBtn.setBounds   ((getWidth() / 5) * 3, 1000, ((getWidth() - sliderLeft - 10) / 5), 20);
    writePresetBtn.setBounds  ((getWidth() / 5) * 4, 1000, ((getWidth() - sliderLeft - 10) / 5), 20);
}

void MainComponent::sliderValueChanged (Slider* slider)
{
	try
	{
	double val = slider->getValue();

	if (slider == &freqSldr)
	{
		armor8VoiceManager.setOperatorFreq(opToEdit, val);
	}
	else if (slider == &detuneSldr)
	{
		armor8VoiceManager.setOperatorDetune(opToEdit, val);
	}
	else if (slider == &attackSldr)
	{
		armor8VoiceManager.setOperatorEGAttack(opToEdit, val, attackExpoSldr.getValue());
	}
	else if (slider == &attackExpoSldr)
	{
		armor8VoiceManager.setOperatorEGAttack(opToEdit, attackSldr.getValue(), val);
	}
	else if (slider == &decaySldr)
	{
		armor8VoiceManager.setOperatorEGDecay(opToEdit, val, decayExpoSldr.getValue());
	}
	else if (slider == &decayExpoSldr)
	{
		armor8VoiceManager.setOperatorEGDecay(opToEdit, decaySldr.getValue(), val);
	}
	else if (slider == &sustainSldr)
	{
		armor8VoiceManager.setOperatorEGSustain(opToEdit, val);
	}
	else if (slider == &releaseSldr)
	{
		armor8VoiceManager.setOperatorEGRelease(opToEdit, val, releaseExpoSldr.getValue());
	}
	else if (slider == &releaseExpoSldr)
	{
		armor8VoiceManager.setOperatorEGRelease(opToEdit, releaseSldr.getValue(), val);
	}
	else if (slider == &op1ModAmountSldr)
	{
		armor8VoiceManager.setOperatorModulation(0, opToEdit, val);
	}
	else if (slider == &op2ModAmountSldr)
	{
		armor8VoiceManager.setOperatorModulation(1, opToEdit, val);
	}
	else if (slider == &op3ModAmountSldr)
	{
		armor8VoiceManager.setOperatorModulation(2, opToEdit, val);
	}
	else if (slider == &op4ModAmountSldr)
	{
		armor8VoiceManager.setOperatorModulation(3, opToEdit, val);
	}
	else if (slider == &amplitudeSldr)
	{
		armor8VoiceManager.setOperatorAmplitude(opToEdit, val);
	}
	else if (slider == &filterFreqSldr)
	{

		armor8VoiceManager.setOperatorFilterFreq(opToEdit, val);
	}
	else if (slider == &filterResSldr)
	{
		armor8VoiceManager.setOperatorFilterRes(opToEdit, val);
	}
	else if (slider == &ampVelSldr)
	{
		armor8VoiceManager.setOperatorAmpVelSens(opToEdit, val);
	}
	else if (slider == &filtVelSldr)
	{
		armor8VoiceManager.setOperatorFiltVelSens(opToEdit, val);
	}
	else if (slider == &pitchBendSldr)
	{
		midiHandler.setNumberOfSemitonesToPitchBend(val);
		armor8VoiceManager.setPitchBendSemitones(val);
	}
	else if (slider == &glideSldr)
	{
		armor8VoiceManager.setGlideTime(val);
	}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception caught in slider shit: " << e.what() << std::endl;
	}
}

void MainComponent::buttonClicked (Button* button)
{
	try
	{
		if (button == &prevPresetBtn)
		{
			ARMor8VoiceState preset = presetManager.prevPreset<ARMor8VoiceState>();
			String presetNumStr( presetManager.getCurrentPresetNum() );
			presetNumLbl.setText( presetNumStr, dontSendNotification );
			armor8VoiceManager.setState( preset );
			op1Btn.triggerClick();
		}
		else if (button == &nextPresetBtn)
		{
			ARMor8VoiceState preset = presetManager.nextPreset<ARMor8VoiceState>();
			String presetNumStr( presetManager.getCurrentPresetNum() );
			presetNumLbl.setText( presetNumStr, dontSendNotification );
			armor8VoiceManager.setState( preset );
			op1Btn.triggerClick();
		}
		else if (button == &writePresetBtn)
		{
			ARMor8VoiceState presetToWrite = armor8VoiceManager.getState();
			presetManager.writePreset<ARMor8VoiceState>( presetToWrite, presetManager.getCurrentPresetNum() );
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
			opToEdit = 0;
			setFromARMor8VoiceState (armor8VoiceManager.getState());
		}
	}
	else if (button == &op2Btn)
	{
		if (isPressed)
		{
			opToEdit = 1;
			setFromARMor8VoiceState (armor8VoiceManager.getState());
		}
	}
	else if (button == &op3Btn)
	{
		if (isPressed)
		{
			opToEdit = 2;
			setFromARMor8VoiceState (armor8VoiceManager.getState());
		}
	}
	else if (button == &op4Btn)
	{
		if (isPressed)
		{
			opToEdit = 3;
			setFromARMor8VoiceState (armor8VoiceManager.getState());
		}
	}
	else if (button == &sineBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorWave(opToEdit, OscillatorMode::SINE);
		}
	}
	else if (button == &triangleBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorWave(opToEdit, OscillatorMode::TRIANGLE);
		}
	}
	else if (button == &squareBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorWave(opToEdit, OscillatorMode::SQUARE);
		}
	}
	else if (button == &sawBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorWave(opToEdit, OscillatorMode::SAWTOOTH);
		}
	}
	else if (button == &amplitudeDestBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorEGModDestination(opToEdit, EGModDestination::AMPLITUDE, true);
		}
		else
		{
			armor8VoiceManager.setOperatorEGModDestination(opToEdit, EGModDestination::AMPLITUDE, false);
		}
	}
	else if (button == &frequencyDestBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorEGModDestination(opToEdit, EGModDestination::FREQUENCY, true);
		}
		else
		{
			armor8VoiceManager.setOperatorEGModDestination(opToEdit, EGModDestination::FREQUENCY, false);
		}
	}
	else if (button == &filterDestBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorEGModDestination(opToEdit, EGModDestination::FILT_FREQUENCY, true);
		}
		else
		{
			armor8VoiceManager.setOperatorEGModDestination(opToEdit, EGModDestination::FILT_FREQUENCY, false);
		}
	}
	else if (button == &ratioBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setOperatorRatio(opToEdit, true);
		}
		else
		{
			armor8VoiceManager.setOperatorRatio(opToEdit, false);
		}
	}
	else if (button == &monoBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setMonophonic(true);
			armor8VoiceManager.setUseGlide(true);
		}
		else
		{
			armor8VoiceManager.setMonophonic(false);
			armor8VoiceManager.setUseGlide(false);
		}
	}
	else if (button == &egRetriggerBtn)
	{
		if (isPressed)
		{
			armor8VoiceManager.setGlideRetrigger(true);
		}
		else
		{
			armor8VoiceManager.setGlideRetrigger(false);
		}
	}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception in toggle button shit: " << e.what() << std::endl;
	}
}

void MainComponent::setFromARMor8VoiceState (const ARMor8VoiceState& state)
{
	try
	{

	// handle global states first
	midiHandler.setNumberOfSemitonesToPitchBend(state.pitchBendSemitones);
	pitchBendSldr.setValue(state.pitchBendSemitones);
	glideSldr.setValue(state.glideTime);
	if (!egRetriggerBtn.getToggleState())
	{
		if (state.glideRetrigger)
		{
			egRetriggerBtn.triggerClick();
		}
	}
	else
	{
		if (!state.glideRetrigger)
		{
			egRetriggerBtn.triggerClick();
		}
	}
	if (!monoBtn.getToggleState())
	{
		if (state.monophonic)
		{
			monoBtn.triggerClick();
		}
	}
	else
	{
		if (!state.monophonic)
		{
			monoBtn.triggerClick();
		}
	}

	switch (opToEdit)
	{
		case 0:
			freqSldr.setValue(state.frequency1);
			if (ratioBtn.getToggleState())
			{
				if (!state.useRatio1)
				{
					ratioBtn.triggerClick();
				}
			}
			else
			{
				if (state.useRatio1)
				{
					ratioBtn.triggerClick();
				}
			}
			switch (state.wave1)
			{
				case OscillatorMode::SINE:
					sineBtn.triggerClick();
					break;
				case OscillatorMode::TRIANGLE:
					triangleBtn.triggerClick();
					break;
				case OscillatorMode::SQUARE:
					squareBtn.triggerClick();
					break;
				case OscillatorMode::SAWTOOTH:
					sawBtn.triggerClick();
					break;
				default:
					std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
			}
			attackSldr.setValue(state.attack1);
			attackExpoSldr.setValue(state.attackExpo1);
			decaySldr.setValue(state.decay1);
			decayExpoSldr.setValue(state.decayExpo1);
			sustainSldr.setValue(state.sustain1);
			releaseSldr.setValue(state.release1);
			releaseExpoSldr.setValue(state.releaseExpo1);
			if (amplitudeDestBtn.getToggleState())
			{
				if (!state.egAmplitudeMod1)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egAmplitudeMod1)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			if (frequencyDestBtn.getToggleState())
			{
				if (!state.egFrequencyMod1)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			else
			{

				if (state.egFrequencyMod1)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			if (filterDestBtn.getToggleState())
			{
				if (!state.egFilterMod1)
				{
					filterDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egFilterMod1)
				{
					filterDestBtn.triggerClick();
				}
			}
			op1ModAmountSldr.setValue(state.op1ModAmount1);
			op2ModAmountSldr.setValue(state.op2ModAmount1);
			op3ModAmountSldr.setValue(state.op3ModAmount1);
			op4ModAmountSldr.setValue(state.op4ModAmount1);
			amplitudeSldr.setValue(state.amplitude1);
			filterFreqSldr.setValue(state.filterFreq1);
			filterResSldr.setValue(state.filterRes1);
			ampVelSldr.setValue(state.ampVelSens1);
			filtVelSldr.setValue(state.filtVelSens1);
			detuneSldr.setValue(state.detune1);

			break;
		case 1:
			freqSldr.setValue(state.frequency2);
			if (ratioBtn.getToggleState())
			{
				if (!state.useRatio2)
				{
					ratioBtn.triggerClick();
				}
			}
			else
			{
				if (state.useRatio2)
				{
					ratioBtn.triggerClick();
				}
			}
			switch (state.wave2)
			{
				case OscillatorMode::SINE:
					sineBtn.triggerClick();
					break;
				case OscillatorMode::TRIANGLE:
					triangleBtn.triggerClick();
					break;
				case OscillatorMode::SQUARE:
					squareBtn.triggerClick();
					break;
				case OscillatorMode::SAWTOOTH:
					sawBtn.triggerClick();
					break;
				default:
					std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
			}
			attackSldr.setValue(state.attack2);
			attackExpoSldr.setValue(state.attackExpo2);
			decaySldr.setValue(state.decay2);
			decayExpoSldr.setValue(state.decayExpo2);
			sustainSldr.setValue(state.sustain2);
			releaseSldr.setValue(state.release2);
			releaseExpoSldr.setValue(state.releaseExpo2);
			if (amplitudeDestBtn.getToggleState())
			{
				if (!state.egAmplitudeMod2)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egAmplitudeMod2)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			if (frequencyDestBtn.getToggleState())
			{
				if (!state.egFrequencyMod2)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			else
			{

				if (state.egFrequencyMod2)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			if (filterDestBtn.getToggleState())
			{
				if (!state.egFilterMod2)
				{
					filterDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egFilterMod2)
				{
					filterDestBtn.triggerClick();
				}
			}
			op1ModAmountSldr.setValue(state.op1ModAmount2);
			op2ModAmountSldr.setValue(state.op2ModAmount2);
			op3ModAmountSldr.setValue(state.op3ModAmount2);
			op4ModAmountSldr.setValue(state.op4ModAmount2);
			amplitudeSldr.setValue(state.amplitude2);
			filterFreqSldr.setValue(state.filterFreq2);
			filterResSldr.setValue(state.filterRes2);
			ampVelSldr.setValue(state.ampVelSens2);
			filtVelSldr.setValue(state.filtVelSens2);
			detuneSldr.setValue(state.detune2);

			break;
		case 2:
			freqSldr.setValue(state.frequency3);
			if (ratioBtn.getToggleState())
			{
				if (!state.useRatio3)
				{
					ratioBtn.triggerClick();
				}
			}
			else
			{
				if (state.useRatio3)
				{
					ratioBtn.triggerClick();
				}
			}
			switch (state.wave3)
			{
				case OscillatorMode::SINE:
					sineBtn.triggerClick();
					break;
				case OscillatorMode::TRIANGLE:
					triangleBtn.triggerClick();
					break;
				case OscillatorMode::SQUARE:
					squareBtn.triggerClick();
					break;
				case OscillatorMode::SAWTOOTH:
					sawBtn.triggerClick();
					break;
				default:
					std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
			}
			attackSldr.setValue(state.attack3);
			attackExpoSldr.setValue(state.attackExpo3);
			decaySldr.setValue(state.decay3);
			decayExpoSldr.setValue(state.decayExpo3);
			sustainSldr.setValue(state.sustain3);
			releaseSldr.setValue(state.release3);
			releaseExpoSldr.setValue(state.releaseExpo3);
			if (amplitudeDestBtn.getToggleState())
			{
				if (!state.egAmplitudeMod3)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egAmplitudeMod3)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			if (frequencyDestBtn.getToggleState())
			{
				if (!state.egFrequencyMod3)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			else
			{

				if (state.egFrequencyMod3)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			if (filterDestBtn.getToggleState())
			{
				if (!state.egFilterMod3)
				{
					filterDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egFilterMod3)
				{
					filterDestBtn.triggerClick();
				}
			}
			op1ModAmountSldr.setValue(state.op1ModAmount3);
			op2ModAmountSldr.setValue(state.op2ModAmount3);
			op3ModAmountSldr.setValue(state.op3ModAmount3);
			op4ModAmountSldr.setValue(state.op4ModAmount3);
			amplitudeSldr.setValue(state.amplitude3);
			filterFreqSldr.setValue(state.filterFreq3);
			filterResSldr.setValue(state.filterRes3);
			ampVelSldr.setValue(state.ampVelSens3);
			filtVelSldr.setValue(state.filtVelSens3);
			detuneSldr.setValue(state.detune3);

			break;
		case 3:
			freqSldr.setValue(state.frequency4);
			if (ratioBtn.getToggleState())
			{
				if (!state.useRatio4)
				{
					ratioBtn.triggerClick();
				}
			}
			else
			{
				if (state.useRatio4)
				{
					ratioBtn.triggerClick();
				}
			}
			switch (state.wave4)
			{
				case OscillatorMode::SINE:
					sineBtn.triggerClick();
					break;
				case OscillatorMode::TRIANGLE:
					triangleBtn.triggerClick();
					break;
				case OscillatorMode::SQUARE:
					squareBtn.triggerClick();
					break;
				case OscillatorMode::SAWTOOTH:
					sawBtn.triggerClick();
					break;
				default:
					std::cout << "Wave not recognized in setFromARMor8VoiceState..." << std::endl;
			}
			attackSldr.setValue(state.attack4);
			attackExpoSldr.setValue(state.attackExpo4);
			decaySldr.setValue(state.decay4);
			decayExpoSldr.setValue(state.decayExpo4);
			sustainSldr.setValue(state.sustain4);
			releaseSldr.setValue(state.release4);
			releaseExpoSldr.setValue(state.releaseExpo4);
			if (amplitudeDestBtn.getToggleState())
			{
				if (!state.egAmplitudeMod4)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egAmplitudeMod4)
				{
					amplitudeDestBtn.triggerClick();
				}
			}
			if (frequencyDestBtn.getToggleState())
			{
				if (!state.egFrequencyMod4)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			else
			{

				if (state.egFrequencyMod4)
				{
					frequencyDestBtn.triggerClick();
				}
			}
			if (filterDestBtn.getToggleState())
			{
				if (!state.egFilterMod4)
				{
					filterDestBtn.triggerClick();
				}
			}
			else
			{
				if (state.egFilterMod4)
				{
					filterDestBtn.triggerClick();
				}
			}
			op1ModAmountSldr.setValue(state.op1ModAmount4);
			op2ModAmountSldr.setValue(state.op2ModAmount4);
			op3ModAmountSldr.setValue(state.op3ModAmount4);
			op4ModAmountSldr.setValue(state.op4ModAmount4);
			amplitudeSldr.setValue(state.amplitude4);
			filterFreqSldr.setValue(state.filterFreq4);
			filterResSldr.setValue(state.filterRes4);
			ampVelSldr.setValue(state.ampVelSens4);
			filtVelSldr.setValue(state.filtVelSens4);
			detuneSldr.setValue(state.detune4);

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

void MainComponent::setMidiInput (int index)
{
    auto list = MidiInput::getDevices();

    deviceManager.removeMidiInputCallback (list[lastInputIndex], this);

    auto newInput = list[index];
    
    if (! deviceManager.isMidiInputEnabled (newInput))
        deviceManager.setMidiInputEnabled (newInput, true);

    deviceManager.addMidiInputCallback (newInput, this);
    midiInputList.setSelectedId (index + 1, dontSendNotification);

    lastInputIndex = index;
}

void MainComponent::handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message)
{
	for (int byte = 0; byte < message.getRawDataSize(); byte++)
	{
		midiHandler.processByte( message.getRawData()[byte] );
	}

	midiHandler.dispatchEvents();
}
