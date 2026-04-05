/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "AudioConstants.hpp"
#include "FrameBuffer.hpp"

//==============================================================================
ARMor8VSTAudioProcessorEditor::ARMor8VSTAudioProcessorEditor (ARMor8VSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      effect1Sldr(),
      effect1Lbl(),
      effect1SldrAttachment( std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getVTS(), "effect1", effect1Sldr) ),
      effect2Sldr(),
      effect2Lbl(),
      effect2SldrAttachment( std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getVTS(), "effect2", effect2Sldr) ),
      effect3Sldr(),
      effect3Lbl(),
      effect3SldrAttachment( std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getVTS(), "effect3", effect3Sldr) ),
      effect1Btn( "Effect 1" ),
      effect2Btn( "Effect 2" ),
      screenRep( juce::Image::RGB, 256, 128, true ) // this is actually double the size so we can actually see it
{
    // adding all child components
    addAndMakeVisible( effect1Sldr );
    effect1Sldr.setTextValueSuffix( "%" );
    effect1Sldr.addListener( this );
    addAndMakeVisible( effect1Lbl );
    effect1Lbl.setText( "Effect 1", juce::dontSendNotification );
    effect1Lbl.attachToComponent( &effect1Sldr, true );

    addAndMakeVisible( effect2Sldr );
    effect2Sldr.setTextValueSuffix( "%" );
    effect2Sldr.addListener( this );
    addAndMakeVisible( effect2Lbl );
    effect2Lbl.setText( "Effect 2", juce::dontSendNotification );
    effect2Lbl.attachToComponent( &effect2Sldr, true );

    addAndMakeVisible( effect3Sldr );
    effect3Sldr.setTextValueSuffix( "%" );
    effect3Sldr.addListener( this );
    addAndMakeVisible( effect3Lbl );
    effect3Lbl.setText( "Effect 3", juce::dontSendNotification );
    effect3Lbl.attachToComponent( &effect3Sldr, true );

    addAndMakeVisible( effect1Btn );
    effect1Btn.addListener( this );

    addAndMakeVisible( effect2Btn );
    effect2Btn.addListener( this );

    setSize( 800, 600 );

    this->bindToARMor8LCDRefreshEventSystem();

    // set initial values
    float effect1SldrPercentage = (effect1Sldr.getValue() - effect1Sldr.getMinimum()) / (effect1Sldr.getMaximum() - effect1Sldr.getMinimum());
    float effect2SldrPercentage = (effect2Sldr.getValue() - effect2Sldr.getMinimum()) / (effect2Sldr.getMaximum() - effect2Sldr.getMinimum());
    float effect3SldrPercentage = (effect3Sldr.getValue() - effect3Sldr.getMinimum()) / (effect3Sldr.getMaximum() - effect3Sldr.getMinimum());
    IPotEventListener::PublishEvent( PotEvent(effect1SldrPercentage, static_cast<unsigned int>(POT_CHANNEL::EFFECT1)) );
    IPotEventListener::PublishEvent( PotEvent(effect2SldrPercentage, static_cast<unsigned int>(POT_CHANNEL::EFFECT2)) );
    IPotEventListener::PublishEvent( PotEvent(effect3SldrPercentage, static_cast<unsigned int>(POT_CHANNEL::EFFECT3)) );

    // draw the target ui
    audioProcessor.getARMor8UiManager().draw();

    // start timer for fake loading and input polling
    this->startTimer( 33 );
}

ARMor8VSTAudioProcessorEditor::~ARMor8VSTAudioProcessorEditor()
{
}

//==============================================================================
void ARMor8VSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll( getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId) );

    // You can add your drawing code here!
    g.drawImageWithin( screenRep, 0, 150, getWidth(), 120, juce::RectanglePlacement::centred | juce::RectanglePlacement::doNotResize );
}

