#include "ARMor8Voice.hpp"

#include "AudioConstants.hpp"

static constexpr float PER_BLOCK_OFFSET = 1.0f / static_cast<float>( ABUFFER_SIZE );
static constexpr unsigned int numOps = 4;

ARMor8Voice::ARMor8Voice() :
	m_Osc1(),
	m_Osc2(),
	m_Osc3(),
	m_Osc4(),
	m_AtkResponse1(),
	m_AtkResponse2(),
	m_AtkResponse3(),
	m_AtkResponse4(),
	m_DecResponse1(),
	m_DecResponse2(),
	m_DecResponse3(),
	m_DecResponse4(),
	m_RelResponse1(),
	m_RelResponse2(),
	m_RelResponse3(),
	m_RelResponse4(),
	m_Eg1( 0.0f, 0.0f, 1.0f, 0.0f, &m_AtkResponse1, &m_DecResponse1, &m_RelResponse1 ),
	m_Eg2( 0.0f, 0.0f, 1.0f, 0.0f, &m_AtkResponse2, &m_DecResponse2, &m_RelResponse2 ),
	m_Eg3( 0.0f, 0.0f, 1.0f, 0.0f, &m_AtkResponse3, &m_DecResponse3, &m_RelResponse3 ),
	m_Eg4( 0.0f, 0.0f, 1.0f, 0.0f, &m_AtkResponse4, &m_DecResponse4, &m_RelResponse4 ),
	m_Filt1(),
	m_Filt2(),
	m_Filt3(),
	m_Filt4(),
	m_Op1( m_Osc1, m_Eg1, m_Filt1, 1.0f, 1000.0f, m_Op1, m_Op2, m_Op3, m_Op4 ),
	m_Op2( m_Osc2, m_Eg2, m_Filt2, 1.0f, 1000.0f, m_Op1, m_Op2, m_Op3, m_Op4 ),
	m_Op3( m_Osc3, m_Eg3, m_Filt3, 1.0f, 1000.0f, m_Op1, m_Op2, m_Op3, m_Op4 ),
	m_Op4( m_Osc4, m_Eg4, m_Filt4, 1.0f, 1000.0f, m_Op1, m_Op2, m_Op3, m_Op4 ),
	m_AttackTimes{ 0.0f },
	m_DecayTimes{ 0.0f },
	m_ReleaseTimes{ 0.0f },
	m_ActiveKeyEvent()
{
}

ARMor8Voice::~ARMor8Voice()
{
}

