/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "IARMor8LCDRefreshEventListener.hpp"

//==============================================================================
/**
*/
class ARMor8VSTAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::Button::Listener,
                                         private IARMor8LCDRefreshEventListener, private juce::Timer
{
public:
    ARMor8VSTAudioProcessorEditor (ARMor8VSTAudioProcessor&);
    ~ARMor8VSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void sliderValueChanged (juce::Slider* slider) override;
    bool keyPressed (const juce::KeyPress& k) override;
    bool keyStateChanged (bool isKeyDown) override;
    void buttonClicked (juce::Button* button) override;
    void onARMor8LCDRefreshEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent) override;

    void copyFrameBufferToImage (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ARMor8VSTAudioProcessor& audioProcessor;

    juce::Slider effect1Sldr;
    juce::Label effect1Lbl;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> effect1SldrAttachment;

    juce::Slider effect2Sldr;
    juce::Label effect2Lbl;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> effect2SldrAttachment;

    juce::Slider effect3Sldr;
    juce::Label effect3Lbl;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> effect3SldrAttachment;

    juce::TextButton effect1Btn;
    juce::TextButton effect2Btn;

    juce::Image screenRep;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ARMor8VSTAudioProcessorEditor)
};
