#include "ARMor8Operator.hpp"

#include "ARMor8Constants.hpp"
#include "AudioConstants.hpp"
#include "MidiConstants.hpp"
#include "PolyBLEPOsc.hpp"
#include <math.h>

ARMor8Operator::ARMor8Operator (PolyBLEPOsc* wave, ADSREnvelopeGenerator<EG_RESPONSE>* eg, ARMor8Filter* filt, float amplitude, float frequency,
				ARMor8Operator* mod1, ARMor8Operator* mod2, ARMor8Operator* mod3, ARMor8Operator* mod4) :
	m_Osc( wave ),
	m_EG( eg ),
	m_Filter( filt ),
	m_FilterCenterFreq( 20000.0f ),
	m_UseRatio( false ),
	m_ModOperators{ mod1, mod2, mod3, mod4 },
	m_ModOperatorAmplitudes{ 0.0f },
	m_UseAmplitudeMod( false ),
	m_UseFrequencyMod( false ),
	m_UseFiltFreqMod( false ),
	m_Amplitude( amplitude ),
	m_AmplitudeCached( amplitude ),
	m_Frequency( frequency ),
	m_FrequencyCached( frequency ),
	m_Detune( 0 ),
	m_DetuneCached( 1.0f ),
	m_Ratio( 1.0f ),
	m_RatioFrequency( frequency ),
	m_CurrentValue( 0.0f ),
	m_AmpVelSens( 0.0f ),
	m_FiltVelSens( 0.0f ),
	m_CurrentVelocity( 1.0f ),
	m_FrequencyOffset( 0.0f ),
	m_GlideFrequency( 0.0f ),
	m_GlideTime( 0.0f ),
	m_GlideNumSamples( 1 ),
	m_GlideIncr( 0.0f ),
	m_UseGlideRetrigger( true ),
	m_UseGlide( false )
{
}

ARMor8Operator::~ARMor8Operator()
{
}

void ARMor8Operator::cachePerBlockValues()
{
	float egValue = m_EG->nextValue();

	m_AmplitudeCached = 1.0f - ( m_AmpVelSens * m_CurrentVelocity );

	if ( m_UseAmplitudeMod )
	{
		m_AmplitudeCached *= egValue;
	}

	if ( (m_GlideIncr >= 0.0f && m_GlideFrequency < m_RatioFrequency) || (m_GlideIncr <  0.0f && m_GlideFrequency > m_RatioFrequency) )
	{
		m_GlideFrequency += ( m_GlideIncr * ABUFFER_SIZE );

		if ( (m_GlideIncr >= 0.0f && m_GlideFrequency > m_RatioFrequency) || (m_GlideIncr <  0.0f && m_GlideFrequency < m_RatioFrequency) )
		{
			m_GlideFrequency = m_RatioFrequency;
		}
	}

	m_FrequencyCached = ( m_UseRatio ) ? m_GlideFrequency : m_Frequency;
	m_FrequencyCached += m_FrequencyOffset;
	m_FrequencyCached *= m_DetuneCached;

	if ( m_UseFrequencyMod )
	{
		m_FrequencyCached = ( m_FrequencyCached * egValue ) + 1.0f;
	}

	m_Osc->setFrequency( m_FrequencyCached );
	m_Osc->applyTriangleFilter();

	float filtFrequency = m_FilterCenterFreq;
	filtFrequency *= ( 1.0f - (m_FiltVelSens * m_CurrentVelocity) );

	if ( m_UseFiltFreqMod )
	{
		filtFrequency *= egValue;
	}

	m_Filter->setCoefficients( filtFrequency );
}

float ARMor8Operator::nextSample()
{
	float frequency = m_FrequencyCached;

	for ( unsigned int modNum = 0; modNum < ARMOR8_NUM_OPERATORS_PER_VOICE; modNum++ )
	{
		frequency += m_ModOperators[modNum]->currentValue() * m_ModOperatorAmplitudes[modNum];
	}

	m_Osc->setFrequency( frequency );
	m_CurrentValue = ( m_Osc->nextSample() ) * m_AmplitudeCached;

	m_CurrentValue = m_Filter->processSample( m_CurrentValue );

	return m_CurrentValue * m_Amplitude;
}

float ARMor8Operator::currentValue()
{
	return m_CurrentValue;
}