void ARMor8VSTAudioProcessorEditor::resized()
{
    int sliderLeft = 120;
    effect1Sldr.setBounds 	(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    effect2Sldr.setBounds 	(sliderLeft, 60, getWidth() - sliderLeft - 10, 20);
    effect3Sldr.setBounds 	(sliderLeft, 100, getWidth() - sliderLeft - 10, 20);
    effect1Btn.setBounds 	(sliderLeft, 300, (getWidth() / 2) - sliderLeft - 10, 20);
    effect2Btn.setBounds 	(sliderLeft, 340, (getWidth() / 2) - sliderLeft - 10, 20);
}

void ARMor8VSTAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    // using timer callback to process input instead

    // TODO not a good way to test the target's ui since we should be only updating the dirty part of the screen, but for now I'm lazy
    audioProcessor.getARMor8UiManager().draw();
}

bool ARMor8VSTAudioProcessorEditor::keyPressed (const juce::KeyPress& k)
{
    // for holding both buttons down at the same time
    if ( k.getTextCharacter() == 'z' )
    {
        effect1Btn.setState( juce::Button::ButtonState::buttonDown );
        effect2Btn.setState( juce::Button::ButtonState::buttonDown );
    }
    else if ( k.getTextCharacter() == '9' )
    {
        effect1Btn.setState( juce::Button::ButtonState::buttonDown );
    }
    else if ( k.getTextCharacter() == '0' )
    {
        effect2Btn.setState( juce::Button::ButtonState::buttonDown );
    }

    return true;
}

bool ARMor8VSTAudioProcessorEditor::keyStateChanged (bool isKeyDown)
{
    if ( ! isKeyDown ) // if a key has been released
    {
        effect1Btn.setState( juce::Button::ButtonState::buttonNormal );
        effect2Btn.setState( juce::Button::ButtonState::buttonNormal );
    }

    return true;
}

void ARMor8VSTAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    // using timer callback to process input instead

    // TODO not a good way to test the target's ui since we should be only updating the dirty part of the screen, but for now I'm lazy
    audioProcessor.getARMor8UiManager().draw();
}

void ARMor8VSTAudioProcessorEditor::timerCallback()
{
    static unsigned int fakeLoadingCounter = 100;

    if ( fakeLoadingCounter == 100 )
    {
        fakeLoadingCounter++;

        // set preset to first preset
        // audioProcessor.getARMor8VoiceManager().loadCurrentPreset();

        audioProcessor.getARMor8UiManager().endLoading();
    }
    else if ( fakeLoadingCounter < 100 )
    {
        audioProcessor.getARMor8UiManager().drawLoadingLogo();
        fakeLoadingCounter++;
    }
    else
    {
        audioProcessor.getARMor8UiManager().tickForChangingBackToStatus();
        audioProcessor.getARMor8UiManager().processEffect1Btn( effect1Btn.isDown() );
        audioProcessor.getARMor8UiManager().processEffect2Btn( effect2Btn.isDown() );

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

void ARMor8VSTAudioProcessorEditor::onARMor8LCDRefreshEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent)
{
    this->copyFrameBufferToImage( lcdRefreshEvent.getXStart(), lcdRefreshEvent.getYStart(),
                                  lcdRefreshEvent.getXEnd(), lcdRefreshEvent.getYEnd() );
    this->repaint();
}

void ARMor8VSTAudioProcessorEditor::copyFrameBufferToImage (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd)
{
    ARMor8UiManager& armor8UiManager = audioProcessor.getARMor8UiManager();
    ColorProfile* colorProfile = armor8UiManager.getColorProfile();
    FrameBuffer* frameBuffer = armor8UiManager.getFrameBuffer();
    unsigned int frameBufferWidth = frameBuffer->getWidth();

    for ( unsigned int pixelY = yStart; pixelY < yEnd + 1; pixelY++ )
    {
        for ( unsigned int pixelX = xStart; pixelX < xEnd + 1; pixelX++ )
        {
            if ( ! colorProfile->getPixel(frameBuffer->getPixels(), frameBufferWidth * frameBuffer->getHeight(), (pixelY * frameBufferWidth) + pixelX).m_M )
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
