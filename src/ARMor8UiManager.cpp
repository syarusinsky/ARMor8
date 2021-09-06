#include "ARMor8UiManager.hpp"

#include "AudioConstants.hpp"
#include "ARMor8Constants.hpp"
#include "Graphics.hpp"
#include "Sprite.hpp"
#include "IARMor8LCDRefreshEventListener.hpp"
#include "IPotEventListener.hpp"
#include "IButtonEventListener.hpp"

ARMor8UiManager::ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	Surface( width, height, format ),
	m_Logo( nullptr ),
	m_CurrentMenu( ARMOR8_MENUS::LOADING ),
	m_CurrentPresetNum( 1 ),
	m_OpCurrentlyBeingEdited( 1 ),
	m_WaveNumCurrentlyBeingEdited( 1 ),
	m_EGDestBitmask( 0b000 ),
	m_UsingRatio( false ),
	m_UsingGlideRetrigger( false ),
	m_UsingMono( false ),
	m_PrstAndOpStr{ "[PRST: XX][OP: X]" },
	m_AttackStr{ "ATK:X.XXX" },
	m_DecayStr{ "DEC:X.XXX" },
	m_SustainStr{ "SUS:X.XXX" },
	m_ReleaseStr{ "REL:X.XXX" },
	m_Op1Str{ "OP1:X.XXX" },
	m_Op2Str{ "OP2:X.XXX" },
	m_Op3Str{ "OP3:X.XXX" },
	m_Op4Str{ "OP4:X.XXX" },
	m_OpAmpStr{ "AMPL: X.XXX" },
	m_FreqStr{ "FREQ: XXXXX" },
	m_FiltFreqStr{ "FILT: XXXXX" },
	m_MonoPolyStr{ "[XXXX]" },
	m_WaveStr{ "SINE" },
	m_RatioFixedStr{ "[XXXXX]" },
	m_DetuneStr{ "DETUNE:XXXXX CENTS" },
	m_AttackExpoStr{ "ATKEXP:X.XX" },
	m_DecayExpoStr{ "DECEXP:X.XX" },
	m_ReleaseExpoStr{ "RELEXP:X.XX" },
	m_AmplitudeVelStr{ "AMPVEL:X.XX" },
	m_FiltVelStr{ "FLTVEL:X.XX" },
	m_GlideStr{ "GLIDE TIME: X.XXX" },
	m_PitchBendStr{ "PITCH BEND:    XX" },
	m_FiltResStr{ "X.XX" },
	m_FreqPotCached( 0.0f ),
	m_DetunePotCached( 0.0f ),
	m_AttackPotCached( 0.0f ),
	m_AttackExpoPotCached( 0.0f ),
	m_Op1ModPotCached( 0.0f ),
	m_DecayPotCached( 0.0f ),
	m_DecayExpoPotCached( 0.0f ),
	m_Op2ModPotCached( 0.0f ),
	m_SustainPotCached( 0.0f ),
	m_Op3ModPotCached( 0.0f ),
	m_ReleasePotCached( 0.0f ),
	m_ReleaseExpoPotCached( 0.0f ),
	m_Op4ModPotCached( 0.0f ),
	m_AmplitudePotCached( 0.0f ),
	m_AmplitudeVelPotCached( 0.0f ),
	m_FiltFreqPotCached( 0.0f ),
	m_FiltResPotCached( 0.0f ),
	m_FiltVelPotCached( 0.0f ),
	m_PitchBendPotCached( 0.0f ),
	m_GlidePotCached( 0.0f ),
	m_FreqPotLocked( false ),
	m_DetunePotLocked( false ),
	m_AttackPotLocked( false ),
	m_AttackExpoPotLocked( false ),
	m_Op1ModPotLocked( false ),
	m_DecayPotLocked( false ),
	m_DecayExpoPotLocked( false ),
	m_Op2ModPotLocked( false ),
	m_SustainPotLocked( false ),
	m_Op3ModPotLocked( false ),
	m_ReleasePotLocked( false ),
	m_ReleaseExpoPotLocked( false ),
	m_Op4ModPotLocked( false ),
	m_AmplitudePotLocked( false ),
	m_AmplitudeVelPotLocked( false ),
	m_FiltFreqPotLocked( false ),
	m_FiltResPotLocked( false ),
	m_FiltVelPotLocked( false ),
	m_PitchBendPotLocked( false ),
	m_GlidePotLocked( false ),
	m_Alt1State( BUTTON_STATE::FLOATING ),
	m_Alt2State( BUTTON_STATE::FLOATING ),
	m_RatioOrFixedBtnState( BUTTON_STATE::FLOATING ),
	m_NextOpBtnState( BUTTON_STATE::FLOATING ),
	m_NextWaveBtnState( BUTTON_STATE::FLOATING ),
	m_GlideRetrigBtnState( BUTTON_STATE::FLOATING ),
	m_MonoBtnState( BUTTON_STATE::FLOATING ),
	m_EGDestBtnState( BUTTON_STATE::FLOATING ),
	m_PrevPresetBtnState( BUTTON_STATE::FLOATING ),
	m_NextPresetBtnState( BUTTON_STATE::FLOATING ),
	m_WritePresetBtnState( BUTTON_STATE::FLOATING )
{
	this->bindToARMor8PresetEventSystem();
	this->bindToARMor8ParameterEventSystem();
}

ARMor8UiManager::~ARMor8UiManager()
{
}

void ARMor8UiManager::setFont (Font* font)
{
	m_Graphics->setFont( font );
}

void ARMor8UiManager::setLogo (Sprite* logo)
{
	m_Logo = logo;
}