void ARMor8Operator::onKeyEvent (const KeyEvent& keyEvent)
{
	if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
	{
		m_CurrentVelocity = 1.0f - ( (float)keyEvent.velocity() / 127.0f );
	}

	if ( m_EG )
	{
		m_EG->onKeyEvent( keyEvent );
	}

	if ( m_UseRatio )
	{
		m_RatioFrequency = 0.0f;

		switch ( keyEvent.note() )
		{
			case MIDI_NOTE_A0:
				m_RatioFrequency = MUSIC_A0;
				break;
			case MIDI_NOTE_BB0:
				m_RatioFrequency = MUSIC_BB0;
				break;
			case MIDI_NOTE_B0:
				m_RatioFrequency = MUSIC_B0;
				break;
			case MIDI_NOTE_C1:
				m_RatioFrequency = MUSIC_C1;
				break;
			case MIDI_NOTE_DB1:
				m_RatioFrequency = MUSIC_DB1;
				break;
			case MIDI_NOTE_D1:
				m_RatioFrequency = MUSIC_D1;
				break;
			case MIDI_NOTE_EB1:
				m_RatioFrequency = MUSIC_EB1;
				break;
			case MIDI_NOTE_E1:
				m_RatioFrequency = MUSIC_E1;
				break;
			case MIDI_NOTE_F1:
				m_RatioFrequency = MUSIC_F1;
				break;
			case MIDI_NOTE_GB1:
				m_RatioFrequency = MUSIC_GB1;
				break;
			case MIDI_NOTE_G1:
				m_RatioFrequency = MUSIC_G1;
				break;
			case MIDI_NOTE_AB1:
				m_RatioFrequency = MUSIC_AB1;
				break;
			case MIDI_NOTE_A1:
				m_RatioFrequency = MUSIC_A1;
				break;
			case MIDI_NOTE_BB1:
				m_RatioFrequency = MUSIC_BB1;
				break;
			case MIDI_NOTE_B1:
				m_RatioFrequency = MUSIC_B1;
				break;
			case MIDI_NOTE_C2:
				m_RatioFrequency = MUSIC_C2;
				break;
			case MIDI_NOTE_DB2:
				m_RatioFrequency = MUSIC_DB2;
				break;
			case MIDI_NOTE_D2:
				m_RatioFrequency = MUSIC_D2;
				break;
			case MIDI_NOTE_EB2:
				m_RatioFrequency = MUSIC_EB2;
				break;
			case MIDI_NOTE_E2:
				m_RatioFrequency = MUSIC_E2;
				break;
			case MIDI_NOTE_F2:
				m_RatioFrequency = MUSIC_F2;
				break;
			case MIDI_NOTE_GB2:
				m_RatioFrequency = MUSIC_GB2;
				break;
			case MIDI_NOTE_G2:
				m_RatioFrequency = MUSIC_G2;
				break;
			case MIDI_NOTE_AB2:
				m_RatioFrequency = MUSIC_AB2;
				break;
			case MIDI_NOTE_A2:
				m_RatioFrequency = MUSIC_A2;
				break;
			case MIDI_NOTE_BB2:
				m_RatioFrequency = MUSIC_BB2;
				break;
			case MIDI_NOTE_B2:
				m_RatioFrequency = MUSIC_B2;
				break;
			case MIDI_NOTE_C3:
				m_RatioFrequency = MUSIC_C3;
				break;
			case MIDI_NOTE_DB3:
				m_RatioFrequency = MUSIC_DB3;
				break;
			case MIDI_NOTE_D3:
				m_RatioFrequency = MUSIC_D3;
				break;
			case MIDI_NOTE_EB3:
				m_RatioFrequency = MUSIC_EB3;
				break;
			case MIDI_NOTE_E3:
				m_RatioFrequency = MUSIC_E3;
				break;
			case MIDI_NOTE_F3:
				m_RatioFrequency = MUSIC_F3;
				break;
			case MIDI_NOTE_GB3:
				m_RatioFrequency = MUSIC_GB3;
				break;
			case MIDI_NOTE_G3:
				m_RatioFrequency = MUSIC_G3;
				break;
			case MIDI_NOTE_AB3:
				m_RatioFrequency = MUSIC_AB3;
				break;
			case MIDI_NOTE_A3:
				m_RatioFrequency = MUSIC_A3;
				break;
			case MIDI_NOTE_BB3:
				m_RatioFrequency = MUSIC_BB3;
				break;
			case MIDI_NOTE_B3:
				m_RatioFrequency = MUSIC_B3;
				break;
			case MIDI_NOTE_C4:
				m_RatioFrequency = MUSIC_C4;
				break;
			case MIDI_NOTE_DB4:
				m_RatioFrequency = MUSIC_DB4;
				break;
			case MIDI_NOTE_D4:
				m_RatioFrequency = MUSIC_D4;
				break;
			case MIDI_NOTE_EB4:
				m_RatioFrequency = MUSIC_EB4;
				break;
			case MIDI_NOTE_E4:
				m_RatioFrequency = MUSIC_E4;
				break;
			case MIDI_NOTE_F4:
				m_RatioFrequency = MUSIC_F4;
				break;
			case MIDI_NOTE_GB4:
				m_RatioFrequency = MUSIC_GB4;
				break;
			case MIDI_NOTE_G4:
				m_RatioFrequency = MUSIC_G4;
				break;
			case MIDI_NOTE_AB4:
				m_RatioFrequency = MUSIC_AB4;
				break;
			case MIDI_NOTE_A4:
				m_RatioFrequency = MUSIC_A4;
				break;
			case MIDI_NOTE_BB4:
				m_RatioFrequency = MUSIC_BB4;
				break;
			case MIDI_NOTE_B4:
				m_RatioFrequency = MUSIC_B4;
				break;
			case MIDI_NOTE_C5:
				m_RatioFrequency = MUSIC_C5;
				break;
			case MIDI_NOTE_DB5:
				m_RatioFrequency = MUSIC_DB5;
				break;
			case MIDI_NOTE_D5:
				m_RatioFrequency = MUSIC_D5;
				break;
			case MIDI_NOTE_EB5:
				m_RatioFrequency = MUSIC_EB5;
				break;
			case MIDI_NOTE_E5:
				m_RatioFrequency = MUSIC_E5;
				break;
			case MIDI_NOTE_F5:
				m_RatioFrequency = MUSIC_F5;
				break;
			case MIDI_NOTE_GB5:
				m_RatioFrequency = MUSIC_GB5;
				break;
			case MIDI_NOTE_G5:
				m_RatioFrequency = MUSIC_G5;
				break;
			case MIDI_NOTE_AB5:
				m_RatioFrequency = MUSIC_AB5;
				break;
			case MIDI_NOTE_A5:
				m_RatioFrequency = MUSIC_A5;
				break;
			case MIDI_NOTE_BB5:
				m_RatioFrequency = MUSIC_BB5;
				break;
			case MIDI_NOTE_B5:
				m_RatioFrequency = MUSIC_B5;
				break;
			case MIDI_NOTE_C6:
				m_RatioFrequency = MUSIC_C6;
				break;
			case MIDI_NOTE_DB6:
				m_RatioFrequency = MUSIC_DB6;
				break;
			case MIDI_NOTE_D6:
				m_RatioFrequency = MUSIC_D6;
				break;
			case MIDI_NOTE_EB6:
				m_RatioFrequency = MUSIC_EB6;
				break;
			case MIDI_NOTE_E6:
				m_RatioFrequency = MUSIC_E6;
				break;
			case MIDI_NOTE_F6:
				m_RatioFrequency = MUSIC_F6;
				break;
			case MIDI_NOTE_GB6:
				m_RatioFrequency = MUSIC_GB6;
				break;
			case MIDI_NOTE_G6:
				m_RatioFrequency = MUSIC_G6;
				break;
			case MIDI_NOTE_AB6:
				m_RatioFrequency = MUSIC_AB6;
				break;
			case MIDI_NOTE_A6:
				m_RatioFrequency = MUSIC_A6;
				break;
			case MIDI_NOTE_BB6:
				m_RatioFrequency = MUSIC_BB6;
				break;
			case MIDI_NOTE_B6:
				m_RatioFrequency = MUSIC_B6;
				break;
			case MIDI_NOTE_C7:
				m_RatioFrequency = MUSIC_C7;
				break;
			case MIDI_NOTE_DB7:
				m_RatioFrequency = MUSIC_DB7;
				break;
			case MIDI_NOTE_D7:
				m_RatioFrequency = MUSIC_D7;
				break;
			case MIDI_NOTE_EB7:
				m_RatioFrequency = MUSIC_EB7;
				break;
			case MIDI_NOTE_E7:
				m_RatioFrequency = MUSIC_E7;
				break;
			case MIDI_NOTE_F7:
				m_RatioFrequency = MUSIC_F7;
				break;
			case MIDI_NOTE_GB7:
				m_RatioFrequency = MUSIC_GB7;
				break;
			case MIDI_NOTE_G7:
				m_RatioFrequency = MUSIC_G7;
				break;
			case MIDI_NOTE_AB7:
				m_RatioFrequency = MUSIC_AB7;
				break;
			case MIDI_NOTE_A7:
				m_RatioFrequency = MUSIC_A7;
				break;
			case MIDI_NOTE_BB7:
				m_RatioFrequency = MUSIC_BB7;
				break;
			case MIDI_NOTE_B7:
				m_RatioFrequency = MUSIC_B7;
				break;
			case MIDI_NOTE_C8:
				m_RatioFrequency = MUSIC_C8;
				break;
			case MIDI_NOTE_DB8:
				m_RatioFrequency = MUSIC_DB8;
				break;
			case MIDI_NOTE_D8:
				m_RatioFrequency = MUSIC_D8;
				break;
			case MIDI_NOTE_EB8:
				m_RatioFrequency = MUSIC_EB8;
				break;
			case MIDI_NOTE_E8:
				m_RatioFrequency = MUSIC_E8;
				break;
			case MIDI_NOTE_F8:
				m_RatioFrequency = MUSIC_F8;
				break;
			case MIDI_NOTE_GB8:
				m_RatioFrequency = MUSIC_GB8;
				break;
			case MIDI_NOTE_G8:
				m_RatioFrequency = MUSIC_G8;
				break;
			case MIDI_NOTE_AB8:
				m_RatioFrequency = MUSIC_AB8;
				break;
			case MIDI_NOTE_A8:
				m_RatioFrequency = MUSIC_A8;
				break;
			case MIDI_NOTE_BB8:
				m_RatioFrequency = MUSIC_BB8;
				break;
			case MIDI_NOTE_B8:
				m_RatioFrequency = MUSIC_B8;
				break;
			case MIDI_NOTE_C9:
				m_RatioFrequency = MUSIC_C9;
				break;
			case MIDI_NOTE_DB9:
				m_RatioFrequency = MUSIC_DB9;
				break;
			case MIDI_NOTE_D9:
				m_RatioFrequency = MUSIC_D9;
				break;
			case MIDI_NOTE_EB9:
				m_RatioFrequency = MUSIC_E9;
				break;
			case MIDI_NOTE_E9:
				m_RatioFrequency = MUSIC_E9;
				break;
			case MIDI_NOTE_F9:
				m_RatioFrequency = MUSIC_F9;
				break;
			case MIDI_NOTE_GB9:
				m_RatioFrequency = MUSIC_GB9;
				break;
			case MIDI_NOTE_G9:
				m_RatioFrequency = MUSIC_G9;
				break;
			default:
				m_RatioFrequency = 0.0f;
		}

		m_RatioFrequency *= m_Ratio;

		if ( !m_UseGlide || (m_UseGlideRetrigger && keyEvent.pressed() == KeyPressedEnum::PRESSED) )
		{
			m_GlideFrequency = m_RatioFrequency;
		}
		else
		{
			m_GlideIncr = (m_RatioFrequency - m_GlideFrequency) / m_GlideNumSamples;
		}
	}
}

