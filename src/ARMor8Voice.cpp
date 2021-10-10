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
	m_KeyEventServer(),
	m_Op1( &m_Osc1, &m_Eg1, &m_Filt1, 1.0f, 1000.0f ),
	m_Op2( &m_Osc2, &m_Eg2, &m_Filt2, 1.0f, 1000.0f ),
	m_Op3( &m_Osc3, &m_Eg3, &m_Filt3, 1.0f, 1000.0f ),
	m_Op4( &m_Osc4, &m_Eg4, &m_Filt4, 1.0f, 1000.0f ),
	m_Operators{ &m_Op1, &m_Op2, &m_Op3, &m_Op4 },
	m_ActiveKeyEvent()
{
	m_KeyEventServer.registerListener(&m_Op1);
	m_KeyEventServer.registerListener(&m_Op2);
	m_KeyEventServer.registerListener(&m_Op3);
	m_KeyEventServer.registerListener(&m_Op4);
}

ARMor8Voice::~ARMor8Voice()
{
}

void ARMor8Voice::setOperatorFreq (unsigned int opNum, float freq)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setFrequency( freq );
	}
}

void ARMor8Voice::setOperatorDetune (unsigned int opNum, int cents)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setDetune( cents );
	}
}

void ARMor8Voice::setOperatorWave (unsigned int opNum, const OscillatorMode& wave)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setWave( wave );
	}
}

void ARMor8Voice::setOperatorEGAttack (unsigned int opNum, float seconds, float expo)
{
	if ( opNum < numOps )
	{
		seconds *= PER_BLOCK_OFFSET;
		( (ADSREnvelopeGenerator<EG_RESPONSE>*) m_Operators[opNum]->getEnvelopeGenerator() )->setAttack( seconds, expo );
	}
}

void ARMor8Voice::setOperatorEGDecay (unsigned int opNum, float seconds, float expo)
{
	if ( opNum < numOps )
	{
		seconds *= PER_BLOCK_OFFSET;
		( (ADSREnvelopeGenerator<EG_RESPONSE>*) m_Operators[opNum]->getEnvelopeGenerator() )->setDecay( seconds, expo );
	}
}

void ARMor8Voice::setOperatorEGSustain (unsigned int opNum, float lvl)
{
	if ( opNum < numOps )
	{
		( (ADSREnvelopeGenerator<EG_RESPONSE>*) m_Operators[opNum]->getEnvelopeGenerator() )->setSustain( lvl );
	}
}

void ARMor8Voice::setOperatorEGRelease (unsigned int opNum, float seconds, float expo)
{
	if ( opNum < numOps )
	{
		seconds *= PER_BLOCK_OFFSET;
		( (ADSREnvelopeGenerator<EG_RESPONSE>*) m_Operators[opNum]->getEnvelopeGenerator() )->setRelease( seconds, expo );
	}
}

void ARMor8Voice::setOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest, const bool on)
{
	if ( opNum < numOps )
	{
		if ( on )
		{
			m_Operators[opNum]->setEGModDestination( modDest, true );
		}
		else
		{
			m_Operators[opNum]->setEGModDestination( modDest, false );
		}
	}
}

void ARMor8Voice::setOperatorModulation (unsigned int sourceOpNum, unsigned int destOpNum, float modulationAmount)
{
	if ( sourceOpNum < numOps && destOpNum < numOps )
	{
		m_Operators[destOpNum]->setModSourceAmplitude( m_Operators[sourceOpNum], modulationAmount );
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
	m_KeyEventServer.propagateKeyEvent( keyEvent );
}

void ARMor8Voice::onPitchEvent (const PitchEvent& pitchEvent)
{
	for (unsigned int op = 0; op < numOps; op++)
	{
		m_Operators[op]->onPitchEvent( pitchEvent );
	}
}

const KeyEvent& ARMor8Voice::getActiveKeyEvent()
{
	return m_ActiveKeyEvent;
}

void ARMor8Voice::setOperatorAmplitude (unsigned int opNum, const float amplitude)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setAmplitude( amplitude );
	}
}

void ARMor8Voice::setOperatorFilterFreq (unsigned int opNum, float frequency)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setFilterFreq( frequency );
	}
}

void ARMor8Voice::setOperatorFilterRes (unsigned int opNum, float resonance)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setFilterRes( resonance );
	}
}

void ARMor8Voice::setOperatorRatio (unsigned int opNum, bool useRatio)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setRatio( useRatio );
	}
}

void ARMor8Voice::setOperatorAmpVelSens (unsigned int opNum, float ampVelSens)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setAmpVelSens( ampVelSens );
	}
}