void ARMor8UiManager::draw()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::LOADING )
	{
		m_Graphics->setColor( false );
		m_Graphics->fill();

		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.25f, 0.9f, "LOADING...", 1.0f );

		m_Graphics->drawSprite( 0.4f, 0.05f, *m_Logo );

		m_Logo->setRotationAngle( m_Logo->getRotationAngle() + 5 );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
	{
		m_Graphics->setColor( false );
		m_Graphics->fill();

		m_Graphics->setColor( true );

		m_Graphics->drawText( 0.0f, 0.0f, m_PrstAndOpStr, 1.0f );

		m_Graphics->drawLine( 0.0f, 0.12f, 1.0f, 0.12f );
		m_Graphics->drawLine( 0.0f, 0.14f, 1.0f, 0.14f );

		m_Graphics->drawText( -0.02f, 0.16f, m_AttackStr,  1.0f );
		m_Graphics->drawText( -0.02f, 0.28f, m_DecayStr,   1.0f );
		m_Graphics->drawText( -0.02f, 0.39f, m_SustainStr, 1.0f );
		m_Graphics->drawText( -0.02f, 0.50f, m_ReleaseStr, 1.0f );

		m_Graphics->drawLine( 0.5f, 0.14f, 0.5f, 0.6f );

		m_Graphics->drawText( 0.52f, 0.16f, m_Op1Str, 1.0f );
		m_Graphics->drawText( 0.52f, 0.28f, m_Op2Str, 1.0f );
		m_Graphics->drawText( 0.52f, 0.39f, m_Op3Str, 1.0f );
		m_Graphics->drawText( 0.52f, 0.50f, m_Op4Str, 1.0f );

		m_Graphics->drawLine( 0.1f, 0.61f, 0.9f, 0.61f );

		m_Graphics->drawText( -0.02f, 0.65f, m_OpAmpStr, 1.0f );

		m_Graphics->drawText( -0.02f, 0.74f, m_FreqStr, 1.0f );

		m_Graphics->drawText( -0.02f, 0.83f, m_FiltFreqStr, 1.0f );

		m_Graphics->drawLine( 0.61f, 0.63f, 0.61f, 0.9f );

		m_Graphics->drawText( 0.63f, 0.65f, "EGAMP", 1.0f );

		m_Graphics->drawText( 0.63f, 0.74f, "EGFRQ", 1.0f );

		m_Graphics->drawText( 0.63f, 0.83f, "EGFLT", 1.0f );

		// eg destination amplitude
		if ( m_EGDestBitmask & 0b100 )
		{
			m_Graphics->drawCircle( 0.96f, 0.67f, 0.02f );
			m_Graphics->drawCircleFilled( 0.96f, 0.67f, 0.02f );
		}
		else
		{
			m_Graphics->drawCircle( 0.96f, 0.67f, 0.02f );
		}

		// eg destination frequency
		if ( m_EGDestBitmask & 0b010 )
		{
			m_Graphics->drawCircle( 0.96f, 0.77f, 0.02f );
			m_Graphics->drawCircleFilled( 0.96f, 0.77f, 0.02f );
		}
		else
		{
			m_Graphics->drawCircle( 0.96f, 0.77f, 0.02f );
		}

		// eg destination filter
		if ( m_EGDestBitmask & 0b001 )
		{
			m_Graphics->drawCircle( 0.96f, 0.87f, 0.02f );
			m_Graphics->drawCircleFilled( 0.96f, 0.87f, 0.02f );
		}
		else
		{
			m_Graphics->drawCircle( 0.96f, 0.87f, 0.02f );
		}

		m_Graphics->drawText( 0.0f, 0.93f, m_MonoPolyStr, 1.0f );

		m_Graphics->drawText( 0.37f, 0.94f, m_WaveStr, 1.0f );

		m_Graphics->drawText( 0.625f, 0.93f, m_RatioFixedStr, 1.0f );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
	{
		m_Graphics->setColor( false );
		m_Graphics->fill();

		m_Graphics->setColor( true );

		m_Graphics->drawText( -0.02f, 0.0f, m_DetuneStr, 1.0f );

		m_Graphics->drawLine( 0.0f, 0.11f, 1.0f, 0.11f );

		m_Graphics->drawText( -0.02f, 0.16f, m_AttackExpoStr, 1.0f );
		m_Graphics->drawText( -0.02f, 0.30f, m_DecayExpoStr, 1.0f );
		m_Graphics->drawText( -0.02f, 0.42f, m_ReleaseExpoStr, 1.0f );

		m_Graphics->drawLine( 0.0f, 0.55f, 0.62f, 0.55f );
		m_Graphics->drawLine( 0.62f, 0.25f, 0.62f, 0.69f );

		m_Graphics->drawText( -0.02f, 0.60f, m_AmplitudeVelStr, 1.0f );
		m_Graphics->drawText( -0.02f, 0.72f, m_FiltVelStr, 1.0f );

		m_Graphics->drawLine( 0.7f, 0.80f, 1.0f, 0.80f );

		m_Graphics->drawText( 0.03f, 0.83f, m_GlideStr, 1.0f );
		m_Graphics->drawText( 0.03f, 0.93f, m_PitchBendStr, 1.0f );

		m_Graphics->drawText( 0.63f, 0.2f, "FLTRES:", 1.0f );
		m_Graphics->drawText( 0.71f, 0.3f, m_FiltResStr, 1.0f );

		m_Graphics->drawText( 0.63f, 0.48f, "GLDRETR", 1.0f );
		if ( m_UsingGlideRetrigger )
		{
			m_Graphics->drawCircle( 0.81f, 0.67f, 0.03f );
			m_Graphics->drawCircleFilled( 0.81f, 0.67f, 0.03f );
		}
		else
		{
			m_Graphics->drawCircle( 0.81f, 0.67f, 0.03f );
		}
	}

	IARMor8LCDRefreshEventListener::PublishEvent(
			ARMor8LCDRefreshEvent(0, 0, this->getFrameBuffer()->getWidth(), this->getFrameBuffer()->getHeight(), 0) );
}

void ARMor8UiManager::drawLoadingLogo()
{
	m_Graphics->setColor( false );

	m_Graphics->drawBoxFilled( 0.3f, 0.05f, 0.7f, 0.8f );

	m_Graphics->setColor( true );
	m_Graphics->drawSprite( 0.4f, 0.05f, *m_Logo );

	m_Logo->setRotationAngle( m_Logo->getRotationAngle() + 5 );

	this->publishPartialLCDRefreshEvent( 0.3f, 0.05f, 0.7f, 0.8f );
}

void ARMor8UiManager::tickForChangingBackToStatus()
{
	if ( m_CurrentMenu != ARMOR8_MENUS::STATUS )
	{
		if ( m_TicksForChangingBackToStatus >= m_MaxTicksForChangingBackToStatus )
		{
			m_CurrentMenu = ARMOR8_MENUS::STATUS;
			this->draw();
			m_TicksForChangingBackToStatus = 0;
		}

		m_TicksForChangingBackToStatus++;
	}
}