void ARMor8Operator::onPitchEvent (const PitchEvent& pitchEvent)
{
	this->setFrequencyOffset( (m_RatioFrequency * pitchEvent.getPitchFactor()) - m_RatioFrequency );
}

OscillatorMode ARMor8Operator::getWave()
{
	return m_Osc->getOscillatorMode();
}

void ARMor8Operator::setWave (const OscillatorMode& wave)
{
	m_Osc->setOscillatorMode( wave );
}

ADSREnvelopeGenerator<EG_RESPONSE>* ARMor8Operator::getEnvelopeGenerator()
{
	return m_EG;
}

void ARMor8Operator::setEnvelopeGenerator (ADSREnvelopeGenerator<EG_RESPONSE>* eg)
{
	if ( eg )
	{
		m_EG = eg;
	}
}

void ARMor8Operator::setModSourceAmplitude (ARMor8Operator* modSource, float amplitude)
{
	if ( modSource == m_ModOperators[0] )
	{
		m_ModOperatorAmplitudes[0] = amplitude;
	}
	else if ( modSource == m_ModOperators[1] )
	{
		m_ModOperatorAmplitudes[1] = amplitude;
	}
	else if ( modSource == m_ModOperators[2] )
	{
		m_ModOperatorAmplitudes[2] = amplitude;
	}
	else if ( modSource == m_ModOperators[3] )
	{
		m_ModOperatorAmplitudes[3] = amplitude;
	}
}