void ARMor8Voice::setOperatorFreq (unsigned int opNum, float freq)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setFrequency( freq );
			break;
		case 1:
			m_Op2.setFrequency( freq );
			break;
		case 2:
			m_Op3.setFrequency( freq );
			break;
		case 3:
			m_Op4.setFrequency( freq );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorDetune (unsigned int opNum, int cents)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setDetune( cents );
			break;
		case 1:
			m_Op2.setDetune( cents );
			break;
		case 2:
			m_Op3.setDetune( cents );
			break;
		case 3:
			m_Op4.setDetune( cents );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorWave (unsigned int opNum, const OscillatorMode& wave)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setWave( wave );
			break;
		case 1:
			m_Op2.setWave( wave );
			break;
		case 2:
			m_Op3.setWave( wave );
			break;
		case 3:
			m_Op4.setWave( wave );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorEGAttack (unsigned int opNum, float seconds, float expo)
{
	m_AttackTimes[opNum] = seconds;
	seconds *= PER_BLOCK_OFFSET;

	switch ( opNum )
	{
		case 0:
			m_Op1.getEnvelopeGenerator().setAttack( seconds, expo );
			break;
		case 1:
			m_Op2.getEnvelopeGenerator().setAttack( seconds, expo );
			break;
		case 2:
			m_Op3.getEnvelopeGenerator().setAttack( seconds, expo );
			break;
		case 3:
			m_Op4.getEnvelopeGenerator().setAttack( seconds, expo );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorEGDecay (unsigned int opNum, float seconds, float expo)
{
	m_DecayTimes[opNum] = seconds;
	seconds *= PER_BLOCK_OFFSET;

	switch ( opNum )
	{
		case 0:
			m_Op1.getEnvelopeGenerator().setDecay( seconds, expo );
			break;
		case 1:
			m_Op2.getEnvelopeGenerator().setDecay( seconds, expo );
			break;
		case 2:
			m_Op3.getEnvelopeGenerator().setDecay( seconds, expo );
			break;
		case 3:
			m_Op4.getEnvelopeGenerator().setDecay( seconds, expo );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorEGSustain (unsigned int opNum, float lvl)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.getEnvelopeGenerator().setSustain( lvl );
			break;
		case 1:
			m_Op2.getEnvelopeGenerator().setSustain( lvl );
			break;
		case 2:
			m_Op3.getEnvelopeGenerator().setSustain( lvl );
			break;
		case 3:
			m_Op4.getEnvelopeGenerator().setSustain( lvl );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorEGRelease (unsigned int opNum, float seconds, float expo)
{
	m_ReleaseTimes[opNum] = seconds;
	seconds *= PER_BLOCK_OFFSET;

	switch ( opNum )
	{
		case 0:
			m_Op1.getEnvelopeGenerator().setRelease( seconds, expo );
			break;
		case 1:
			m_Op2.getEnvelopeGenerator().setRelease( seconds, expo );
			break;
		case 2:
			m_Op3.getEnvelopeGenerator().setRelease( seconds, expo );
			break;
		case 3:
			m_Op4.getEnvelopeGenerator().setRelease( seconds, expo );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest, const bool on)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setEGModDestination( modDest, on );
			break;
		case 1:
			m_Op2.setEGModDestination( modDest, on );
			break;
		case 2:
			m_Op3.setEGModDestination( modDest, on );
			break;
		case 3:
			m_Op4.setEGModDestination( modDest, on );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorModulation (unsigned int sourceOpNum, unsigned int destOpNum, float modulationAmount)
{
	ARMor8Operator* sourceOp = nullptr;
	switch ( sourceOpNum )
	{
		case 0:
			sourceOp = &m_Op1;
			break;
		case 1:
			sourceOp = &m_Op2;
			break;
		case 2:
			sourceOp = &m_Op3;
			break;
		case 3:
			sourceOp = &m_Op4;
			break;
		default:
			break;
	}

	switch ( destOpNum )
	{
		case 0:
			m_Op1.setModSourceAmplitude( sourceOp, modulationAmount );
			break;
		case 1:
			m_Op2.setModSourceAmplitude( sourceOp, modulationAmount );
			break;
		case 2:
			m_Op3.setModSourceAmplitude( sourceOp, modulationAmount );
			break;
		case 3:
			m_Op4.setModSourceAmplitude( sourceOp, modulationAmount );
			break;
		default:
			break;
	}
}

float ARMor8Voice::nextSample()
{
	float output = 0.0f;
	output += m_Op1.nextSample();
	output += m_Op2.nextSample();
	output += m_Op3.nextSample();
	output += m_Op4.nextSample();

	return output;
}

void ARMor8Voice::cachePerBlockValues()
{
	m_Op1.cachePerBlockValues();
	m_Op2.cachePerBlockValues();
	m_Op3.cachePerBlockValues();
	m_Op4.cachePerBlockValues();
}

void ARMor8Voice::onKeyEvent (const KeyEvent& keyEvent)
{
	m_ActiveKeyEvent = keyEvent;
	m_Op1.onKeyEvent( keyEvent );
	m_Op2.onKeyEvent( keyEvent );
	m_Op3.onKeyEvent( keyEvent );
	m_Op4.onKeyEvent( keyEvent );
}

void ARMor8Voice::onPitchEvent (const PitchEvent& pitchEvent)
{
	m_Op1.onPitchEvent( pitchEvent );
	m_Op2.onPitchEvent( pitchEvent );
	m_Op3.onPitchEvent( pitchEvent );
	m_Op4.onPitchEvent( pitchEvent );
}

const KeyEvent& ARMor8Voice::getActiveKeyEvent()
{
	return m_ActiveKeyEvent;
}

void ARMor8Voice::setOperatorAmplitude (unsigned int opNum, const float amplitude)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setAmplitude( amplitude );
			break;
		case 1:
			m_Op2.setAmplitude( amplitude );
			break;
		case 2:
			m_Op3.setAmplitude( amplitude );
			break;
		case 3:
			m_Op4.setAmplitude( amplitude );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorFilterFreq (unsigned int opNum, float frequency)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setFilterFreq( frequency );
			break;
		case 1:
			m_Op2.setFilterFreq( frequency );
			break;
		case 2:
			m_Op3.setFilterFreq( frequency );
			break;
		case 3:
			m_Op4.setFilterFreq( frequency );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorFilterRes (unsigned int opNum, float resonance)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setFilterRes( resonance );
			break;
		case 1:
			m_Op2.setFilterRes( resonance );
			break;
		case 2:
			m_Op3.setFilterRes( resonance );
			break;
		case 3:
			m_Op4.setFilterRes( resonance );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorRatio (unsigned int opNum, bool useRatio)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setRatio( useRatio );
			break;
		case 1:
			m_Op2.setRatio( useRatio );
			break;
		case 2:
			m_Op3.setRatio( useRatio );
			break;
		case 3:
			m_Op4.setRatio( useRatio );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorAmpVelSens (unsigned int opNum, float ampVelSens)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setAmpVelSens( ampVelSens );
			break;
		case 1:
			m_Op2.setAmpVelSens( ampVelSens );
			break;
		case 2:
			m_Op3.setAmpVelSens( ampVelSens );
			break;
		case 3:
			m_Op4.setAmpVelSens( ampVelSens );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorFiltVelSens (unsigned int opNum, float filtVelSens)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setFiltVelSens( filtVelSens );
			break;
		case 1:
			m_Op2.setFiltVelSens( filtVelSens );
			break;
		case 2:
			m_Op3.setFiltVelSens( filtVelSens );
			break;
		case 3:
			m_Op4.setFiltVelSens( filtVelSens );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setOperatorFrequencyOffset (unsigned int opNum, const float freqOffset)
{
	switch ( opNum )
	{
		case 0:
			m_Op1.setFrequencyOffset( freqOffset );
			break;
		case 1:
			m_Op2.setFrequencyOffset( freqOffset );
			break;
		case 2:
			m_Op3.setFrequencyOffset( freqOffset );
			break;
		case 3:
			m_Op4.setFrequencyOffset( freqOffset );
			break;
		default:
			break;
	}
}

void ARMor8Voice::setGlideTime (const float glideTime)
{
	m_Op1.setGlideTime( glideTime );
	m_Op2.setGlideTime( glideTime );
	m_Op3.setGlideTime( glideTime );
	m_Op4.setGlideTime( glideTime );
}

void ARMor8Voice::setGlideRetrigger (const bool useRetrigger)
{
	m_Op1.setGlideRetrigger( useRetrigger );
	m_Op2.setGlideRetrigger( useRetrigger );
	m_Op3.setGlideRetrigger( useRetrigger );
	m_Op4.setGlideRetrigger( useRetrigger );
}

bool ARMor8Voice::getGlideRetrigger()
{
	return m_Op1.getGlideRetrigger();
}

void ARMor8Voice::setUseGlide (const bool useGlide)
{
	m_Op1.setUseGlide( useGlide );
	m_Op2.setUseGlide( useGlide );
	m_Op3.setUseGlide( useGlide );
	m_Op4.setUseGlide( useGlide );
}

bool ARMor8Voice::getUseGlide()
{
	return m_Op1.getUseGlide();
}

OscillatorMode ARMor8Voice::getOperatorWave (unsigned int opNum)
{
	return m_Op1.getWave();
}

bool ARMor8Voice::getOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest)
{
	ARMor8Operator* op = nullptr;

	switch ( opNum )
	{
		case 0:
			op = &m_Op1;

			break;
		case 1:
			op = &m_Op2;

			break;
		case 2:
			op = &m_Op3;

			break;
		case 3:
			op = &m_Op4;

			break;
		default:
			return false;
	}

	switch ( modDest )
	{
		case EGModDestination::AMPLITUDE:
			return op->egModAmplitudeSet();
		case EGModDestination::FREQUENCY:
			return op->egModFrequencySet();
		case EGModDestination::FILT_FREQUENCY:
			return op->egModFilterSet();
		default:
			return false;
	}
}

float ARMor8Voice::getOperatorAttack (unsigned int opNum)
{
	return m_AttackTimes[opNum];
}

float ARMor8Voice::getOperatorDecay (unsigned int opNum)
{
	return m_DecayTimes[opNum];
}

float ARMor8Voice::getOperatorRelease (unsigned int opNum)
{
	return m_ReleaseTimes[opNum];
}

float ARMor8Voice::getOperatorAttackExpo (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_AtkResponse1.getSlope();
		case 1:
			return m_AtkResponse2.getSlope();
		case 2:
			return m_AtkResponse3.getSlope();
		case 3:
			return m_AtkResponse4.getSlope();
		default:
			return 0.0f;
	}
}

float ARMor8Voice::getOperatorDecayExpo (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_DecResponse1.getSlope();
		case 1:
			return m_DecResponse2.getSlope();
		case 2:
			return m_DecResponse3.getSlope();
		case 3:
			return m_DecResponse4.getSlope();
		default:
			return 0.0f;
	}
}

float ARMor8Voice::getOperatorReleaseExpo (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_RelResponse1.getSlope();
		case 1:
			return m_RelResponse2.getSlope();
		case 2:
			return m_RelResponse3.getSlope();
		case 3:
			return m_RelResponse4.getSlope();
		default:
			return 0.0f;
	}
}

float ARMor8Voice::getOperatorRatioFrequency (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_Op1.getRatioFrequency();
		case 1:
			return m_Op2.getRatioFrequency();
		case 2:
			return m_Op3.getRatioFrequency();
		case 3:
			return m_Op4.getRatioFrequency();
		default:
			return 0.0f;
	}
}