void ARMor8UiManager::onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent)
{
	this->lockAllPots();

	ARMor8VoiceState voiceState = presetEvent.getPreset();
	m_OpCurrentlyBeingEdited = presetEvent.getOpToEdit() + 1;
	m_CurrentPresetNum = presetEvent.getPresetNum() + 1;

	// buffer for holding parameter strings
	const unsigned int bufferLen = 20;
	char buffer[bufferLen];

	this->updateOpNumberStr( buffer, bufferLen );
	this->updatePrstNumberStr( buffer, bufferLen);

	float amplitude     = 0.0f;
	float frequency     = 0.0f;
	int   detune        = 0;
	float filtFrequency = 0.0f;
	float attackAmount  = 0.0f;
	float decayAmount   = 0.0f;
	float sustainAmount = 0.0f;
	float releaseAmount = 0.0f;
	float attackExpo    = 0.0f;
	float decayExpo     = 0.0f;
	float releaseExpo   = 0.0f;
	float op1ModAmount  = 0.0f;
	float op2ModAmount  = 0.0f;
	float op3ModAmount  = 0.0f;
	float op4ModAmount  = 0.0f;
	bool  egDestAmpl    = false;
	bool  egDestFreq    = false;
	bool  egDestFilt    = false;
	bool  usingRatio    = false;
	bool  usingGlide    = voiceState.glideRetrigger;
	bool  usingMono     = voiceState.monophonic;
	float amplitudeVel  = 0.0f;
	float filterVel     = 0.0f;
	float glideTime     = voiceState.glideTime;
	OscillatorMode wave = OscillatorMode::SINE;
	unsigned int pitchBendSemitones = voiceState.pitchBendSemitones;
	float filterRes     = 0.0f;

	switch ( m_OpCurrentlyBeingEdited )
	{
		case 1:
			amplitude     = voiceState.amplitude1;
			frequency     = voiceState.frequency1;
			detune        = voiceState.detune1;
			filtFrequency = voiceState.filterFreq1;
			attackAmount  = voiceState.attack1;
			decayAmount   = voiceState.decay1;
			sustainAmount = voiceState.sustain1;
			releaseAmount = voiceState.release1;
			attackExpo    = voiceState.attackExpo1;
			decayExpo     = voiceState.decayExpo1;
			releaseExpo   = voiceState.releaseExpo1;
			op1ModAmount  = voiceState.op1ModAmount1;
			op2ModAmount  = voiceState.op2ModAmount1;
			op3ModAmount  = voiceState.op3ModAmount1;
			op4ModAmount  = voiceState.op4ModAmount1;
			egDestAmpl    = voiceState.egAmplitudeMod1;
			egDestFreq    = voiceState.egFrequencyMod1;
			egDestFilt    = voiceState.egFilterMod1;
			usingRatio    = voiceState.useRatio1;
			amplitudeVel  = voiceState.ampVelSens1;
			filterVel     = voiceState.filtVelSens1;
			wave          = voiceState.wave1;
			filterRes     = voiceState.filterRes1;

			break;
		case 2:
			amplitude     = voiceState.amplitude2;
			frequency     = voiceState.frequency2;
			detune        = voiceState.detune2;
			filtFrequency = voiceState.filterFreq2;
			attackAmount  = voiceState.attack2;
			decayAmount   = voiceState.decay2;
			sustainAmount = voiceState.sustain2;
			releaseAmount = voiceState.release2;
			attackExpo    = voiceState.attackExpo2;
			decayExpo     = voiceState.decayExpo2;
			releaseExpo   = voiceState.releaseExpo2;
			op1ModAmount  = voiceState.op1ModAmount2;
			op2ModAmount  = voiceState.op2ModAmount2;
			op3ModAmount  = voiceState.op3ModAmount2;
			op4ModAmount  = voiceState.op4ModAmount2;
			egDestAmpl    = voiceState.egAmplitudeMod2;
			egDestFreq    = voiceState.egFrequencyMod2;
			egDestFilt    = voiceState.egFilterMod2;
			usingRatio    = voiceState.useRatio2;
			amplitudeVel  = voiceState.ampVelSens2;
			filterVel     = voiceState.filtVelSens2;
			wave          = voiceState.wave2;
			filterRes     = voiceState.filterRes2;

			break;
		case 3:
			amplitude     = voiceState.amplitude3;
			frequency     = voiceState.frequency3;
			detune        = voiceState.detune3;
			filtFrequency = voiceState.filterFreq3;
			attackAmount  = voiceState.attack3;
			decayAmount   = voiceState.decay3;
			sustainAmount = voiceState.sustain3;
			releaseAmount = voiceState.release3;
			attackExpo    = voiceState.attackExpo3;
			decayExpo     = voiceState.decayExpo3;
			releaseExpo   = voiceState.releaseExpo3;
			op1ModAmount  = voiceState.op1ModAmount3;
			op2ModAmount  = voiceState.op2ModAmount3;
			op3ModAmount  = voiceState.op3ModAmount3;
			op4ModAmount  = voiceState.op4ModAmount3;
			egDestAmpl    = voiceState.egAmplitudeMod3;
			egDestFreq    = voiceState.egFrequencyMod3;
			egDestFilt    = voiceState.egFilterMod3;
			usingRatio    = voiceState.useRatio3;
			amplitudeVel  = voiceState.ampVelSens3;
			filterVel     = voiceState.filtVelSens3;
			wave          = voiceState.wave3;
			filterRes     = voiceState.filterRes3;

			break;
		case 4:
			amplitude     = voiceState.amplitude4;
			frequency     = voiceState.frequency4;
			detune        = voiceState.detune4;
			filtFrequency = voiceState.filterFreq4;
			attackAmount  = voiceState.attack4;
			decayAmount   = voiceState.decay4;
			sustainAmount = voiceState.sustain4;
			releaseAmount = voiceState.release4;
			attackExpo    = voiceState.attackExpo4;
			decayExpo     = voiceState.decayExpo4;
			releaseExpo   = voiceState.releaseExpo4;
			op1ModAmount  = voiceState.op1ModAmount4;
			op2ModAmount  = voiceState.op2ModAmount4;
			op3ModAmount  = voiceState.op3ModAmount4;
			op4ModAmount  = voiceState.op4ModAmount4;
			egDestAmpl    = voiceState.egAmplitudeMod4;
			egDestFreq    = voiceState.egFrequencyMod4;
			egDestFilt    = voiceState.egFilterMod4;
			usingRatio    = voiceState.useRatio4;
			amplitudeVel  = voiceState.ampVelSens4;
			filterVel     = voiceState.filtVelSens4;
			wave          = voiceState.wave4;
			filterRes     = voiceState.filterRes4;

			break;
		default:
			break;
	}

	if ( wave == OscillatorMode::SINE )
	{
		m_WaveNumCurrentlyBeingEdited = 1;
	}
	else if ( wave == OscillatorMode::TRIANGLE )
	{
		m_WaveNumCurrentlyBeingEdited = 2;
	}
	else if ( wave == OscillatorMode::SQUARE )
	{
		m_WaveNumCurrentlyBeingEdited = 3;
	}
	else if ( wave == OscillatorMode::SAWTOOTH )
	{
		m_WaveNumCurrentlyBeingEdited = 4;
	}

	m_EGDestBitmask = 0b000;

	if ( egDestAmpl ) m_EGDestBitmask = m_EGDestBitmask | 0b100;
	if ( egDestFreq ) m_EGDestBitmask = m_EGDestBitmask | 0b010;
	if ( egDestFilt ) m_EGDestBitmask = m_EGDestBitmask | 0b001;

	m_UsingRatio = usingRatio;
	m_UsingGlideRetrigger = usingGlide;
	m_UsingMono = usingMono;

	this->updateMonoPolyStr();
	this->updateWaveStr();
	this->updateRatioFixedStr();

	this->updateAmplitudeStr( amplitude, buffer, bufferLen );
	this->updateFrequencyStr( frequency, buffer, bufferLen );
	this->updateDetuneStr( detune, buffer, bufferLen );
	this->updateFiltFreqStr( filtFrequency, buffer, bufferLen );

	this->updateOpModStr( 1, op1ModAmount, buffer, bufferLen );
	this->updateOpModStr( 2, op2ModAmount, buffer, bufferLen );
	this->updateOpModStr( 3, op3ModAmount, buffer, bufferLen );
	this->updateOpModStr( 4, op4ModAmount, buffer, bufferLen );

	this->updateAttackStr( attackAmount, buffer, bufferLen );
	this->updateDecayStr( decayAmount, buffer, bufferLen );
	this->updateSustainStr( sustainAmount, buffer, bufferLen );
	this->updateReleaseStr( releaseAmount, buffer, bufferLen );

	this->updateAttackExpoStr( attackExpo, buffer, bufferLen );
	this->updateDecayExpoStr( decayExpo, buffer, bufferLen );
	this->updateReleaseExpoStr( releaseExpo, buffer, bufferLen );

	this->updateAmplitudeVelStr( amplitudeVel, buffer, bufferLen );
	this->updateFilterVelStr( filterVel, buffer, bufferLen );

	this->updateGlideStr( glideTime, buffer, bufferLen );
	this->updatePitchBendStr( pitchBendSemitones, buffer, bufferLen );

	this->updateFiltResStr( filterRes, buffer, bufferLen );

	m_CurrentMenu = ARMOR8_MENUS::STATUS;
	this->draw();
}