void ARMor8Operator::setEGModDestination (const EGModDestination& modDest, const bool on)
{
	if ( modDest == EGModDestination::AMPLITUDE )
	{
		m_UseAmplitudeMod = on;
	}
	else if ( modDest == EGModDestination::FREQUENCY )
	{
		m_UseFrequencyMod = on;
	}
	else if ( modDest == EGModDestination::FILT_FREQUENCY )
	{
		m_UseFiltFreqMod = on;
	}
}

void ARMor8Operator::setFrequency (const float frequency)
{
	m_Frequency = frequency;

	if ( m_Frequency > OP_THRESHOLD_6X )
	{
		m_Ratio = 6.0f;
	}
	else if ( m_Frequency > OP_THRESHOLD_5X )
	{
		m_Ratio = 5.0f;
	}
	else if ( m_Frequency > OP_THRESHOLD_4X )
	{
		m_Ratio = 4.0f;
	}
	else if ( m_Frequency > OP_THRESHOLD_3X )
	{
		m_Ratio = 3.0f;
	}
	else if ( m_Frequency > OP_THRESHOLD_2X )
	{
		m_Ratio = 2.0f;
	}
	else if ( m_Frequency > OP_THRESHOLD_1X )
	{
		m_Ratio = 1.0f;
	}
	else if ( m_Frequency > OP_THRESHOLD_D2 )
	{
		m_Ratio = 0.5f;
	}
	else if ( m_Frequency >= OP_THRESHOLD_D4 )
	{
		m_Ratio = 0.25f;
	}
}

