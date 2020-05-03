#include "ARMor8VoiceManager.hpp"

#include "AudioConstants.hpp"
#include <string.h>
#include <cmath>

const unsigned int numVoices = 6;

ARMor8VoiceManager::ARMor8VoiceManager() :
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
	m_PresetHeader ({1, 0, 0, true})
{
}

ARMor8VoiceManager::~ARMor8VoiceManager()
{
}

void ARMor8VoiceManager::setOperatorFreq (unsigned int opNum, float freq)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorFreq(opNum, freq);
	}
}

void ARMor8VoiceManager::setOperatorWave (unsigned int opNum, const OscillatorMode& wave)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorWave(opNum, wave);
	}
}

void ARMor8VoiceManager::setOperatorEGAttack (unsigned int opNum, float seconds, float expo)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorEGAttack(opNum, seconds, expo);
	}
}

void ARMor8VoiceManager::setOperatorEGDecay (unsigned int opNum, float seconds, float expo)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorEGDecay(opNum, seconds, expo);
	}
}

void ARMor8VoiceManager::setOperatorEGSustain (unsigned int opNum, float lvl)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{

		m_Voices[voice]->setOperatorEGSustain(opNum, lvl);
	}
}

void ARMor8VoiceManager::setOperatorEGRelease (unsigned int opNum, float seconds, float expo)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorEGRelease(opNum, seconds, expo);
	}
}

void ARMor8VoiceManager::setOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest, const bool on)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorEGModDestination(opNum, modDest, on);
	}
}

void ARMor8VoiceManager::setOperatorModulation (unsigned int sourceOpNum, unsigned int destOpNum, float modulationAmount)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorModulation(sourceOpNum, destOpNum, modulationAmount);
	}
}

void ARMor8VoiceManager::setOperatorAmplitude (unsigned int opNum, float amplitude)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorAmplitude(opNum, amplitude);
	}
}

void ARMor8VoiceManager::setOperatorFilterFreq (unsigned int opNum, float frequency)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorFilterFreq(opNum, frequency);
	}
}

void ARMor8VoiceManager::setOperatorFilterRes (unsigned int opNum, float resonance)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorFilterRes(opNum, resonance);
	}
}

void ARMor8VoiceManager::setOperatorRatio (unsigned int opNum, bool useRatio)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorRatio(opNum, useRatio);
	}
}

void ARMor8VoiceManager::setOperatorAmpVelSens (unsigned int opNum, float ampVelSens)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorAmpVelSens(opNum, ampVelSens);
	}
}

void ARMor8VoiceManager::setOperatorFiltVelSens (unsigned int opNum, float filtVelSens)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setOperatorFiltVelSens(opNum, filtVelSens);
	}
}

void ARMor8VoiceManager::setGlideTime (const float glideTime)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setGlideTime( glideTime );
	}
}

void ARMor8VoiceManager::setGlideRetrigger (const bool useRetrigger)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setGlideRetrigger( useRetrigger );
	}
}

void ARMor8VoiceManager::setUseGlide (const bool useGlide)
{
	for (unsigned int voice = 0; voice < numVoices; voice++)
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
		for (unsigned int voice = 0; voice < numVoices; voice++)
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
			for (unsigned int voice = 0; voice < numVoices; voice++)
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
					m_ActiveKeyEventIndex = (m_ActiveKeyEventIndex + 1) % numVoices;

					if ( m_ActiveKeyEventIndex == initialActiveKeyEventIndex )
					{
						break;
					}
				}
				m_ActiveKeyEvents[m_ActiveKeyEventIndex] = keyEvent;
				m_Voices[m_ActiveKeyEventIndex]->onKeyEvent(keyEvent);

				m_ActiveKeyEventIndex = (m_ActiveKeyEventIndex + 1) % numVoices;

				return;
			}
		}
		else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
		{
			for (unsigned int voice = 0; voice < numVoices; voice++)
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
					for (unsigned int voice = 1; voice < numVoices; voice++)
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
					for (unsigned int voice = 1; voice < numVoices; voice++)
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
			for (unsigned int voice = 0; voice < numVoices; voice++)
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
						for (unsigned int voice2 = 1; voice2 < numVoices; voice2++)
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
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->onPitchEvent( pitchEvent );
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
	for (unsigned int voice = 0; voice < numVoices; voice++)
	{
		m_Voices[voice]->setState( state );
	}

	// global
	m_Monophonic = state.monophonic;
	m_PitchBendSemitones = state.pitchBendSemitones;
}

ARMor8PresetHeader ARMor8VoiceManager::getPresetHeader()
{
	return m_PresetHeader;
}