void ARMor8UiManager::onARMor8ParameterEvent (const ARMor8ParameterEvent& paramEvent)
{
	POT_CHANNEL channel = static_cast<POT_CHANNEL>( paramEvent.getChannel() );

	unsigned int bufferLen = 20;
	char buffer[bufferLen];

	switch ( channel )
	{
		case POT_CHANNEL::AMPLITUDE:
		{
			float amplitudeAmount = paramEvent.getValue();
			this->updateAmplitudeStr( amplitudeAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.65f, 0.62f, 0.73f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.65f, m_OpAmpStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.65f, 0.62f, 0.73f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::FILT_FREQ:
		{
			float filtFrequencyAmount = paramEvent.getValue();
			this->updateFiltFreqStr( filtFrequencyAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.83f, 0.62f, 0.92f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.83f, m_FiltFreqStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.83f, 0.62f, 0.92f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::FREQUENCY:
		{
			float frequencyAmount = paramEvent.getValue();
			this->updateFrequencyStr( frequencyAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.74f, 0.62f, 0.82f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.74f, m_FreqStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.74f, 0.62f, 0.82f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::ATTACK:
		{
			float attackAmount = paramEvent.getValue();
			this->updateAttackStr( attackAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.16f, 0.5f, 0.26f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.16f, m_AttackStr,  1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.16f, 0.5f, 0.26f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::DECAY:
		{
			float decayAmount = paramEvent.getValue();
			this->updateDecayStr( decayAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.28f, 0.5f, 0.38f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.28f, m_DecayStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.28f, 0.5f, 0.38f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}
			break;
		case POT_CHANNEL::SUSTAIN:
		{
			float sustainAmount = paramEvent.getValue();
			this->updateSustainStr( sustainAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.39f, 0.5f, 0.49f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.39f, m_SustainStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.39f, 0.5f, 0.49f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::RELEASE:
		{
			float releaseAmount = paramEvent.getValue();
			this->updateReleaseStr( releaseAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.5f, 0.5f, 0.6f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.50f, m_ReleaseStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.5f, 0.5f, 0.6f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::OP1_MOD_AMT:
		{
			float op1ModAmount = paramEvent.getValue();
			this->updateOpModStr( 1, op1ModAmount, buffer, bufferLen, false );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.52f, 0.16f, 1.1f, 0.26f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.52f, 0.16f, m_Op1Str, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.52f, 0.16f, 1.0f, 0.28f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::OP2_MOD_AMT:
		{
			float op2ModAmount = paramEvent.getValue();
			this->updateOpModStr( 2, op2ModAmount, buffer, bufferLen, false );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.52f, 0.28f, 1.1f, 0.38f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.52f, 0.28f, m_Op2Str, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.52f, 0.28f, 1.0f, 0.38f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::OP3_MOD_AMT:
		{
			float op3ModAmount = paramEvent.getValue();
			this->updateOpModStr( 3, op3ModAmount, buffer, bufferLen, false );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.52f, 0.39f, 1.1f, 0.49f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.52f, 0.39f, m_Op3Str, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.52f, 0.39f, 1.0f, 0.49f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::OP4_MOD_AMT:
		{
			float op4ModAmount = paramEvent.getValue();
			this->updateOpModStr( 4, op4ModAmount, buffer, bufferLen, false );

			if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.52f, 0.50f, 1.1f, 0.60f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.52f, 0.50f, m_Op4Str, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.52f, 0.50f, 1.0f, 0.60f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::STATUS;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::DETUNE:
		{
			float detuneAmountF = paramEvent.getValue();
			int detuneAmount = *reinterpret_cast<int*>( &detuneAmountF );
			this->updateDetuneStr( detuneAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.0f, 1.0f, 0.08f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.0f, m_DetuneStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.0f, 1.0f, 0.08f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::ATTACK_EXPO:
		{
			float attackExpoAmount = paramEvent.getValue();
			this->updateAttackExpoStr( attackExpoAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.16f, 0.6f, 0.26f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.16f, m_AttackExpoStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.16f, 0.6f, 0.26f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::DECAY_EXPO:
		{
			float decayExpoAmount = paramEvent.getValue();
			this->updateDecayExpoStr( decayExpoAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.3f, 0.6f, 0.4 );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.30f, m_DecayExpoStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.3f, 0.6f, 0.4f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::RELEASE_EXPO:
		{
			float releaseExpoAmount = paramEvent.getValue();
			this->updateReleaseExpoStr( releaseExpoAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.42f, 0.6f, 0.52f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.42f, m_ReleaseExpoStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.42f, 0.6f, 0.52f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::VEL_AMP:
		{
			float amplitudeVelAmount = paramEvent.getValue();
			this->updateAmplitudeVelStr( amplitudeVelAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.6f, 0.6f, 0.7f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.60f, m_AmplitudeVelStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.6f, 0.6f, 0.7f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::VEL_FILT:
		{
			float filterVelAmount = paramEvent.getValue();
			this->updateFilterVelStr( filterVelAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.0f, 0.72f, 0.6f, 0.81f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( -0.02f, 0.72f, m_FiltVelStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.0f, 0.72f, 0.6f, 0.81f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::GLIDE_TIME:
		{
			float glideTimeAmount = paramEvent.getValue();
			this->updateGlideStr( glideTimeAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.03f, 0.83f, 0.97f, 0.92f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.03f, 0.83f, m_GlideStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.03f, 0.83f, 0.97f, 0.92f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::PITCH_BEND:
		{
			float pitchBendAmount = paramEvent.getValue();
			unsigned int pitchBendAmountUInt = *reinterpret_cast<unsigned int*>( &pitchBendAmount );
			int pitchBendAmountInt = static_cast<int>( pitchBendAmountUInt );
			this->updatePitchBendStr( pitchBendAmountInt, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.03f, 0.93f, 0.97f, 1.0f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.03f, 0.93f, m_PitchBendStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.03f, 0.93f, 0.97f, 1.0f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		case POT_CHANNEL::FILT_RES:
		{
			float filtResAmount = paramEvent.getValue();
			this->updateFiltResStr( filtResAmount, buffer, bufferLen );

			if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
			{
				m_Graphics->setColor( false );
				m_Graphics->drawBoxFilled( 0.71f, 0.3f, 0.93f, 0.4f );
				m_Graphics->setColor( true );
				m_Graphics->drawText( 0.71f, 0.3f, m_FiltResStr, 1.0f );

				this->publishPartialLCDRefreshEvent( 0.71f, 0.3f, 0.93f, 0.4f );
			}
			else
			{
				m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
				this->draw();
			}
		}

			break;
		default:
			break;
	}
}

void ARMor8UiManager::setEGDestAmplitude (bool on)
{
	if ( on )
	{
		m_EGDestBitmask = m_EGDestBitmask | 0b100;
	}
	else
	{
		m_EGDestBitmask = m_EGDestBitmask & ~(0b100);
	}

	this->updateEGDestState();
	this->refreshEGDest();
}

void ARMor8UiManager::setEGDestFrequency (bool on)
{
	if ( on )
	{
		m_EGDestBitmask = m_EGDestBitmask | 0b010;
	}
	else
	{
		m_EGDestBitmask = m_EGDestBitmask & ~(0b010);
	}

	this->updateEGDestState();
	this->refreshEGDest();
}

void ARMor8UiManager::setEGDestFiltrFreq (bool on)
{
	if ( on )
	{
		m_EGDestBitmask = m_EGDestBitmask | 0b001;
	}
	else
	{
		m_EGDestBitmask = m_EGDestBitmask & ~(0b001);
	}

	this->updateEGDestState();
	this->refreshEGDest();
}

void ARMor8UiManager::processFreqOrDetunePot (float percentage)
{
	if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_DetunePotLocked, m_DetunePotCached, percentage) )
		{
			// detune
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::DETUNE)) );
		}
		else
		{
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_FreqPotLocked, m_FreqPotCached, percentage) )
		{
			// frequency
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::FREQUENCY)) );
		}
	}
}

void ARMor8UiManager::processAtkOrAtkExpoOrOp1ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_Op1ModPotLocked, m_Op1ModPotCached, percentage) )
		{
			// op 1 mod
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP1_MOD_AMT)) );
		}
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_AttackExpoPotLocked, m_AttackExpoPotCached, percentage) )
		{
			// attack expo
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::ATTACK_EXPO)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_AttackPotLocked, m_AttackPotCached, percentage) )
		{
			// attack
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::ATTACK)) );
		}
	}
}

void ARMor8UiManager::processDecOrDecExpoOrOp2ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_Op2ModPotLocked, m_Op2ModPotCached, percentage) )
		{
			// op 2 mod
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP2_MOD_AMT)) );
		}
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_DecayExpoPotLocked, m_DecayExpoPotCached, percentage) )
		{
			// decay expo
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::DECAY_EXPO)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_DecayPotLocked, m_DecayPotCached, percentage) )
		{
			// decay
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::DECAY)) );
		}
	}
}