void ARMor8Operator::setDetune (const int cents)
{
	m_Detune = cents;
	m_DetuneCached = powf( 2.0f, (m_Detune / 1200.0f) );
}

void ARMor8Operator::setAmplitude (const float amplitude)
{
	m_Amplitude = amplitude;
}

void ARMor8Operator::setFilterFreq (const float frequency)
{
	m_FilterCenterFreq = frequency;
}

void ARMor8Operator::setFilterRes (const float resonance)
{
	m_Filter->setResonance( resonance );
}

void ARMor8Operator::setRatio (const bool useRatio)
{
	m_UseRatio = useRatio;
}

void ARMor8Operator::setAmpVelSens (const float ampVelSens)
{
	m_AmpVelSens = ampVelSens;
}

void ARMor8Operator::setFiltVelSens (const float filtVelSens)
{
	m_FiltVelSens = filtVelSens;
}

void ARMor8Operator::setFrequencyOffset (const float freqOffset)
{
	m_FrequencyOffset = freqOffset;
}

void ARMor8Operator::setGlideTime (const float glideTime)
{
	m_GlideTime = glideTime; // since this is calculated 'per block'

	if ( glideTime <= 0.0f )
	{
		m_GlideNumSamples = 1;
	}
	else
	{
		m_GlideNumSamples = static_cast<unsigned int>( static_cast<float>(SAMPLE_RATE) * glideTime ) + 1;
	}
}

void ARMor8Operator::setGlideRetrigger (const bool useRetrigger)
{
	m_UseGlideRetrigger = useRetrigger;
}

void ARMor8Operator::setUseGlide (const bool useGlide)
{
	m_UseGlide = useGlide;
}

void ARMor8Operator::call (float* writeBuffer)
{
	this->cachePerBlockValues();

	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] += this->nextSample();
	}
}
