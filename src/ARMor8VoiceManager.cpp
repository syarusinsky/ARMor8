#include "ARMor8VoiceManager.hpp"

#include "IARMor8PresetEventListener.hpp"
#include "MidiHandler.hpp"
#include "PresetManager.hpp"
#include "AudioConstants.hpp"
#include <cmath>
#include <algorithm>

ARMor8VoiceManager::ARMor8VoiceManager (MidiHandler* midiHandler, PresetManager* presetManager) :
	m_MidiHandler( midiHandler ),
	m_PresetManager( presetManager ),
	m_Monophonic( false ),
	m_Voice1(),
	m_Voice2(),
	m_Voice3(),
	m_Voice4(),
	m_Voice5(),
	m_Voice6(),
	m_Voices { &m_Voice1, &m_Voice2, &m_Voice3, &m_Voice4, &m_Voice5, &m_Voice6 },
	m_Pot1AssignmentIndex( 0 ),
	m_Pot1AssignmentOp( 0 ),
	m_Pot2AssignmentIndex( 1 ),
	m_Pot2AssignmentOp( 0 ),
	m_Pot3AssignmentIndex( 2 ),
	m_Pot3AssignmentOp( 0 ),
	m_ActiveKeyEventIndex( 0 ),
	m_PitchBendSemitones( 1 ),
	m_PresetHeader( {1, 2, 0, true} )
{
}

ARMor8VoiceManager::~ARMor8VoiceManager()
{
}

void ARMor8VoiceManager::setOperatorFreq (unsigned int opNum, float freq)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorFreq( opNum, freq );
	}
}

void ARMor8VoiceManager::setOperatorDetune (unsigned int opNum, int cents)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorDetune( opNum, cents );
	}
}

void ARMor8VoiceManager::setOperatorWave (unsigned int opNum, const OscillatorMode& wave)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorWave( opNum, wave );
	}
}

void ARMor8VoiceManager::setOperatorEGAttack (unsigned int opNum, float seconds, float expo)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorEGAttack( opNum, seconds, expo );
	}
}

void ARMor8VoiceManager::setOperatorEGDecay (unsigned int opNum, float seconds, float expo)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorEGDecay( opNum, seconds, expo );
	}
}

void ARMor8VoiceManager::setOperatorEGSustain (unsigned int opNum, float lvl)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{

		m_Voices[voice]->setOperatorEGSustain( opNum, lvl );
	}
}

void ARMor8VoiceManager::setOperatorEGRelease (unsigned int opNum, float seconds, float expo)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorEGRelease( opNum, seconds, expo );
	}
}

void ARMor8VoiceManager::setOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest, const bool on)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorEGModDestination( opNum, modDest, on );
	}
}

void ARMor8VoiceManager::setOperatorModulation (unsigned int sourceOpNum, unsigned int destOpNum, float modulationAmount)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorModulation( sourceOpNum, destOpNum, modulationAmount );
	}
}

void ARMor8VoiceManager::setOperatorAmplitude (unsigned int opNum, float amplitude)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorAmplitude( opNum, amplitude );
	}
}

void ARMor8VoiceManager::setOperatorFilterFreq (unsigned int opNum, float frequency)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorFilterFreq( opNum, frequency );
	}
}

void ARMor8VoiceManager::setOperatorFilterRes (unsigned int opNum, float resonance)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorFilterRes( opNum, resonance );
	}
}

void ARMor8VoiceManager::setOperatorRatio (unsigned int opNum, bool useRatio)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorRatio( opNum, useRatio );
	}
}

void ARMor8VoiceManager::setOperatorAmpVelSens (unsigned int opNum, float ampVelSens)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorAmpVelSens( opNum, ampVelSens );
	}
}

void ARMor8VoiceManager::setOperatorFiltVelSens (unsigned int opNum, float filtVelSens)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setOperatorFiltVelSens( opNum, filtVelSens );
	}
}

void ARMor8VoiceManager::setGlideTime (const float glideTime)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setGlideTime( glideTime );
	}
}

