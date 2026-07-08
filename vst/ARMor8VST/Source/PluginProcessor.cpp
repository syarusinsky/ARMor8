/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "ARMor8Constants.hpp"
#include "ARMor8PresetUpgrader.hpp"
#include "CPPFile.hpp"
#include "EventQueue.hpp"
#include "Smoll.h"
#include "theroomdisconnectlogo.h"


//==============================================================================
ARMor8VSTAudioProcessor::ARMor8VSTAudioProcessor()
    : sAudioBuffer(),
      presetManager( sizeof(ARMor8PresetHeader), 20, new CPPFile("ARMor8Presets.spf") ),
      midiHandler(),
      font( Smoll_data ),
      logo( theroomdisconnectlogo_data ),
      armor8VoiceManager( &midiHandler, &presetManager ),
      armor8UiManager( 128, 64, CP_FORMAT::MONOCHROME_1BIT ),
      sampleRateConverter( 96000, SAMPLE_RATE, 512 ),
      undoManager(),
      apvts( *this, &undoManager, "PARAMETERS",
                                  { std::make_unique<AudioParameterFloat> ("effect1", "Effect 1", NormalisableRange<float> (0.0f, 1.0f), 0),
                                    std::make_unique<AudioParameterFloat> ("effect2", "Effect 2", NormalisableRange<float> (0.0f, 1.0f), 0),
                                    std::make_unique<AudioParameterFloat> ("effect3", "Effect 3", NormalisableRange<float> (0.0f, 1.0f), 0),
                                  }),
#ifndef JucePlugin_PreferredChannelConfigurations
      AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      processorId( IEventListener::getGlobalJuceProcessorId() )
{
    // add font and image to ui
    armor8UiManager.setFont( &font );
    armor8UiManager.setLogo( &logo );

    // this is to avoid unwanted audio on startup
    armor8VoiceManager.setOperatorAmplitude(0, 0.0f);
    armor8VoiceManager.setOperatorAmplitude(1, 0.0f);
    armor8VoiceManager.setOperatorAmplitude(2, 0.0f);
    armor8VoiceManager.setOperatorAmplitude(3, 0.0f);

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

    sAudioBuffer.registerCallback( &armor8VoiceManager );

    armor8UiManager.endLoading();
}

ARMor8VSTAudioProcessor::~ARMor8VSTAudioProcessor()
{
}

//==============================================================================
const juce::String ARMor8VSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ARMor8VSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ARMor8VSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ARMor8VSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ARMor8VSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ARMor8VSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ARMor8VSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ARMor8VSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ARMor8VSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void ARMor8VSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ARMor8VSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    sampleRateConverter.setSourceRate( static_cast<unsigned int>(sampleRate) );
    sampleRateConverter.setSourceBufferSize( samplesPerBlock );
    sampleRateConverter.resetAAFilters();
}

void ARMor8VSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ARMor8VSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ARMor8VSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const float* inBufferL = buffer.getReadPointer( 0, 0 );
    float* outBufferL = buffer.getWritePointer( 0, 0 );

    // if downsampling, anti-alias filter the source
    if ( ! sampleRateConverter.sourceToTargetIsUpsampling() )
    {
        float* inBufferLNonConst = const_cast<float*>( inBufferL );
        sampleRateConverter.filterSourceToTargetDownsampling( inBufferLNonConst );
    }

    const unsigned int maxTargetBufferSize = static_cast<unsigned int>( std::ceil(sampleRateConverter.getFractionalTargetBufferSize()) );
    float targetBuffer[ maxTargetBufferSize ]; // ceil, since can be fractional

    const unsigned int actualTargetBufferSize = sampleRateConverter.convertFromSourceToTargetDownsampling( inBufferL, targetBuffer );

    // then pass this audio into the target
    for ( unsigned int sample = 0; sample < actualTargetBufferSize; sample++ )
    {
        targetBuffer[sample] = sAudioBuffer.getNextSample( targetBuffer[sample] );
        sAudioBuffer.pollToFillBuffers();
    }

    // now we need to convert back
    sampleRateConverter.convertFromTargetToSourceUpsampling( targetBuffer, actualTargetBufferSize, outBufferL );

    // if upsampling, anti-alias filter the source
    if ( sampleRateConverter.targetToSourceIsUpsampling() )
    {
        sampleRateConverter.filterTargetToSourceUpsampling( outBufferL );
    }

    // fill the rest of the channels with the mono audio from channel 0
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for ( auto sample = 0; sample < buffer.getNumSamples(); sample++ )
        {
            channelData[sample] = outBufferL[sample];
        }
    }

    // handle midi input
    // TODO remove after testing
    if ( midiMessages.getNumEvents() > 0 )
    {
        std::cout << "STARTING MIDI IN PROCESSING ----------------" << std::endl;
    }
    for ( const auto& messageMetaData : midiMessages )
    {
        // midi input
        const auto& message = messageMetaData.getMessage();

        if ( message.getRawData()[0] == 0xF0 && message.getRawData()[2] == armor8VoiceManager.getDevId() ) // a looped back sysex message
        {
            continue;
        }
        else
        {
            // TODO remove after testing
            std::cout << "   MIDI IN: " <<  message.getDescription() << std::endl;
            for ( int byte = 0; byte < message.getRawDataSize(); byte++ )
            {
                midiHandler.processByte( message.getRawData()[byte] );
            }

            midiHandler.dispatchEvents();
        }
    }

    // handle midi output
    // TODO remove after testing
    static bool startedHandling = false;
    startedHandling = true;
    MidiEvent* outputMessage = midiHandler.nextOutputMidiMessage();

    while ( outputMessage != nullptr )
    {
        // TODO remove after testing
        if ( startedHandling == true )
        {
            std::cout << "STARTING MIDI OUT PROCESSING ----------------" << std::endl;
            startedHandling = false;
        }
        juce::MidiMessage juceMsg( outputMessage->getRawData(), outputMessage->getNumBytes() );
        // TODO remove after testing
        std::cout << "   MIDI OUT: " << juceMsg.getDescription() << std::endl;
        midiMessages.addEvent( juceMsg, 0 );

        outputMessage = midiHandler.nextOutputMidiMessage();
    }


    this->dispatchEventsForIds( processorId, processorEditorId );
}

