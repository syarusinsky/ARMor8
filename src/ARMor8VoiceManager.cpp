#include "ARMor8VoiceManager.hpp"

#include "IARMor8PresetEventListener.hpp"
#include "MidiHandler.hpp"
#include "PresetManager.hpp"
#include "AudioConstants.hpp"
#include <string.h>
#include <cmath>

ARMor8VoiceManager::ARMor8VoiceManager (MidiHandler* midiHandler, PresetManager* presetManager) :
	m_MidiHandler (midiHandler),
	m_PresetManager (presetManager),
	m_OpToEdit (0),
	m_Monophonic (false),
	m_Voice1(),
	m_Voice2(),
	m_Voice3(),
	m_Voice4(),
	m_Voice5(),
	m_Voice6(),
	m_Voices { &m_Voice1, &m_Voice2, &m_Voice3, &m_Voice4, &m_Voice5, &m_Voice6 },
	m_ActiveKeyEventIndex (0),
	m_PitchBendSemitones (1),
	m_PresetHeader ({1, 1, 0, true})
{
}

ARMor8VoiceManager::~ARMor8VoiceManager()
{
}

void ARMor8VoiceManager::setOperatorToEdit (unsigned int opToEdit)
{
	if ( opToEdit < 4 )
	{
		m_OpToEdit = opToEdit;
	}
}

unsigned int ARMor8VoiceManager::getOperatorToEdit()
{
	return m_OpToEdit;
}

void ARMor8VoiceManager::setOperatorFreq (unsigned int opNum, float freq)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorFreq(opNum, freq);
	}
}

void ARMor8VoiceManager::setOperatorDetune (unsigned int opNum, int cents)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorDetune(opNum, cents);
	}
}

void ARMor8VoiceManager::setOperatorWave (unsigned int opNum, const OscillatorMode& wave)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorWave(opNum, wave);
	}
}

void ARMor8VoiceManager::setOperatorEGAttack (unsigned int opNum, float seconds, float expo)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorEGAttack(opNum, seconds, expo);
	}
}

void ARMor8VoiceManager::setOperatorEGDecay (unsigned int opNum, float seconds, float expo)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorEGDecay(opNum, seconds, expo);
	}
}

void ARMor8VoiceManager::setOperatorEGSustain (unsigned int opNum, float lvl)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{

		m_Voices[voice]->setOperatorEGSustain(opNum, lvl);
	}
}

void ARMor8VoiceManager::setOperatorEGRelease (unsigned int opNum, float seconds, float expo)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorEGRelease(opNum, seconds, expo);
	}
}

void ARMor8VoiceManager::setOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest, const bool on)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorEGModDestination(opNum, modDest, on);
	}
}

void ARMor8VoiceManager::setOperatorModulation (unsigned int sourceOpNum, unsigned int destOpNum, float modulationAmount)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorModulation(sourceOpNum, destOpNum, modulationAmount);
	}
}

void ARMor8VoiceManager::setOperatorAmplitude (unsigned int opNum, float amplitude)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorAmplitude(opNum, amplitude);
	}
}

void ARMor8VoiceManager::setOperatorFilterFreq (unsigned int opNum, float frequency)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorFilterFreq(opNum, frequency);
	}
}

void ARMor8VoiceManager::setOperatorFilterRes (unsigned int opNum, float resonance)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorFilterRes(opNum, resonance);
	}
}

void ARMor8VoiceManager::setOperatorRatio (unsigned int opNum, bool useRatio)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorRatio(opNum, useRatio);
	}
}

void ARMor8VoiceManager::setOperatorAmpVelSens (unsigned int opNum, float ampVelSens)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorAmpVelSens(opNum, ampVelSens);
	}
}

void ARMor8VoiceManager::setOperatorFiltVelSens (unsigned int opNum, float filtVelSens)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setOperatorFiltVelSens(opNum, filtVelSens);
	}
}

void ARMor8VoiceManager::setGlideTime (const float glideTime)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setGlideTime( glideTime );
	}
}

void ARMor8VoiceManager::setGlideRetrigger (const bool useRetrigger)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setGlideRetrigger( useRetrigger );
	}
}

void ARMor8VoiceManager::setUseGlide (const bool useGlide)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setUseGlide( useGlide );
	}
}