void ARMor8UiManager::processSusOrOp3ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_Op3ModPotLocked, m_Op3ModPotCached, percentage) )
		{
			// op 3 mod
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP3_MOD_AMT)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_SustainPotLocked, m_SustainPotCached, percentage) )
		{
			// sustain
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::SUSTAIN)) );
		}
	}
}

void ARMor8UiManager::processRelOrRelExpoOrOp4ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_Op4ModPotLocked, m_Op4ModPotCached, percentage) )
		{
			// op 4 mod
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP4_MOD_AMT)) );
		}
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_ReleaseExpoPotLocked, m_ReleaseExpoPotCached, percentage) )
		{
			// release expo
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::RELEASE_EXPO)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_ReleasePotLocked, m_ReleasePotCached, percentage) )
		{
			// release
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::RELEASE)) );
		}
	}
}

void ARMor8UiManager::processAmpOrAmpVelPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_AmplitudeVelPotLocked, m_AmplitudeVelPotCached, percentage) )
		{
			// amplitude velocity
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::VEL_AMP)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_AmplitudePotLocked, m_AmplitudePotCached, percentage) )
		{
			// amplitude
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::AMPLITUDE)) );
		}
	}
}

void ARMor8UiManager::processFiltFreqOrFiltResOrFiltVelPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_FiltVelPotLocked, m_FiltVelPotCached, percentage) )
		{
			// filter velocity
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::VEL_FILT)) );
		}
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_FiltResPotLocked, m_FiltResPotCached, percentage) )
		{
			// filter resonance
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::FILT_RES)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_FiltFreqPotLocked, m_FiltFreqPotCached, percentage) )
		{
			// filter frequency
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::FILT_FREQ)) );
		}
	}
}

void ARMor8UiManager::processPitchBendOrGlidePot (float percentage)
{
	if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		if ( this->hasBrokenLock(m_PitchBendPotLocked, m_PitchBendPotCached, percentage) )
		{
			// pitch bend
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::PITCH_BEND)) );
		}
	}
	else
	{
		if ( this->hasBrokenLock(m_GlidePotLocked, m_GlidePotCached, percentage) )
		{
			// glide time
			IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::GLIDE_TIME)) );
		}
	}
}

void ARMor8UiManager::processAlt1Btn (bool pressed)
{
	this->updateButtonState( m_Alt1State, pressed );
}

void ARMor8UiManager::processAlt2Btn (bool pressed)
{
	this->updateButtonState( m_Alt2State, pressed );
}

void ARMor8UiManager::processRatioOrFixedBtn (bool pressed)
{
	this->updateButtonState( m_RatioOrFixedBtnState, pressed );

	if ( m_RatioOrFixedBtnState == BUTTON_STATE::RELEASED )
	{
		m_UsingRatio = !m_UsingRatio;

		if ( m_UsingRatio )
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::RATIO)) );
		}
		else
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::RATIO)) );
		}

		this->updateRatioFixedStr();
		this->refreshRatioFixed();
	}
}

void ARMor8UiManager::processNextOpBtn (bool pressed)
{
	this->updateButtonState( m_NextOpBtnState, pressed );

	if ( m_NextOpBtnState == BUTTON_STATE::RELEASED )
	{
		if ( m_OpCurrentlyBeingEdited == 1 )
		{
			m_OpCurrentlyBeingEdited = 2;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP2)) );
		}
		else if ( m_OpCurrentlyBeingEdited == 2 )
		{
			m_OpCurrentlyBeingEdited = 3;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP3)) );
		}
		else if ( m_OpCurrentlyBeingEdited == 3 )
		{
			m_OpCurrentlyBeingEdited = 4;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP4)) );
		}
		else if ( m_OpCurrentlyBeingEdited == 4 )
		{
			m_OpCurrentlyBeingEdited = 1;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP1)) );
		}
	}
}

void ARMor8UiManager::processNextWaveBtn (bool pressed)
{
	this->updateButtonState( m_NextWaveBtnState, pressed );

	if ( m_NextWaveBtnState == BUTTON_STATE::RELEASED )
	{
		if ( m_WaveNumCurrentlyBeingEdited == 1 )
		{
			m_WaveNumCurrentlyBeingEdited = 2;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::TRIANGLE)) );
		}
		else if ( m_WaveNumCurrentlyBeingEdited == 2 )
		{
			m_WaveNumCurrentlyBeingEdited = 3;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::SQUARE)) );
		}
		else if ( m_WaveNumCurrentlyBeingEdited == 3 )
		{
			m_WaveNumCurrentlyBeingEdited = 4;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::SAWTOOTH)) );
		}
		else if ( m_WaveNumCurrentlyBeingEdited == 4 )
		{
			m_WaveNumCurrentlyBeingEdited = 1;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::SINE)) );
		}

		this->updateWaveStr();
		this->refreshWave();
	}
}

void ARMor8UiManager::processGlideRetrigBtn (bool pressed)
{
	this->updateButtonState( m_GlideRetrigBtnState, pressed );

	if ( m_GlideRetrigBtnState == BUTTON_STATE::RELEASED )
	{
		m_UsingGlideRetrigger = !m_UsingGlideRetrigger;

		if ( m_UsingGlideRetrigger )
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
								static_cast<unsigned int>(BUTTON_CHANNEL::GLIDE_RETRIG)) );
		}
		else
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
								static_cast<unsigned int>(BUTTON_CHANNEL::GLIDE_RETRIG)) );
		}

		this->refreshGlideRetrig();
	}
}

void ARMor8UiManager::processMonoBtn (bool pressed)
{
	this->updateButtonState( m_MonoBtnState, pressed );

	if ( m_MonoBtnState == BUTTON_STATE::RELEASED )
	{
		m_UsingMono = !m_UsingMono;

		if ( m_UsingMono )
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
								static_cast<unsigned int>(BUTTON_CHANNEL::MONOPHONIC)) );
		}
		else
		{
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
								static_cast<unsigned int>(BUTTON_CHANNEL::MONOPHONIC)) );
		}

		this->updateMonoPolyStr();
		this->refreshMonoPoly();
	}
}

void ARMor8UiManager::processEGDestBtn (bool pressed)
{
	this->updateButtonState( m_EGDestBtnState, pressed );

	if ( m_EGDestBtnState == BUTTON_STATE::RELEASED )
	{
		m_EGDestBitmask++;

		if ( m_EGDestBitmask > 0b111 ) m_EGDestBitmask = 0b000;

		this->updateEGDestState();
		this->refreshEGDest();
	}
}