//==============================================================================
bool ARMor8VSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ARMor8VSTAudioProcessor::createEditor()
{
    ARMor8VSTAudioProcessorEditor* editor = new ARMor8VSTAudioProcessorEditor( *this );
    processorEditorId = editor->getProcessorEditorId();
    this->dispatchEventsForIds( processorId, processorEditorId );

    return editor;
}

//==============================================================================
void ARMor8VSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();

    // store current voice state
    ARMor8VoiceState voiceState = armor8VoiceManager.getState();
    juce::MemoryBlock voiceStateMemoryBlock( &voiceState, sizeof(ARMor8VoiceState) );
    state.setProperty("voiceState", voiceStateMemoryBlock, nullptr);

    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ARMor8VSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));

            if (apvts.state.hasProperty("voiceState"))
            {
                juce::MemoryBlock* voiceStateMemoryBlock = apvts.state.getProperty("voiceState").getBinaryData();
                ARMor8VoiceState voiceState = *( reinterpret_cast<ARMor8VoiceState*>(voiceStateMemoryBlock->getData()) );
                armor8VoiceManager.setState( voiceState );

                this->dispatchEventsForIds( processorId, processorEditorId );
            }
        }
    }
}

void ARMor8VSTAudioProcessor::dispatchEventsForIds (const unsigned int processorId, const unsigned int processorEditorId)
{
    // The sequencing of these calls is extremely important and it's possible for other projects that the juceDispatchQueuedEvents function
    // may need to be called more than once if the event handling of a different event listener publishes new events to an event listener that
    // has already called it's juceDispatchQueuedEvents function. For example with this project IPotEventListener and IButtonEventListener handling
    // publishes IARMor8ParameterEventListener and IARMor8ParameterEventListener events, so they must be called first. Likewise, the handling of
    // IARMor8ParameterEventListener and IARMor8LCDRefreshEventListener events publishes IARMor8LCDRefreshEventListener events, so those must
    // be called before IARMor8LCDRefreshEventListener. The onus is on the user to sequence these correctly in the most performant way possible.
    EventDispatcher<IPotEventListener, PotEvent, &IPotEventListener::onPotEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<IButtonEventListener, ButtonEvent, &IButtonEventListener::onButtonEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<ISalSysexEventListener, SalSysexEvent, &ISalSysexEventListener::onSalSysexEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<IARMor8ParameterEventListener, ARMor8ParameterEvent,
                    &IARMor8ParameterEventListener::onARMor8ParameterEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<IARMor8PresetEventListener, ARMor8PresetEvent,
                    &IARMor8PresetEventListener::onARMor8PresetChangedEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<IARMor8LCDRefreshEventListener, ARMor8LCDRefreshEvent,
                    &IARMor8LCDRefreshEventListener::onARMor8LCDRefreshEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<IKeyEventListener, KeyEvent, &IKeyEventListener::onKeyEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
    EventDispatcher<IPitchEventListener, PitchEvent, &IPitchEventListener::onPitchEvent>::juceDispatchQueuedEvents( processorId, processorEditorId );
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ARMor8VSTAudioProcessor();
}
