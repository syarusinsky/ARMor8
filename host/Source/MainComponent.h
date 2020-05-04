/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioBuffer.hpp"
#include "ARMor8VoiceManager.hpp"
#include "MidiHandler.hpp"
#include "PresetManager.hpp"

#include <iostream>
#include <fstream>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, public Slider::Listener, public Button::Listener, public MidiInputCallback
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    void sliderValueChanged (Slider* slider) override;
    void buttonClicked (Button* button) override;
    void updateToggleState (Button* button);

    void setFromARMor8VoiceState (const ARMor8VoiceState& state);

    void setMidiInput (int index);
    void handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message) override;

private:
    //==============================================================================
    // Your private member variables go here...
    PresetManager presetManager;
    MidiHandler midiHandler;
    int lastInputIndex;
    siike92::AudioBuffer sAudioBuffer;
    ARMor8VoiceManager armor8VoiceManager;
    bool keyButtonRelease;
    unsigned int opToEdit;

    AudioFormatWriter* writer;
    Slider freqSldr;
    Label freqLbl;
    Slider detuneSldr;
    Label detuneLbl;
    ToggleButton ratioBtn;

    Label editLbl;
    ToggleButton op1Btn;
    ToggleButton op2Btn;
    ToggleButton op3Btn;
    ToggleButton op4Btn;

    Label waveLbl;
    ToggleButton sineBtn;
    ToggleButton triangleBtn;
    ToggleButton squareBtn;
    ToggleButton sawBtn;

    Slider attackSldr;
    Label attackLbl;
    Slider attackExpoSldr;
    Label attackExpoLbl;
    Slider decaySldr;
    Label decayLbl;
    Slider decayExpoSldr;
    Label decayExpoLbl;
    Slider sustainSldr;
    Label sustainLbl;
    Slider releaseSldr;
    Label releaseLbl;
    Slider releaseExpoSldr;
    Label releaseExpoLbl;

    Label egDestLbl;
    ToggleButton amplitudeDestBtn;
    ToggleButton frequencyDestBtn;
    ToggleButton filterDestBtn;

    Slider op1ModAmountSldr;
    Label op1ModAmountLbl;
    Slider op2ModAmountSldr;
    Label op2ModAmountLbl;
    Slider op3ModAmountSldr;
    Label op3ModAmountLbl;
    Slider op4ModAmountSldr;
    Label op4ModAmountLbl;

    Slider amplitudeSldr;
    Label amplitudeLbl;

    Slider filterFreqSldr;
    Label filterFreqLbl;

    Slider filterResSldr;
    Label filterResLbl;

    Slider ampVelSldr;
    Label ampVelLbl;

    Slider filtVelSldr;
    Label filtVelLbl;

    Slider pitchBendSldr;
    Label pitchBendLbl;

    Slider glideSldr;
    Label glideLbl;

    ToggleButton egRetriggerBtn;

    ComboBox midiInputList;
    Label midiInputListLbl;

    ToggleButton monoBtn;

    TextButton prevPresetBtn;
    Label presetNumLbl;
    TextButton nextPresetBtn;
    TextButton writePresetBtn;

    std::ofstream testFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