void ARMor8VoiceManager::setPitchBendSemitones (const unsigned int pitchBendSemitones)
{
	m_PitchBendSemitones = pitchBendSemitones;
}

void ARMor8VoiceManager::call (float* writeBuffer)
{
	// first clear write buffer
	memset(writeBuffer, 0, sizeof(float) * ABUFFER_SIZE);

	if (!m_Monophonic) // if polyphonic, we sum the voices
	{
		for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
		{
			ARMor8Voice& currentVoice = *m_Voices[voice];

			for (unsigned int sample = 0; sample < ABUFFER_SIZE; sample++)
			{
				writeBuffer[sample] += currentVoice.nextSample();
			}
		}
	}
	else // if monophonic, we only output the first voice
	{
		for (unsigned int sample = 0; sample < ABUFFER_SIZE; sample++)
		{
			ARMor8Voice& voice = *m_Voices[0];
			writeBuffer[sample] += voice.nextSample();
		}
	}
}

void ARMor8VoiceManager::setMonophonic (bool on)
{
	m_Monophonic = on;
}

void ARMor8VoiceManager::onKeyEvent (const KeyEvent& keyEvent)
{
	if ( !m_Monophonic ) // polyphonic implementation
	{
		if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
		{
			bool containsKeyEvent = false;
			for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
			{
				if ( m_ActiveKeyEvents[voice].isNoteAndType( keyEvent ) )
				{
					containsKeyEvent = true;
					m_ActiveKeyEvents[voice] = keyEvent;
					m_Voices[voice]->onKeyEvent(keyEvent);

					return;
				}
			}

			if (!containsKeyEvent)
			{
				// ensure we aren't overwriting a pressed key
				unsigned int initialActiveKeyEventIndex = m_ActiveKeyEventIndex;
				while ( m_ActiveKeyEvents[m_ActiveKeyEventIndex].pressed() == KeyPressedEnum::PRESSED )
				{
					m_ActiveKeyEventIndex = (m_ActiveKeyEventIndex + 1) % MAX_VOICES;

					if ( m_ActiveKeyEventIndex == initialActiveKeyEventIndex )
					{
						break;
					}
				}
				m_ActiveKeyEvents[m_ActiveKeyEventIndex] = keyEvent;
				m_Voices[m_ActiveKeyEventIndex]->onKeyEvent(keyEvent);

				m_ActiveKeyEventIndex = (m_ActiveKeyEventIndex + 1) % MAX_VOICES;

				return;
			}
		}
		else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
		{
			for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
			{
				if ( m_ActiveKeyEvents[voice].isNoteAndType( keyEvent, KeyPressedEnum::PRESSED ) )
				{
					m_ActiveKeyEvents[voice] = keyEvent;
					m_Voices[voice]->onKeyEvent( keyEvent );

					return;
				}
			}
		}
	}
	else // monophonic implementation
	{
		if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
		{
			// if a key is currently playing
			KeyPressedEnum activeKeyPressed = m_ActiveKeyEvents[0].pressed();
			if ( activeKeyPressed == KeyPressedEnum::PRESSED || activeKeyPressed == KeyPressedEnum::HELD )
			{
				// build a 'held' key event, since we don't want to retrigger the envelope generator
				KeyEvent newKeyEvent( KeyPressedEnum::HELD, keyEvent.note(), keyEvent.velocity() );

				if ( m_ActiveKeyEvents[0].note() < newKeyEvent.note() )
				{
					KeyEvent oldKeyEvent( KeyPressedEnum::HELD, m_ActiveKeyEvents[0].note(), m_ActiveKeyEvents[0].velocity() );

					// look for a place to store the old key event, since we only want to play the highest note
					for (unsigned int voice = 1; voice < MAX_VOICES; voice++)
					{
						if ( m_ActiveKeyEvents[voice].pressed() == KeyPressedEnum::RELEASED )
						{
							m_ActiveKeyEvents[voice] = oldKeyEvent;
							break;
						}
					}

					m_ActiveKeyEvents[0] = newKeyEvent;
					m_Voices[0]->onKeyEvent( newKeyEvent );

					return;
				}
				else if ( m_ActiveKeyEvents[0].note() > newKeyEvent.note() )
				{
					// look for a place to store this key event, since we only want to play the highest note
					for (unsigned int voice = 1; voice < MAX_VOICES; voice++)
					{
						if ( m_ActiveKeyEvents[voice].pressed() == KeyPressedEnum::RELEASED )
						{
							m_ActiveKeyEvents[voice] = newKeyEvent;
							break;
						}
					}

					return;
				}
			}
			else // there is no note currently active
			{
				m_ActiveKeyEvents[0] = keyEvent;
				m_Voices[0]->onKeyEvent( keyEvent );

				return;
			}
		}
		else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
		{
			// look for this note in the active key events array
			for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
			{
				// if there is a note that matches and isn't released
				KeyPressedEnum voiceKeyPressed = m_ActiveKeyEvents[voice].pressed();
				unsigned int voiceKeyNote = m_ActiveKeyEvents[voice].note();
				if ( voiceKeyPressed != KeyPressedEnum::RELEASED && voiceKeyNote == keyEvent.note() )
				{
					// if the main active voice is released, replace with lower note
					if (voice == 0)
					{
						int highestNote = -1; // negative 1 means no highest note found
						for (unsigned int voice2 = 1; voice2 < MAX_VOICES; voice2++)
						{
							KeyPressedEnum voice2KeyPressed = m_ActiveKeyEvents[voice2].pressed();
							int voice2KeyNote = m_ActiveKeyEvents[voice2].note();
							if ( voice2KeyPressed == KeyPressedEnum::HELD && voice2KeyNote > highestNote )
							{
								highestNote = voice2;
							}
						}

						if ( highestNote > 0 ) // if an active lower key is found
						{
							// store the lower key
							KeyEvent newActiveKeyEvent = m_ActiveKeyEvents[highestNote];

							// replace the lower key with a released key event
							unsigned int keyNote = m_ActiveKeyEvents[highestNote].note();
							unsigned int keyVelocity = m_ActiveKeyEvents[highestNote].velocity();
							KeyEvent inactiveKeyEvent( KeyPressedEnum::RELEASED, keyNote, keyVelocity );
							m_ActiveKeyEvents[highestNote] = inactiveKeyEvent;

							// replace the currently active note with the lower key
							m_ActiveKeyEvents[0] = newActiveKeyEvent;
							m_Voices[0]->onKeyEvent( newActiveKeyEvent );

							return;
						}
						else // if there are no active lower keys
						{
							m_ActiveKeyEvents[0] = keyEvent;
							m_Voices[0]->onKeyEvent( keyEvent );

							return;
						}
					}
					else // if one of the lower notes is released, replace with released key event
					{
						m_ActiveKeyEvents[voice] = keyEvent;

						return;
					}
				}
			}
		}
	}
}