void ARMor8Voice::setOperatorFiltVelSens (unsigned int opNum, float filtVelSens)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setFiltVelSens( filtVelSens );
	}
}

void ARMor8Voice::setOperatorFrequencyOffset (unsigned int opNum, const float freqOffset)
{
	if ( opNum < numOps )
	{
		m_Operators[opNum]->setFrequencyOffset( freqOffset );
	}
}

void ARMor8Voice::setGlideTime (const float glideTime)
{
	for ( unsigned int op = 0; op < numOps; op++ )
	{
		m_Operators[op]->setGlideTime( glideTime );
	}
}

void ARMor8Voice::setGlideRetrigger (const bool useRetrigger)
{
	for ( unsigned int op = 0; op < numOps; op++ )
	{
		m_Operators[op]->setGlideRetrigger( useRetrigger );
	}
}

bool ARMor8Voice::getGlideRetrigger()
{
	return m_Operators[0]->getGlideRetrigger();
}

void ARMor8Voice::setUseGlide (const bool useGlide)
{
	for ( unsigned int op = 0; op < numOps; op++ )
	{
		m_Operators[op]->setUseGlide( useGlide );
	}
}

bool ARMor8Voice::getUseGlide()
{
	return m_Operators[0]->getUseGlide();
}

OscillatorMode ARMor8Voice::getOperatorWave (unsigned int opNum)
{
	return m_Operators[opNum]->getWave();
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
	switch ( opNum )
	{
		case 0:
			return m_Eg1.getAttack();
		case 1:
			return m_Eg2.getAttack();
		case 2:
			return m_Eg3.getAttack();
		case 3:
			return m_Eg4.getAttack();
		default:
			return 0.0f;
	}
}

float ARMor8Voice::getOperatorDecay (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_Eg1.getDecay();
		case 1:
			return m_Eg2.getDecay();
		case 2:
			return m_Eg3.getDecay();
		case 3:
			return m_Eg4.getDecay();
		default:
			return 0.0f;
	}
}

float ARMor8Voice::getOperatorRelease (unsigned int opNum)
{
	switch ( opNum )
	{
		case 0:
			return m_Eg1.getRelease();
		case 1:
			return m_Eg2.getRelease();
		case 2:
			return m_Eg3.getRelease();
		case 3:
			return m_Eg4.getRelease();
		default:
			return 0.0f;
	}
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
	if ( opNum < numOps )
	{
		return m_Operators[opNum]->getRatioFrequency();
	}

	return 1.0f;
}

bool ARMor8Voice::getOperatorUseRatio (unsigned int opNum)
{
	if ( opNum < numOps )
	{
		return m_Operators[opNum]->getRatio();
	}

	return false;
}

ARMor8VoiceState ARMor8Voice::getState()
{
	// operator 1 state
	ARMor8VoiceState state;
	state.frequency1 = m_Op1.getFrequency();
	state.useRatio1 = m_Op1.getRatio();
	state.wave1 = m_Op1.getWave();
	state.attack1 = m_Eg1.getAttack();
	state.attackExpo1 = m_AtkResponse1.getSlope();
	state.decay1 = m_Eg1.getDecay();
	state.decayExpo1 = m_DecResponse1.getSlope();
	state.sustain1 = m_Eg1.getSustain();
	state.release1 = m_Eg1.getRelease();
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
	state.attack2 = m_Eg2.getAttack();
	state.attackExpo2 = m_AtkResponse2.getSlope();
	state.decay2 = m_Eg2.getDecay();
	state.decayExpo2 = m_DecResponse2.getSlope();
	state.sustain2 = m_Eg2.getSustain();
	state.release2 = m_Eg2.getRelease();
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
	state.attack3 = m_Eg3.getAttack();
	state.attackExpo3 = m_AtkResponse3.getSlope();
	state.decay3 = m_Eg3.getDecay();
	state.decayExpo3 = m_DecResponse3.getSlope();
	state.sustain3 = m_Eg3.getSustain();
	state.release3 = m_Eg3.getRelease();
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
	state.attack4 = m_Eg4.getAttack();
	state.attackExpo4 = m_AtkResponse4.getSlope();
	state.decay4 = m_Eg4.getDecay();
	state.decayExpo4 = m_DecResponse4.getSlope();
	state.sustain4 = m_Eg4.getSustain();
	state.release4 = m_Eg4.getRelease();
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
	for ( unsigned int op = 0; op < numOps; op++ )
	{
		m_Operators[op]->setGlideRetrigger( state.glideRetrigger );
	}
}

void ARMor8Voice::call (float* writeBuffer)
{
	this->cachePerBlockValues();

	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] += this->nextSample();
	}
}