void ARMor8UiManager::processPrevPresetBtn (bool pressed)
{
	this->updateButtonState( m_PrevPresetBtnState, pressed );

	if ( m_PrevPresetBtnState == BUTTON_STATE::RELEASED )
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::PREV_PRESET)) );
	}
}

void ARMor8UiManager::processNextPresetBtn (bool pressed)
{
	this->updateButtonState( m_NextPresetBtnState, pressed );

	if ( m_NextPresetBtnState == BUTTON_STATE::RELEASED )
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::NEXT_PRESET)) );
	}
}

void ARMor8UiManager::processWritePresetBtn (bool pressed)
{
	this->updateButtonState( m_WritePresetBtnState, pressed );

	if ( m_WritePresetBtnState == BUTTON_STATE::RELEASED )
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::WRITE_PRESET)) );
	}
}

void ARMor8UiManager::updateButtonState (BUTTON_STATE& buttonState, bool pressed)
{
	if ( pressed )
	{
		if ( buttonState == BUTTON_STATE::FLOATING || buttonState == BUTTON_STATE::RELEASED )
		{
			buttonState = BUTTON_STATE::PRESSED;
		}
		else
		{
			buttonState = BUTTON_STATE::HELD;
		}
	}
	else
	{
		if ( buttonState == BUTTON_STATE::PRESSED || buttonState == BUTTON_STATE::HELD )
		{
			buttonState = BUTTON_STATE::RELEASED;
		}
		else
		{
			buttonState = BUTTON_STATE::FLOATING;
		}
	}
}

void ARMor8UiManager::updateEGDestState()
{
	bool amplitudeActive = (m_EGDestBitmask >> 2);
	bool frequencyActive = (m_EGDestBitmask >> 1) & 0b00000001;
	bool filtrFreqActive = (m_EGDestBitmask >> 0) & 0b00000001;

	if ( amplitudeActive )
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_AMP)) );
	}
	else
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_AMP)) );
	}

	if ( frequencyActive )
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FREQ)) );
	}
	else
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FREQ)) );
	}

	if ( filtrFreqActive )
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FILT)) );
	}
	else
	{
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FILT)) );
	}
}

void ARMor8UiManager::publishPartialLCDRefreshEvent (float xStart, float yStart, float xEnd, float yEnd)
{
	unsigned int xStartUInt = m_Graphics->convertXPercentageToUInt( xStart );
	unsigned int yStartUInt = m_Graphics->convertYPercentageToUInt( yStart );
	unsigned int xEndUInt   = m_Graphics->convertXPercentageToUInt( xEnd   );
	unsigned int yEndUInt   = m_Graphics->convertYPercentageToUInt( yEnd   );

	IARMor8LCDRefreshEventListener::PublishEvent( ARMor8LCDRefreshEvent(xStartUInt, yStartUInt, xEndUInt, yEndUInt, 0) );
}

void ARMor8UiManager::lockAllPots()
{
	m_FreqPotLocked = true;
	m_DetunePotLocked = true;
	m_AttackPotLocked = true;
	m_AttackExpoPotLocked = true;
	m_Op1ModPotLocked = true;
	m_DecayPotLocked = true;
	m_DecayExpoPotLocked = true;
	m_Op2ModPotLocked = true;
	m_SustainPotLocked = true;
	m_Op3ModPotLocked = true;
	m_ReleasePotLocked = true;
	m_ReleaseExpoPotLocked = true;
	m_Op4ModPotLocked = true;
	m_AmplitudePotLocked = true;
	m_AmplitudeVelPotLocked = true;
	m_FiltFreqPotLocked = true;
	m_FiltResPotLocked = true;
	m_FiltVelPotLocked = true;
	m_PitchBendPotLocked = true;
	m_GlidePotLocked = true;
}

bool ARMor8UiManager::hasBrokenLock (bool& potLockedVal, float& potCachedVal, float newPotVal)
{
	bool hasBrokenLowerRange  = newPotVal <= ( potCachedVal - m_PotChangeThreshold );
	bool hasBrokenHigherRange = newPotVal >= ( potCachedVal + m_PotChangeThreshold );

	if ( (! potLockedVal) && (potCachedVal != newPotVal) ) // if not locked and not the exact same value
	{
		potCachedVal = newPotVal;
		return true;
	}
	else if ( potLockedVal && (hasBrokenHigherRange || hasBrokenLowerRange) ) // if locked but broke threshold
	{
		potLockedVal = false;
		potCachedVal = newPotVal;
		return true;
	}

	return false;
}

void ARMor8UiManager::updateOpNumberStr (char* buffer, unsigned int bufferLen)
{
	this->intToCString( m_OpCurrentlyBeingEdited, buffer, bufferLen );
	this->concatDigitStr( m_OpCurrentlyBeingEdited, buffer, m_PrstAndOpStr, 15, 1 );
}

void ARMor8UiManager::updatePrstNumberStr (char* buffer, unsigned int bufferLen)
{
	this->intToCString( m_CurrentPresetNum, buffer, bufferLen );
	this->concatDigitStr( m_CurrentPresetNum, buffer, m_PrstAndOpStr, 7, 2 );
}

void ARMor8UiManager::updateMonoPolyStr()
{
	if ( m_UsingMono )
	{
		m_MonoPolyStr[1] = 'M';
		m_MonoPolyStr[2] = '0';
		m_MonoPolyStr[3] = 'N';
		m_MonoPolyStr[4] = '0';
	}
	else
	{
		m_MonoPolyStr[1] = 'P';
		m_MonoPolyStr[2] = '0';
		m_MonoPolyStr[3] = 'L';
		m_MonoPolyStr[4] = 'Y';
	}
}

void ARMor8UiManager::updateWaveStr()
{
	if ( m_WaveNumCurrentlyBeingEdited == 1 ) // sine
	{
		m_WaveStr[0] = 'S';
		m_WaveStr[1] = 'I';
		m_WaveStr[2] = 'N';
		m_WaveStr[3] = 'E';
	}
	else if ( m_WaveNumCurrentlyBeingEdited == 2 ) // triangle
	{
		m_WaveStr[0] = 'T';
		m_WaveStr[1] = 'R';
		m_WaveStr[2] = 'I';
		m_WaveStr[3] = ' ';
	}
	else if ( m_WaveNumCurrentlyBeingEdited == 3 ) // square
	{
		m_WaveStr[0] = 'S';
		m_WaveStr[1] = 'Q';
		m_WaveStr[2] = 'R';
		m_WaveStr[3] = ' ';
	}
	else if ( m_WaveNumCurrentlyBeingEdited == 4 ) // sawtooth
	{
		m_WaveStr[0] = 'S';
		m_WaveStr[1] = 'A';
		m_WaveStr[2] = 'W';
		m_WaveStr[3] = ' ';
	}
}

void ARMor8UiManager::updateRatioFixedStr()
{
	if ( m_UsingRatio )
	{
		m_RatioFixedStr[1] = 'R';
		m_RatioFixedStr[2] = 'A';
		m_RatioFixedStr[3] = 'T';
		m_RatioFixedStr[4] = 'I';
		m_RatioFixedStr[5] = '0';
	}
	else
	{
		m_RatioFixedStr[1] = 'F';
		m_RatioFixedStr[2] = 'I';
		m_RatioFixedStr[3] = 'X';
		m_RatioFixedStr[4] = 'E';
		m_RatioFixedStr[5] = 'D';
	}
}

void ARMor8UiManager::updateAmplitudeStr (float amplitude, char* buffer, unsigned int bufferLen)
{
	int amplitudeInt = static_cast<int>( amplitude * 1000.0f );
	this->intToCString( amplitudeInt, buffer, bufferLen );
	this->concatDigitStr( amplitudeInt, buffer, m_OpAmpStr, 6, 5, 2 );
}