void ARMor8VoiceManager::onPitchEvent (const PitchEvent& pitchEvent)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->onPitchEvent( pitchEvent );
	}
}

void ARMor8VoiceManager::onPotEvent (const PotEvent& potEvent)
{
	// TODO for hardware, may also need thresholds for changing values. ie: Pot needs to be turned
	// a certain amount before taking effect. That should probably be ifdef'd. Previous values may
	// be able to just be static variables beneath each pot channel.
	// UPDATE: this should actually be done in the ARMor8UiManager, which should generate the PotEvents

	POT_CHANNEL channel = static_cast<POT_CHANNEL>( potEvent.getChannel() );
	float percentage = potEvent.getPercentage();

	switch ( channel )
	{
		case POT_CHANNEL::FREQUENCY:
			this->setOperatorFreq( m_OpToEdit, FREQUENCY_MAX * percentage );

			break;
		case POT_CHANNEL::DETUNE:
			this->setOperatorDetune( m_OpToEdit, std::round((percentage * DETUNE_MAX * 2.0f) - DETUNE_MAX) );

			break;
		case POT_CHANNEL::ATTACK:
			this->setOperatorEGAttack( m_OpToEdit, (percentage * (ATTACK_MAX - ATTACK_MIN)) + ATTACK_MIN,
					m_Voices[0]->getOperatorAttackExpo(m_OpToEdit) );

			break;
		case POT_CHANNEL::ATTACK_EXPO:
			this->setOperatorEGAttack( m_OpToEdit, m_Voices[0]->getOperatorAttack(m_OpToEdit),
					(percentage * (EXPO_MAX - EXPO_MIN)) + EXPO_MIN );

			break;
		case POT_CHANNEL::DECAY:
			this->setOperatorEGDecay( m_OpToEdit, (percentage * (DECAY_MAX - DECAY_MIN)) + DECAY_MIN,
					m_Voices[0]->getOperatorDecayExpo(m_OpToEdit) );

			break;
		case POT_CHANNEL::DECAY_EXPO:
			this->setOperatorEGDecay( m_OpToEdit, m_Voices[0]->getOperatorDecay(m_OpToEdit),
					(percentage * (EXPO_MAX - EXPO_MIN)) + EXPO_MIN );

			break;
		case POT_CHANNEL::SUSTAIN:
			this->setOperatorEGSustain( m_OpToEdit, percentage );

			break;
		case POT_CHANNEL::RELEASE:
			this->setOperatorEGRelease( m_OpToEdit, (percentage * (RELEASE_MAX - RELEASE_MIN)) + RELEASE_MIN,
					m_Voices[0]->getOperatorReleaseExpo(m_OpToEdit) );

			break;
		case POT_CHANNEL::RELEASE_EXPO:
			this->setOperatorEGRelease( m_OpToEdit, m_Voices[0]->getOperatorRelease(m_OpToEdit),
					(percentage * (EXPO_MAX - EXPO_MIN)) + EXPO_MIN );

			break;
		case POT_CHANNEL::OP1_MOD_AMT:
			this->setOperatorModulation( 0, m_OpToEdit, percentage * OP_MOD_MAX );

			break;
		case POT_CHANNEL::OP2_MOD_AMT:
			this->setOperatorModulation( 1, m_OpToEdit, percentage * OP_MOD_MAX );

			break;
		case POT_CHANNEL::OP3_MOD_AMT:
			this->setOperatorModulation( 2, m_OpToEdit, percentage * OP_MOD_MAX );

			break;
		case POT_CHANNEL::OP4_MOD_AMT:
			this->setOperatorModulation( 3, m_OpToEdit, percentage * OP_MOD_MAX );

			break;
		case POT_CHANNEL::AMPLITUDE:
			this->setOperatorAmplitude( m_OpToEdit, percentage * AMPLITUDE_MAX );

			break;
		case POT_CHANNEL::FILT_FREQ:
			this->setOperatorFilterFreq( m_OpToEdit, (percentage * (FILT_FREQ_MAX - FILT_FREQ_MIN)) + FILT_FREQ_MIN );

			break;
		case POT_CHANNEL::FILT_RES:
			this->setOperatorFilterRes( m_OpToEdit, percentage * FILT_RES_MAX );

			break;
		case POT_CHANNEL::VEL_AMP:
			this->setOperatorAmpVelSens( m_OpToEdit, percentage );

			break;
		case POT_CHANNEL::VEL_FILT:
			this->setOperatorFiltVelSens( m_OpToEdit, percentage );

			break;
		case POT_CHANNEL::PITCH_BEND:
			this->setPitchBendSemitones( std::round(percentage * (PITCH_BEND_MAX - PITCH_BEND_MIN) + PITCH_BEND_MIN) );
			m_MidiHandler->setNumberOfSemitonesToPitchBend( m_PitchBendSemitones );

			break;
		case POT_CHANNEL::GLIDE_TIME:
			this->setGlideTime( percentage * GLIDE_TIME_MAX );

			break;
		default:
			break;
	}
}