void ARMor8VoiceManager::setGlideRetrigger (const bool useRetrigger)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setGlideRetrigger( useRetrigger );
	}
}

void ARMor8VoiceManager::setUseGlide (const bool useGlide)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
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
	std::fill( writeBuffer, writeBuffer + ABUFFER_SIZE, 0.0f );

	if ( ! m_Monophonic ) // if polyphonic, we sum the voices
	{
		for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
		{
			m_Voices[voice]->call( writeBuffer );
		}
	}
	else // if monophonic, we only output the first voice
	{
		m_Voices[0]->call( writeBuffer );
	}
}

void ARMor8VoiceManager::setMonophonic (bool on)
{
	m_Monophonic = on;
}

void ARMor8VoiceManager::onKeyEvent (const KeyEvent& keyEvent)
{
	if ( ! m_Monophonic ) // polyphonic implementation
	{
		if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
		{
			bool containsKeyEvent = false;
			for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
			{
				if ( m_ActiveKeyEvents[voice].isNoteAndType( keyEvent ) )
				{
					containsKeyEvent = true;
					m_ActiveKeyEvents[voice] = keyEvent;
					m_Voices[voice]->onKeyEvent( keyEvent );

					return;
				}
			}

			if ( ! containsKeyEvent )
			{
				// ensure we aren't overwriting a pressed key
				unsigned int initialActiveKeyEventIndex = m_ActiveKeyEventIndex;
				while ( m_ActiveKeyEvents[m_ActiveKeyEventIndex].pressed() == KeyPressedEnum::PRESSED )
				{
					m_ActiveKeyEventIndex = ( m_ActiveKeyEventIndex + 1 ) % MAX_VOICES;

					if ( m_ActiveKeyEventIndex == initialActiveKeyEventIndex )
					{
						break;
					}
				}
				m_ActiveKeyEvents[m_ActiveKeyEventIndex] = keyEvent;
				m_Voices[m_ActiveKeyEventIndex]->onKeyEvent( keyEvent );

				m_ActiveKeyEventIndex = ( m_ActiveKeyEventIndex + 1 ) % MAX_VOICES;

				return;
			}
		}
		else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
		{
			for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
			{
				if ( m_ActiveKeyEvents[voice].isNoteAndType(keyEvent, KeyPressedEnum::PRESSED) )
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
					for ( unsigned int voice = 1; voice < MAX_VOICES; voice++ )
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
					for ( unsigned int voice = 1; voice < MAX_VOICES; voice++ )
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
			for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
			{
				// if there is a note that matches and isn't released
				KeyPressedEnum voiceKeyPressed = m_ActiveKeyEvents[voice].pressed();
				unsigned int voiceKeyNote = m_ActiveKeyEvents[voice].note();
				if ( voiceKeyPressed != KeyPressedEnum::RELEASED && voiceKeyNote == keyEvent.note() )
				{
					// if the main active voice is released, replace with lower note
					if ( voice == 0 )
					{
						int highestNote = -1; // negative 1 means no highest note found
						for ( unsigned int voice2 = 1; voice2 < MAX_VOICES; voice2++ )
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
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->onPitchEvent( pitchEvent );
	}
}

void ARMor8VoiceManager::onARMor8ParameterEvent (const ARMor8ParameterEvent& paramEvent)
{
	PARAM_CHANNEL channel = static_cast<PARAM_CHANNEL>( paramEvent.getChannel() );
	unsigned int op = paramEvent.getOperator() - 1; // the ui manager doesn't zero-index this value
	float val = paramEvent.getValue();

	switch ( channel )
	{
		case PARAM_CHANNEL::FREQUENCY:
			this->setOperatorFreq( op, val );

			break;
		case PARAM_CHANNEL::DETUNE:
			this->setOperatorDetune( op, static_cast<int>(val) );

			break;
		case PARAM_CHANNEL::EG_ATTACK:
			this->setOperatorEGAttack( op, val, m_Voices[0]->getOperatorAttackExpo(op) );

			break;
		case PARAM_CHANNEL::EG_ATTACK_EXPO:
			this->setOperatorEGAttack( op, m_Voices[0]->getOperatorAttack(op), val );

			break;
		case PARAM_CHANNEL::EG_DECAY:
			this->setOperatorEGDecay( op, val, m_Voices[0]->getOperatorDecayExpo(op) );

			break;
		case PARAM_CHANNEL::EG_DECAY_EXPO:
			this->setOperatorEGDecay( op, m_Voices[0]->getOperatorDecay(op), val );

			break;
		case PARAM_CHANNEL::EG_SUSTAIN:
			this->setOperatorEGSustain( op, val );

			break;
		case PARAM_CHANNEL::EG_RELEASE:
			this->setOperatorEGRelease( op, val, m_Voices[0]->getOperatorReleaseExpo(op) );

			break;
		case PARAM_CHANNEL::EG_RELEASE_EXPO:
			this->setOperatorEGRelease( op, m_Voices[0]->getOperatorRelease(op), val );

			break;
		case PARAM_CHANNEL::OP_1_MOD_AMOUNT:
			this->setOperatorModulation( 0, op, val );

			break;
		case PARAM_CHANNEL::OP_2_MOD_AMOUNT:
			this->setOperatorModulation( 1, op, val );

			break;
		case PARAM_CHANNEL::OP_3_MOD_AMOUNT:
			this->setOperatorModulation( 2, op, val );

			break;
		case PARAM_CHANNEL::OP_4_MOD_AMOUNT:
			this->setOperatorModulation( 3, op, val );

			break;
		case PARAM_CHANNEL::AMPLITUDE:
			this->setOperatorAmplitude( op, val );

			break;
		case PARAM_CHANNEL::FILTER_FREQ:
			this->setOperatorFilterFreq( op, val );

			break;
		case PARAM_CHANNEL::FILTER_RES:
			this->setOperatorFilterRes( op, val );

			break;
		case PARAM_CHANNEL::AMP_VEL_SENS:
			this->setOperatorAmpVelSens( op, val );

			break;
		case PARAM_CHANNEL::FILT_VEL_SENS:
			this->setOperatorFiltVelSens( op, val );

			break;
		case PARAM_CHANNEL::PITCH_BEND_SEMI:
			this->setPitchBendSemitones( static_cast<int>(val) );
			m_MidiHandler->setNumberOfSemitonesToPitchBend( m_PitchBendSemitones );

			break;
		case PARAM_CHANNEL::GLIDE_TIME:
			this->setGlideTime( val );

			break;
		case PARAM_CHANNEL::USE_RATIO:
			this->setOperatorRatio( op, static_cast<bool>(val) );

			break;
		case PARAM_CHANNEL::EG_DEST_AMP:
			this->setOperatorEGModDestination( op, EGModDestination::AMPLITUDE, static_cast<bool>(val) );

			break;
		case PARAM_CHANNEL::EG_DEST_FREQ:
			this->setOperatorEGModDestination( op, EGModDestination::FREQUENCY, static_cast<bool>(val) );

			break;
		case PARAM_CHANNEL::EG_DEST_FILT:
			this->setOperatorEGModDestination( op, EGModDestination::FILT_FREQUENCY, static_cast<bool>(&val) );

			break;
		case PARAM_CHANNEL::GLIDE_RETRIG:
			this->setGlideRetrigger( static_cast<bool>(val) );

			break;
		case PARAM_CHANNEL::MONOPHONIC:
			m_Monophonic = static_cast<float>(val);

			break;
		case PARAM_CHANNEL::SELECT_WAVEFORM:
			this->setOperatorWave( op, static_cast<OscillatorMode>(val) );

			break;
		case PARAM_CHANNEL::SELECT_OPERATOR:
			// refresh screen with new operator values
			if ( m_PresetManager )
			{
				IARMor8PresetEventListener::PublishEvent(
						ARMor8PresetEvent(this->getState(), m_PresetManager->getCurrentPresetNum(), 0) );
			}

			break;
		case PARAM_CHANNEL::NEXT_PRESET:
		{
			if ( m_PresetManager )
			{
				ARMor8VoiceState preset = m_PresetManager->nextPreset<ARMor8VoiceState>();
				this->setState( preset );
				IARMor8PresetEventListener::PublishEvent(
						ARMor8PresetEvent(this->getState(), m_PresetManager->getCurrentPresetNum(), 0) );
			}
		}

			break;
		case PARAM_CHANNEL::PREV_PRESET:
		{
			if ( m_PresetManager )
			{
				ARMor8VoiceState preset = m_PresetManager->prevPreset<ARMor8VoiceState>();
				this->setState( preset );
				IARMor8PresetEventListener::PublishEvent(
						ARMor8PresetEvent(this->getState(), m_PresetManager->getCurrentPresetNum(), 0) );
			}
		}
			break;
		case PARAM_CHANNEL::WRITE_PRESET:
		{
			if ( m_PresetManager )
			{
				ARMor8VoiceState presetToWrite = this->getState();
				m_PresetManager->writePreset<ARMor8VoiceState>( presetToWrite, m_PresetManager->getCurrentPresetNum() );
			}
		}

			break;
		case PARAM_CHANNEL::POT1_ASSIGNMENT:
			m_Pot1AssignmentIndex = static_cast<unsigned int>( val );
			m_Pot1AssignmentOp = op + 1; // to offset previous offset

			break;
		case PARAM_CHANNEL::POT2_ASSIGNMENT:
			m_Pot2AssignmentIndex = static_cast<unsigned int>( val );
			m_Pot2AssignmentOp = op + 1; // to offset previous offset

			break;
		case PARAM_CHANNEL::POT3_ASSIGNMENT:
			m_Pot3AssignmentIndex = static_cast<unsigned int>( val );
			m_Pot3AssignmentOp = op + 1; // to offset previous offset

			break;
		default:
			break;
	}
}

ARMor8VoiceState ARMor8VoiceManager::getState()
{
	ARMor8VoiceState state = m_Voices[0]->getState();
	state.pitchBendSemitones = m_PitchBendSemitones;
	state.pot1AssignmentIndex = m_Pot1AssignmentIndex;
	state.pot1AssignmentOp = m_Pot1AssignmentOp;
	state.pot2AssignmentIndex = m_Pot2AssignmentIndex;
	state.pot2AssignmentOp = m_Pot2AssignmentOp;
	state.pot3AssignmentIndex = m_Pot3AssignmentIndex;
	state.pot3AssignmentOp = m_Pot3AssignmentOp;

	return state;
}

void ARMor8VoiceManager::setState (const ARMor8VoiceState& state)
{
	for ( unsigned int voice = 0; voice < MAX_VOICES; voice++ )
	{
		m_Voices[voice]->setState( state );
	}

	// global
	m_Monophonic = state.monophonic;
	m_PitchBendSemitones = state.pitchBendSemitones;
	m_MidiHandler->setNumberOfSemitonesToPitchBend( m_PitchBendSemitones );
	m_Pot1AssignmentIndex = state.pot1AssignmentIndex;
	m_Pot1AssignmentOp = state.pot1AssignmentOp;
	m_Pot2AssignmentIndex = state.pot2AssignmentIndex;
	m_Pot2AssignmentOp = state.pot2AssignmentOp;
	m_Pot3AssignmentIndex = state.pot3AssignmentIndex;
	m_Pot3AssignmentOp = state.pot3AssignmentOp;
}

void ARMor8VoiceManager::loadCurrentPreset()
{
	if ( m_PresetManager )
	{
		ARMor8VoiceState preset = m_PresetManager->retrievePreset<ARMor8VoiceState>( m_PresetManager->getCurrentPresetNum() );
		this->setState( preset );
		IARMor8PresetEventListener::PublishEvent(
				ARMor8PresetEvent(this->getState(), m_PresetManager->getCurrentPresetNum(), 0) );
	}
}

ARMor8PresetHeader ARMor8VoiceManager::getPresetHeader()
{
	return m_PresetHeader;
}