void ARMor8UiManager::updateFrequencyStr (float frequency, char* buffer, unsigned int bufferLen)
{
	if ( m_UsingRatio )
	{
		if ( frequency > OP_THRESHOLD_6X )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '6';
			m_FreqStr[10] = 'X';
		}
		else if ( frequency > OP_THRESHOLD_5X )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '5';
			m_FreqStr[10] = 'X';
		}
		else if ( frequency > OP_THRESHOLD_4X )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '4';
			m_FreqStr[10] = 'X';
		}
		else if ( frequency > OP_THRESHOLD_3X )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '3';
			m_FreqStr[10] = 'X';
		}
		else if ( frequency > OP_THRESHOLD_2X )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '2';
			m_FreqStr[10] = 'X';
		}
		else if ( frequency > OP_THRESHOLD_1X )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '1';
			m_FreqStr[10] = 'X';
		}
		else if ( frequency > OP_THRESHOLD_D2 )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '/';
			m_FreqStr[10] = '2';
		}
		else if ( frequency >= OP_THRESHOLD_D4 )
		{
			m_FreqStr[6] =  ' ';
			m_FreqStr[7] =  ' ';
			m_FreqStr[8] =  ' ';
			m_FreqStr[9] =  '/';
			m_FreqStr[10] = '4';
		}
	}
	else
	{
		int frequencyInt = static_cast<int>( frequency );
		this->intToCString( frequencyInt, buffer, bufferLen );
		this->concatDigitStr( frequencyInt, buffer, m_FreqStr, 6, 5 );
	}
}

void ARMor8UiManager::updateFiltFreqStr (float filtFrequency, char* buffer, unsigned int bufferLen)
{
	int filtFreqInt = static_cast<int>( filtFrequency );
	this->intToCString( filtFreqInt, buffer, bufferLen );
	this->concatDigitStr( filtFreqInt, buffer, m_FiltFreqStr, 6, 5 );
}

void ARMor8UiManager::updateOpModStr (unsigned int opNum, float opModAmount, char* buffer, unsigned int bufferLen, bool div)
{
	if ( div )
	{
		opModAmount = ( opModAmount / ARMOR8_OP_MOD_MAX ) ;
	}

	if ( opNum == 1 )
	{
		int op1ModAmountInt = static_cast<int>( opModAmount * 1000.0f );
		this->intToCString( op1ModAmountInt, buffer, bufferLen );
		this->concatDigitStr( op1ModAmountInt, buffer, m_Op1Str, 4, 5, 2 );
	}
	else if ( opNum == 2 )
	{
		int op2ModAmountInt = static_cast<int>( opModAmount * 1000.0f );
		this->intToCString( op2ModAmountInt, buffer, bufferLen );
		this->concatDigitStr( op2ModAmountInt, buffer, m_Op2Str, 4, 5, 2 );
	}
	else if ( opNum == 3 )
	{
		int op3ModAmountInt = static_cast<int>( opModAmount * 1000.0f );
		this->intToCString( op3ModAmountInt, buffer, bufferLen );
		this->concatDigitStr( op3ModAmountInt, buffer, m_Op3Str, 4, 5, 2 );
	}
	else if ( opNum == 4 )
	{
		int op4ModAmountInt = static_cast<int>( opModAmount * 1000.0f );
		this->intToCString( op4ModAmountInt, buffer, bufferLen );
		this->concatDigitStr( op4ModAmountInt, buffer, m_Op4Str, 4, 5, 2 );
	}
}

void ARMor8UiManager::updateAttackStr (float attackAmount, char* buffer, unsigned int bufferLen)
{
	int attackAmountInt = static_cast<int>( attackAmount * 1000.0f );
	this->intToCString( attackAmountInt, buffer, bufferLen );
	this->concatDigitStr( attackAmountInt, buffer, m_AttackStr, 4, 5, 2 );
}

void ARMor8UiManager::updateDecayStr (float decayAmount, char* buffer, unsigned int bufferLen)
{
	int decayAmountInt = static_cast<int>( decayAmount * 1000.0f );
	this->intToCString( decayAmountInt, buffer, bufferLen );
	this->concatDigitStr( decayAmountInt, buffer, m_DecayStr, 4, 5, 2 );
}

void ARMor8UiManager::updateSustainStr (float sustainAmount, char* buffer, unsigned int bufferLen)
{
	int sustainAmountInt = static_cast<int>( sustainAmount * 1000.0f );
	this->intToCString( sustainAmountInt, buffer, bufferLen );
	this->concatDigitStr( sustainAmountInt, buffer, m_SustainStr, 4, 5, 2 );
}

void ARMor8UiManager::updateReleaseStr (float releaseAmount, char* buffer, unsigned int bufferLen)
{
	int releaseAmountInt = static_cast<int>( releaseAmount * 1000.0f );
	this->intToCString( releaseAmountInt, buffer, bufferLen );
	this->concatDigitStr( releaseAmountInt, buffer, m_ReleaseStr, 4, 5, 2 );
}

void ARMor8UiManager::updateDetuneStr (int detuneAmount, char* buffer, unsigned int bufferLen)
{
	this->intToCString( detuneAmount, buffer, bufferLen );
	this->concatDigitStr( detuneAmount, buffer, m_DetuneStr, 7, 5 );
}

void ARMor8UiManager::updateAttackExpoStr (float attackExpoAmount, char* buffer, unsigned int bufferLen)
{
	attackExpoAmount = (attackExpoAmount - ARMOR8_EXPO_MIN) / (ARMOR8_EXPO_MAX - ARMOR8_EXPO_MIN);
	int attackExpoAmountInt = static_cast<int>( attackExpoAmount * 100.0f );
	this->intToCString( attackExpoAmountInt, buffer, bufferLen );
	this->concatDigitStr( attackExpoAmountInt, buffer, m_AttackExpoStr, 7, 4, 2 );
}

void ARMor8UiManager::updateDecayExpoStr (float decayExpoAmount, char* buffer, unsigned int bufferLen)
{
	decayExpoAmount = (decayExpoAmount - ARMOR8_EXPO_MIN) / (ARMOR8_EXPO_MAX - ARMOR8_EXPO_MIN);
	int decayExpoAmountInt = static_cast<int>( decayExpoAmount * 100.0f );
	this->intToCString( decayExpoAmountInt, buffer, bufferLen );
	this->concatDigitStr( decayExpoAmountInt, buffer, m_DecayExpoStr, 7, 4, 2 );
}

void ARMor8UiManager::updateReleaseExpoStr (float releaseExpoAmount, char* buffer, unsigned int bufferLen)
{
	releaseExpoAmount = (releaseExpoAmount - ARMOR8_EXPO_MIN) / (ARMOR8_EXPO_MAX - ARMOR8_EXPO_MIN);
	int releaseExpoAmountInt = static_cast<int>( releaseExpoAmount * 100.0f );
	this->intToCString( releaseExpoAmountInt, buffer, bufferLen );
	this->concatDigitStr( releaseExpoAmountInt, buffer, m_ReleaseExpoStr, 7, 4, 2 );
}

void ARMor8UiManager::updateAmplitudeVelStr (float amplitudeVelAmount, char* buffer, unsigned int bufferLen)
{
	int amplitudeVelAmountInt = static_cast<int>( amplitudeVelAmount * 100.0f );
	this->intToCString( amplitudeVelAmountInt, buffer, bufferLen );
	this->concatDigitStr( amplitudeVelAmountInt, buffer, m_AmplitudeVelStr, 7, 4, 2 );
}