bool ARMor8Voice::getOperatorUseRatio (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_Op1.getRatio();
		case 1:
			return m_Op2.getRatio();
		case 2:
			return m_Op3.getRatio();
		case 3:
			return m_Op4.getRatio();
		default:
			return false;
	}
}

ARMor8VoiceState ARMor8Voice::getState()
{
	// operator 1 state
	ARMor8VoiceState state;
	state.frequency1 = m_Op1.getFrequency();
	state.useRatio1 = m_Op1.getRatio();
	state.wave1 = m_Op1.getWave();
	state.attack1 = m_AttackTimes[0];
	state.attackExpo1 = m_AtkResponse1.getSlope();
	state.decay1 = m_DecayTimes[0];
	state.decayExpo1 = m_DecResponse1.getSlope();
	state.sustain1 = m_Eg1.getSustain();
	state.release1 = m_ReleaseTimes[0];
	state.releaseExpo1 = m_RelResponse1.getSlope();
	state.egAmplitudeMod1 = m_Op1.egModAmplitudeSet();
	state.egFrequencyMod1 = m_Op1.egModFrequencySet();
	state.egFilterMod1 = m_Op1.egModFilterSet();
	state.op1ModAmount1 = m_Op1.getModulationAmount( &m_Op1 );
	state.op2ModAmount1 = m_Op1.getModulationAmount( &m_Op2 );
	state.op3ModAmount1 = m_Op1.getModulationAmount( &m_Op3 );
	state.op4ModAmount1 = m_Op1.getModulationAmount( &m_Op4 );
	state.amplitude1 = m_Op1.getAmplitude();
	state.filterFreq1 = m_Op1.getFilterFreq();
	state.filterRes1 = m_Op1.getFilterRes();
	state.ampVelSens1 = m_Op1.getAmpVelSens();
	state.filtVelSens1 = m_Op1.getFiltVelSens();
	state.detune1 = m_Op1.getDetune();

	// operator 2 state
	state.frequency2 = m_Op2.getFrequency();
	state.useRatio2 = m_Op2.getRatio();
	state.wave2 = m_Op2.getWave();
	state.attack2 = m_AttackTimes[1];
	state.attackExpo2 = m_AtkResponse2.getSlope();
	state.decay2 = m_DecayTimes[1];
	state.decayExpo2 = m_DecResponse2.getSlope();
	state.sustain2 = m_Eg2.getSustain();
	state.release2 = m_ReleaseTimes[1];
	state.releaseExpo2 = m_RelResponse2.getSlope();
	state.egAmplitudeMod2 = m_Op2.egModAmplitudeSet();
	state.egFrequencyMod2 = m_Op2.egModFrequencySet();
	state.egFilterMod2 = m_Op2.egModFilterSet();
	state.op1ModAmount2 = m_Op2.getModulationAmount( &m_Op1 );
	state.op2ModAmount2 = m_Op2.getModulationAmount( &m_Op2 );
	state.op3ModAmount2 = m_Op2.getModulationAmount( &m_Op3 );
	state.op4ModAmount2 = m_Op2.getModulationAmount( &m_Op4 );
	state.amplitude2 = m_Op2.getAmplitude();
	state.filterFreq2 = m_Op2.getFilterFreq();
	state.filterRes2 = m_Op2.getFilterRes();
	state.ampVelSens2 = m_Op2.getAmpVelSens();
	state.filtVelSens2 = m_Op2.getFiltVelSens();
	state.detune2 = m_Op2.getDetune();

	// operator 3 state
	state.frequency3 = m_Op3.getFrequency();
	state.useRatio3 = m_Op3.getRatio();
	state.wave3 = m_Op3.getWave();
	state.attack3 = m_AttackTimes[2];
	state.attackExpo3 = m_AtkResponse3.getSlope();
	state.decay3 = m_DecayTimes[2];
	state.decayExpo3 = m_DecResponse3.getSlope();
	state.sustain3 = m_Eg3.getSustain();
	state.release3 = m_ReleaseTimes[2];
	state.releaseExpo3 = m_RelResponse3.getSlope();
	state.egAmplitudeMod3 = m_Op3.egModAmplitudeSet();
	state.egFrequencyMod3 = m_Op3.egModFrequencySet();
	state.egFilterMod3 = m_Op3.egModFilterSet();
	state.op1ModAmount3 = m_Op3.getModulationAmount( &m_Op1 );
	state.op2ModAmount3 = m_Op3.getModulationAmount( &m_Op2 );
	state.op3ModAmount3 = m_Op3.getModulationAmount( &m_Op3 );
	state.op4ModAmount3 = m_Op3.getModulationAmount( &m_Op4 );
	state.amplitude3 = m_Op3.getAmplitude();
	state.filterFreq3 = m_Op3.getFilterFreq();
	state.filterRes3 = m_Op3.getFilterRes();
	state.ampVelSens3 = m_Op3.getAmpVelSens();
	state.filtVelSens3 = m_Op3.getFiltVelSens();
	state.detune3 = m_Op3.getDetune();

	// operator 4 state
	state.frequency4 = m_Op4.getFrequency();
	state.useRatio4 = m_Op4.getRatio();
	state.wave4 = m_Op4.getWave();
	state.attack4 = m_AttackTimes[3];
	state.attackExpo4 = m_AtkResponse4.getSlope();
	state.decay4 = m_DecayTimes[3];
	state.decayExpo4 = m_DecResponse4.getSlope();
	state.sustain4 = m_Eg4.getSustain();
	state.release4 = m_ReleaseTimes[3];
	state.releaseExpo4 = m_RelResponse4.getSlope();
	state.egAmplitudeMod4 = m_Op4.egModAmplitudeSet();
	state.egFrequencyMod4 = m_Op4.egModFrequencySet();
	state.egFilterMod4 = m_Op4.egModFilterSet();
	state.op1ModAmount4 = m_Op4.getModulationAmount( &m_Op1 );
	state.op2ModAmount4 = m_Op4.getModulationAmount( &m_Op2 );
	state.op3ModAmount4 = m_Op4.getModulationAmount( &m_Op3 );
	state.op4ModAmount4 = m_Op4.getModulationAmount( &m_Op4 );
	state.amplitude4 = m_Op4.getAmplitude();
	state.filterFreq4 = m_Op4.getFilterFreq();
	state.filterRes4 = m_Op4.getFilterRes();
	state.ampVelSens4 = m_Op4.getAmpVelSens();
	state.filtVelSens4 = m_Op4.getFiltVelSens();
	state.detune4 = m_Op4.getDetune();

	// global states
	state.glideTime = m_Op1.getGlideTime();
	state.glideRetrigger = m_Op1.getGlideRetrigger();

	return state;
}