void ARMor8VoiceManager::onButtonEvent (const ButtonEvent& buttonEvent)
{
	if ( buttonEvent.getButtonState() == BUTTON_STATE::RELEASED )
	{
		switch ( static_cast<BUTTON_CHANNEL>(buttonEvent.getChannel()) )
		{
			case BUTTON_CHANNEL::OP1:
				m_OpToEdit = 0;

				IARMor8PresetEventListener::PublishEvent( ARMor8PresetEvent(this->getState(),
										m_OpToEdit,
										m_PresetManager->getCurrentPresetNum(),
										0) );

				break;
			case BUTTON_CHANNEL::OP2:
				m_OpToEdit = 1;

				IARMor8PresetEventListener::PublishEvent( ARMor8PresetEvent(this->getState(),
										m_OpToEdit,
										m_PresetManager->getCurrentPresetNum(),
										0) );

				break;
			case BUTTON_CHANNEL::OP3:
				m_OpToEdit = 2;

				IARMor8PresetEventListener::PublishEvent( ARMor8PresetEvent(this->getState(),
										m_OpToEdit,
										m_PresetManager->getCurrentPresetNum(),
										0) );

				break;
			case BUTTON_CHANNEL::OP4:
				m_OpToEdit = 3;

				IARMor8PresetEventListener::PublishEvent( ARMor8PresetEvent(this->getState(),
										m_OpToEdit,
										m_PresetManager->getCurrentPresetNum(),
										0) );

				break;
			case BUTTON_CHANNEL::SINE:
				this->setOperatorWave( m_OpToEdit, OscillatorMode::SINE );

				break;
			case BUTTON_CHANNEL::TRIANGLE:
				this->setOperatorWave( m_OpToEdit, OscillatorMode::TRIANGLE );

				break;
			case BUTTON_CHANNEL::SQUARE:
				this->setOperatorWave( m_OpToEdit, OscillatorMode::SQUARE );

				break;
			case BUTTON_CHANNEL::SAWTOOTH:
				this->setOperatorWave( m_OpToEdit, OscillatorMode::SAWTOOTH );

				break;
			case BUTTON_CHANNEL::EG_AMP:
				{
					bool lastState = m_Voices[0]->getOperatorEGModDestination( m_OpToEdit, EGModDestination::AMPLITUDE );
					this->setOperatorEGModDestination( m_OpToEdit, EGModDestination::AMPLITUDE, !lastState );
				}

				break;
			case BUTTON_CHANNEL::EG_FREQ:
				{
					bool lastState = m_Voices[0]->getOperatorEGModDestination( m_OpToEdit, EGModDestination::FREQUENCY );
					this->setOperatorEGModDestination( m_OpToEdit, EGModDestination::FREQUENCY, !lastState );
				}

				break;
			case BUTTON_CHANNEL::EG_FILT:
				{
					bool lastState = m_Voices[0]->getOperatorEGModDestination( m_OpToEdit, EGModDestination::FILT_FREQUENCY );
					this->setOperatorEGModDestination( m_OpToEdit, EGModDestination::FILT_FREQUENCY, !lastState );
				}

				break;
			case BUTTON_CHANNEL::RATIO:
				{
					bool lastState = m_Voices[0]->getOperatorUseRatio( m_OpToEdit );
					this->setOperatorRatio( m_OpToEdit, !lastState );
				}

				break;
			case BUTTON_CHANNEL::MONOPHONIC:
				{
					m_Monophonic = !m_Monophonic;
				}

				break;
			case BUTTON_CHANNEL::GLIDE_RETRIG:
				{
					bool lastState = m_Voices[0]->getGlideRetrigger();
					this->setGlideRetrigger( !lastState );
				}

				break;
			case BUTTON_CHANNEL::PREV_PRESET:
				{
					ARMor8VoiceState preset = m_PresetManager->prevPreset<ARMor8VoiceState>();
					this->setState( preset );
					IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::OP1)) );
				}

				break;
			case BUTTON_CHANNEL::NEXT_PRESET:
				{
					ARMor8VoiceState preset = m_PresetManager->nextPreset<ARMor8VoiceState>();
					this->setState( preset );
					IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::OP1)) );
				}

				break;
			case BUTTON_CHANNEL::WRITE_PRESET:
				{
					ARMor8VoiceState presetToWrite = this->getState();
					m_PresetManager->writePreset<ARMor8VoiceState>( presetToWrite, m_PresetManager->getCurrentPresetNum() );
				}

				break;
			default:
				break;
		}
	}
}

ARMor8VoiceState ARMor8VoiceManager::getState()
{
	ARMor8VoiceState state = m_Voices[0]->getState();
	state.monophonic = m_Monophonic;
	state.pitchBendSemitones = m_PitchBendSemitones;

	return state;
}

void ARMor8VoiceManager::setState (const ARMor8VoiceState& state)
{
	for (unsigned int voice = 0; voice < MAX_VOICES; voice++)
	{
		m_Voices[voice]->setState( state );
	}

	// global
	m_Monophonic = state.monophonic;
	m_PitchBendSemitones = state.pitchBendSemitones;
	m_MidiHandler->setNumberOfSemitonesToPitchBend( m_PitchBendSemitones );
}

ARMor8PresetHeader ARMor8VoiceManager::getPresetHeader()
{
	return m_PresetHeader;
}