void ARMor8UiManager::updateFilterVelStr (float filterVelAmount, char* buffer, unsigned int bufferLen)
{
	int filterVelAmountInt = static_cast<int>( filterVelAmount * 100.0f );
	this->intToCString( filterVelAmountInt, buffer, bufferLen );
	this->concatDigitStr( filterVelAmountInt, buffer, m_FiltVelStr, 7, 4, 2 );
}

void ARMor8UiManager::updateGlideStr (float glideAmount, char* buffer, unsigned int bufferLen)
{
	int glideAmountInt = static_cast<int>( glideAmount * 1000.0f );
	this->intToCString( glideAmountInt, buffer, bufferLen );
	this->concatDigitStr( glideAmountInt, buffer, m_GlideStr, 12, 5, 2 );
}

void ARMor8UiManager::updatePitchBendStr (unsigned int pitchBendAmount, char* buffer, unsigned int bufferLen)
{
	int pitchBendAmountInt = static_cast<int>( pitchBendAmount );
	this->intToCString( pitchBendAmountInt, buffer, bufferLen );
	this->concatDigitStr( pitchBendAmountInt, buffer, m_PitchBendStr, 15, 2 );
}

void ARMor8UiManager::updateFiltResStr (float filtResAmount, char* buffer, unsigned int bufferLen)
{
	filtResAmount = filtResAmount / ARMOR8_FILT_RES_MAX;
	int filtResAmountInt = static_cast<int>( filtResAmount * 100.0f );
	this->intToCString( filtResAmountInt, buffer, bufferLen );
	this->concatDigitStr( filtResAmountInt, buffer, m_FiltResStr, 0, 4, 2 );
}

void ARMor8UiManager::refreshEGDest()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.92f, 0.64f, 1.0f, 0.9f );
		m_Graphics->setColor( true );

		// eg destination amplitude
		if ( m_EGDestBitmask & 0b100 )
		{
			m_Graphics->drawCircle( 0.96f, 0.67f, 0.02f );
			m_Graphics->drawCircleFilled( 0.96f, 0.67f, 0.02f );
		}
		else
		{
			m_Graphics->drawCircle( 0.96f, 0.67f, 0.02f );
		}

		// eg destination frequency
		if ( m_EGDestBitmask & 0b010 )
		{
			m_Graphics->drawCircle( 0.96f, 0.77f, 0.02f );
			m_Graphics->drawCircleFilled( 0.96f, 0.77f, 0.02f );
		}
		else
		{
			m_Graphics->drawCircle( 0.96f, 0.77f, 0.02f );
		}

		// eg destination filter
		if ( m_EGDestBitmask & 0b001 )
		{
			m_Graphics->drawCircle( 0.96f, 0.87f, 0.02f );
			m_Graphics->drawCircleFilled( 0.96f, 0.87f, 0.02f );
		}
		else
		{
			m_Graphics->drawCircle( 0.96f, 0.87f, 0.02f );
		}

		this->publishPartialLCDRefreshEvent( 0.92f, 0.64f, 1.0f, 0.9f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS;
		this->draw();
	}
}

void ARMor8UiManager::refreshRatioFixed()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.625f, 0.93f, 1.0f, 1.0f );
		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.625f, 0.93f, m_RatioFixedStr, 1.0f );

		this->publishPartialLCDRefreshEvent( 0.625f, 0.93f, 1.0f, 1.0f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS;
		this->draw();
	}
}

void ARMor8UiManager::refreshMonoPoly()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.0f, 0.93f, 0.4f, 1.0f );
		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.0f, 0.93f, m_MonoPolyStr, 1.0f );

		this->publishPartialLCDRefreshEvent( 0.0f, 0.93f, 0.4f, 1.0f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS;
		this->draw();
	}
}

void ARMor8UiManager::refreshWave()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.37f, 0.94f, 0.63f, 1.0f );
		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.37f, 0.94f, m_WaveStr, 1.0f );

		this->publishPartialLCDRefreshEvent( 0.37f, 0.94f, 0.63f, 1.0f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS;
		this->draw();
	}
}

void ARMor8UiManager::refreshGlideRetrig()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::ADDITIONAL )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.76f, 0.63f, 0.86f, 0.75f );

		m_Graphics->setColor( true );
		if ( m_UsingGlideRetrigger )
		{
			m_Graphics->drawCircle( 0.81f, 0.67f, 0.03f );
			m_Graphics->drawCircleFilled( 0.81f, 0.67f, 0.03f );
		}
		else
		{
			m_Graphics->drawCircle( 0.81f, 0.67f, 0.03f );
		}

		this->publishPartialLCDRefreshEvent( 0.76f, 0.63f, 0.86f, 0.75f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::ADDITIONAL;
		this->draw();
	}
}

void ARMor8UiManager::intToCString (int val, char* buffer, unsigned int bufferLen)
{
	if ( bufferLen == 0 ) return;

	unsigned int bufferIndex = 0;

	bool isNegative = val < 0;

	unsigned int valUInt = isNegative ? -val : val;

	while ( valUInt != 0 )
	{
		if ( bufferIndex == bufferLen - 1 )
		{
			buffer[bufferIndex] = '\0';
			return;
		}

		buffer[bufferIndex] = ( valUInt % 10 ) + '0';
		valUInt = valUInt / 10;
		bufferIndex++;
	}

	if ( isNegative && bufferIndex != bufferLen - 1 )
	{
		buffer[bufferIndex] = '-';
		bufferIndex++;
	}

	buffer[bufferIndex] = '\0';

	for ( int swapIndex = 0; swapIndex < bufferIndex/2; swapIndex++ )
	{
		buffer[swapIndex] ^= buffer[bufferIndex - swapIndex - 1];
		buffer[bufferIndex - swapIndex - 1] ^= buffer[swapIndex];
		buffer[swapIndex] ^= buffer[ bufferIndex - swapIndex - 1];
	}

	if ( val == 0 )
	{
		buffer[0] = '0';
		buffer[1] = '\0';
	}
}

void ARMor8UiManager::concatDigitStr (int val, char* sourceBuffer, char* destBuffer, unsigned int offset, unsigned int digitWidth,
					int decimalPlaceIndex)
{
	int sourceNumDigits = 1;

	unsigned int valAbs = abs( val );

	if ( valAbs > 0 )
	{
		for (sourceNumDigits = 0; valAbs > 0; sourceNumDigits++)
		{
			valAbs = valAbs / 10;
		}
	}

	// if it's negative, we need an extra space
	if ( val < 0 ) sourceNumDigits += 1;

	bool usingDecimalPoint = false;

	// if it's got a decimal place, it also needs an extra space
	if ( decimalPlaceIndex > -1 )
	{
		usingDecimalPoint = true;
		sourceNumDigits += 1;
	}

	unsigned int numToSkip = abs( static_cast<int>(sourceNumDigits - digitWidth) );

	// this needs to be set after skipping the decimal place so that source buffer index is still correct
	unsigned int decimalPlaceOffset = 0;

	for ( unsigned int index = 0; index < digitWidth; index++ )
	{
		if ( index != decimalPlaceIndex - 1 )
		{
			if ( index < (numToSkip + decimalPlaceOffset) )
			{
				destBuffer[offset + index] = ' ';
			}
			else
			{
				destBuffer[offset + index] = sourceBuffer[index - numToSkip - decimalPlaceOffset];
			}
		}
		else
		{
			decimalPlaceOffset = 1;
		}
	}
}