void ARMor8Voice::setState (const ARMor8VoiceState& state)
{
	// operator 1 state
	m_Op1.setUseGlide( true );
	m_Op1.setFrequency( state.frequency1 );
	m_Op1.setRatio( state.useRatio1 );
	m_Op1.setWave( state.wave1 );
	this->setOperatorEGAttack( 0, state.attack1, state.attackExpo1 );
	this->setOperatorEGDecay( 0, state.decay1, state.decayExpo1 );
	m_Eg1.setSustain( state.sustain1 );
	this->setOperatorEGRelease( 0, state.release1, state.releaseExpo1 );
	if ( state.egAmplitudeMod1 )
	{
		m_Op1.setEGModDestination( EGModDestination::AMPLITUDE, true );
	}
	else
	{
		m_Op1.setEGModDestination( EGModDestination::AMPLITUDE, false );
	}
	if ( state.egFrequencyMod1 )
	{
		m_Op1.setEGModDestination( EGModDestination::FREQUENCY, true );
	}
	else
	{
		m_Op1.setEGModDestination( EGModDestination::FREQUENCY, false );
	}
	if ( state.egFilterMod1 )
	{
		m_Op1.setEGModDestination( EGModDestination::FILT_FREQUENCY, true );
	}
	else
	{
		m_Op1.setEGModDestination( EGModDestination::FILT_FREQUENCY, false );
	}
	m_Op1.setModSourceAmplitude( &m_Op1, state.op1ModAmount1 );
	m_Op1.setModSourceAmplitude( &m_Op2, state.op2ModAmount1 );
	m_Op1.setModSourceAmplitude( &m_Op3, state.op3ModAmount1 );
	m_Op1.setModSourceAmplitude( &m_Op4, state.op4ModAmount1 );
	m_Op1.setAmplitude( state.amplitude1 );
	m_Op1.setFilterFreq( state.filterFreq1 );
	m_Op1.setFilterRes( state.filterRes1 );
	m_Op1.setAmpVelSens( state.ampVelSens1 );
	m_Op1.setFiltVelSens( state.filtVelSens1 );
	m_Op1.setDetune( state.detune1 );

	// operator 2 state
	m_Op2.setUseGlide( true );
	m_Op2.setFrequency( state.frequency2 );
	m_Op2.setRatio( state.useRatio2 );
	m_Op2.setWave( state.wave2 );
	this->setOperatorEGAttack( 1,  state.attack2, state.attackExpo2 );
	this->setOperatorEGDecay( 1, state.decay2, state.decayExpo2 );
	m_Eg2.setSustain( state.sustain2 );
	this->setOperatorEGRelease( 1, state.release2, state.releaseExpo2 );
	if ( state.egAmplitudeMod2 )
	{
		m_Op2.setEGModDestination( EGModDestination::AMPLITUDE, true );
	}
	else
	{
		m_Op2.setEGModDestination( EGModDestination::AMPLITUDE, false );
	}
	if ( state.egFrequencyMod2 )
	{
		m_Op2.setEGModDestination( EGModDestination::FREQUENCY, true );
	}
	else
	{
		m_Op2.setEGModDestination( EGModDestination::FREQUENCY, false );
	}
	if ( state.egFilterMod2 )
	{
		m_Op2.setEGModDestination( EGModDestination::FILT_FREQUENCY, true );
	}
	else
	{
		m_Op2.setEGModDestination( EGModDestination::FILT_FREQUENCY, false );
	}
	m_Op2.setModSourceAmplitude( &m_Op1, state.op1ModAmount2 );
	m_Op2.setModSourceAmplitude( &m_Op2, state.op2ModAmount2 );
	m_Op2.setModSourceAmplitude( &m_Op3, state.op3ModAmount2 );
	m_Op2.setModSourceAmplitude( &m_Op4, state.op4ModAmount2 );
	m_Op2.setAmplitude( state.amplitude2 );
	m_Op2.setFilterFreq( state.filterFreq2 );
	m_Op2.setFilterRes( state.filterRes2 );
	m_Op2.setAmpVelSens( state.ampVelSens2 );
	m_Op2.setFiltVelSens( state.filtVelSens2 );
	m_Op2.setDetune( state.detune2 );

	// operator 3 state
	m_Op3.setUseGlide( true );
	m_Op3.setFrequency( state.frequency3 );
	m_Op3.setRatio( state.useRatio3 );
	m_Op3.setWave( state.wave3 );
	this->setOperatorEGAttack( 2, state.attack3, state.attackExpo3 );
	this->setOperatorEGDecay( 2, state.decay3, state.decayExpo3 );
	m_Eg3.setSustain( state.sustain3 );
	this->setOperatorEGRelease( 2, state.release3, state.releaseExpo3 );
	if ( state.egAmplitudeMod3 )
	{
		m_Op3.setEGModDestination( EGModDestination::AMPLITUDE, true );
	}
	else
	{
		m_Op3.setEGModDestination( EGModDestination::AMPLITUDE, false );
	}
	if ( state.egFrequencyMod3 )
	{
		m_Op3.setEGModDestination( EGModDestination::FREQUENCY, true );
	}
	else
	{
		m_Op3.setEGModDestination( EGModDestination::FREQUENCY, false );
	}
	if ( state.egFilterMod3 )
	{
		m_Op3.setEGModDestination( EGModDestination::FILT_FREQUENCY, true );
	}
	else
	{
		m_Op3.setEGModDestination( EGModDestination::FILT_FREQUENCY, false );
	}
	m_Op3.setModSourceAmplitude( &m_Op1, state.op1ModAmount3 );
	m_Op3.setModSourceAmplitude( &m_Op2, state.op2ModAmount3 );
	m_Op3.setModSourceAmplitude( &m_Op3, state.op3ModAmount3 );
	m_Op3.setModSourceAmplitude( &m_Op4, state.op4ModAmount3 );
	m_Op3.setAmplitude( state.amplitude3 );
	m_Op3.setFilterFreq( state.filterFreq3 );
	m_Op3.setFilterRes( state.filterRes3 );
	m_Op3.setAmpVelSens( state.ampVelSens3 );
	m_Op3.setFiltVelSens( state.filtVelSens3 );
	m_Op3.setDetune( state.detune3 );

	// operator 4 state
	m_Op4.setUseGlide( true );
	m_Op4.setFrequency( state.frequency4 );
	m_Op4.setRatio( state.useRatio4 );
	m_Op4.setWave( state.wave4 );
	this->setOperatorEGAttack( 3, state.attack4, state.attackExpo4 );
	this->setOperatorEGDecay( 3, state.decay4, state.decayExpo4 );
	m_Eg4.setSustain( state.sustain4 );
	this->setOperatorEGRelease( 3, state.release4, state.releaseExpo4 );
	if ( state.egAmplitudeMod4 )
	{
		m_Op4.setEGModDestination( EGModDestination::AMPLITUDE, true );
	}
	else
	{
		m_Op4.setEGModDestination( EGModDestination::AMPLITUDE, false );
	}
	if ( state.egFrequencyMod4 )
	{
		m_Op4.setEGModDestination( EGModDestination::FREQUENCY, true );
	}
	else
	{
		m_Op4.setEGModDestination( EGModDestination::FREQUENCY, false );
	}
	if ( state.egFilterMod4 )
	{
		m_Op4.setEGModDestination( EGModDestination::FILT_FREQUENCY, true );
	}
	else
	{
		m_Op4.setEGModDestination( EGModDestination::FILT_FREQUENCY, false );
	}
	m_Op4.setModSourceAmplitude( &m_Op1, state.op1ModAmount4 );
	m_Op4.setModSourceAmplitude( &m_Op2, state.op2ModAmount4 );
	m_Op4.setModSourceAmplitude( &m_Op3, state.op3ModAmount4 );
	m_Op4.setModSourceAmplitude( &m_Op4, state.op4ModAmount4 );
	m_Op4.setAmplitude( state.amplitude4 );
	m_Op4.setFilterFreq( state.filterFreq4 );
	m_Op4.setFilterRes( state.filterRes4 );
	m_Op4.setAmpVelSens( state.ampVelSens4 );
	m_Op4.setFiltVelSens( state.filtVelSens4 );
	m_Op4.setDetune( state.detune4 );

	// global states
	this->setGlideTime( state.glideTime );
	this->setGlideRetrigger( state.glideRetrigger );
}

void ARMor8Voice::call (float* writeBuffer)
{
	this->cachePerBlockValues();

	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] += this->nextSample();
	}
}
