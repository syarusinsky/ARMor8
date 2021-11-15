#include "ARMor8UiManager.hpp"

#include "AudioConstants.hpp"
#include "Graphics.hpp"
#include "Sprite.hpp"
#include "IARMor8LCDRefreshEventListener.hpp"
#include "IARMor8ParameterEventListener.hpp"
#include "IPotEventListener.hpp"
#include "IButtonEventListener.hpp"

static inline float AUDIO_TAPER (float input) { return pow(input, 2); }

constexpr unsigned int SETTINGS_NUM_VISIBLE_ENTRIES = 6;

ARMor8UiManager::ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	Surface( width, height, format ),
	m_Logo( nullptr ),
	m_CurrentMenu( ARMOR8_MENUS::LOADING ),
	m_SettingsMainModel( SETTINGS_NUM_VISIBLE_ENTRIES ),
	m_AssignEffectPotModel( SETTINGS_NUM_VISIBLE_ENTRIES ),
	m_SelectOperatorModel( SETTINGS_NUM_VISIBLE_ENTRIES ),
	m_SelectWaveformModel( SETTINGS_NUM_VISIBLE_ENTRIES ),
	m_EffectPotToAssign( 0 ),
	m_Effect1PotAssignmentIndex( 0 ),
	m_Effect2PotAssignmentIndex( 1 ),
	m_Effect3PotAssignmentIndex( 2 ),
	m_Effect1PotAssignmentOp( 1 ),
	m_Effect2PotAssignmentOp( 1 ),
	m_Effect3PotAssignmentOp( 1 ),
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
	m_Effect1PotCached( 0.0f ),
	m_Effect2PotCached( 0.0f ),
	m_Effect3PotCached( 0.0f ),
	m_Effect1PotLocked( false ),
	m_Effect2PotLocked( false ),
	m_Effect3PotLocked( false ),
	m_Effect1BtnState( BUTTON_STATE::FLOATING ),
	m_Effect2BtnState( BUTTON_STATE::FLOATING ),
	m_Pot1StabilizerBuf{ 0.0f },
	m_Pot2StabilizerBuf{ 0.0f },
	m_Pot3StabilizerBuf{ 0.0f },
	m_Pot1StabilizerIndex( 0 ),
	m_Pot2StabilizerIndex( 0 ),
	m_Pot3StabilizerIndex( 0 ),
	m_Pot1StabilizerValue( 0.0f ),
	m_Pot2StabilizerValue( 0.0f ),
	m_Pot3StabilizerValue( 0.0f ),
	m_Pot1StabilizerCachedPer( 0.0f ),
	m_Pot2StabilizerCachedPer( 0.0f ),
	m_Pot3StabilizerCachedPer( 0.0f )
{
	// add entries to main settings menu and store indices for comparison
	m_SettingsMenuAssignEffect1Index = m_SettingsMainModel.addEntry( "Assign EFFECT1" );
	m_SettingsMenuAssignEffect2Index = m_SettingsMainModel.addEntry( "Assign EFFECT2" );
	m_SettingsMenuAssignEffect3Index = m_SettingsMainModel.addEntry( "Assign EFFECT3" );
	m_SettingsMenuSelectOperatorIndex = m_SettingsMainModel.addEntry( "Select operator" );
	m_SettingsMenuSelectWaveformIndex = m_SettingsMainModel.addEntry( "Select waveform" );
	m_SettingsMenuUseRatioFreqIndex = m_SettingsMainModel.addEntry( "Use ratio freq", true );
	m_SettingsMenuEGDestAmpIndex = m_SettingsMainModel.addEntry( "EG Dest: AMP", true );
	m_SettingsMenuEGDestFreqIndex = m_SettingsMainModel.addEntry( "EG Dest: FREQ", true );
	m_SettingsMenuEGDestFiltIndex = m_SettingsMainModel.addEntry( "EG Dest: FILT", true );
	m_SettingsMenuGlideRetrigIndex = m_SettingsMainModel.addEntry( "Glide retrig", true );
	m_SettingsMenuMonophonicIndex = m_SettingsMainModel.addEntry( "Monophonic", true );
	m_SettingsMenuWritePresetIndex = m_SettingsMainModel.addEntry( "Write preset" );
	m_SettingsMenuExitMenuIndex = m_SettingsMainModel.addEntry( "> Exit menu" );

	// add entries to assign effect pot menu and store indices for comparison
	m_AssignEffectPotMenuFreqIndex = m_AssignEffectPotModel.addEntry( "Frequency" );
	m_AssignEffectPotMenuDetuneIndex = m_AssignEffectPotModel.addEntry( "Detune" );
	m_AssignEffectPotMenuAttackIndex = m_AssignEffectPotModel.addEntry( "EG attack" );
	m_AssignEffectPotMenuDecayIndex = m_AssignEffectPotModel.addEntry( "EG decay" );
	m_AssignEffectPotMenuSustainIndex = m_AssignEffectPotModel.addEntry( "EG sustain" );
	m_AssignEffectPotMenuReleaseIndex = m_AssignEffectPotModel.addEntry( "EG release" );
	m_AssignEffectPotMenuAtkExpoIndex = m_AssignEffectPotModel.addEntry( "Attack expo" );
	m_AssignEffectPotMenuDecExpoIndex = m_AssignEffectPotModel.addEntry( "Decay expo" );
	m_AssignEffectPotMenuRelExpoIndex = m_AssignEffectPotModel.addEntry( "Release expo" );
	m_AssignEffectPotMenuOp1ModIndex = m_AssignEffectPotModel.addEntry( "Op1 mod amount" );
	m_AssignEffectPotMenuOp2ModIndex = m_AssignEffectPotModel.addEntry( "Op2 mod amount" );
	m_AssignEffectPotMenuOp3ModIndex = m_AssignEffectPotModel.addEntry( "Op3 mod amount" );
	m_AssignEffectPotMenuOp4ModIndex = m_AssignEffectPotModel.addEntry( "Op4 mod amount" );
	m_AssignEffectPotMenuAmplitudeIndex = m_AssignEffectPotModel.addEntry( "Amplitude" );
	m_AssignEffectPotMenuFiltFreqIndex = m_AssignEffectPotModel.addEntry( "Filt frequency" );
	m_AssignEffectPotMenuFiltResIndex = m_AssignEffectPotModel.addEntry( "Filt resonance" );
	m_AssignEffectPotMenuAmpVelSensIndex = m_AssignEffectPotModel.addEntry( "Amp velocity" );
	m_AssignEffectPotMenuFiltVelSensIndex = m_AssignEffectPotModel.addEntry( "Filt velocity" );
	m_AssignEffectPotMenuPBendSemiIndex = m_AssignEffectPotModel.addEntry( "Pitch bend" );
	m_AssignEffectPotMenuGlideTimeIndex = m_AssignEffectPotModel.addEntry( "Glide time" );

	// add entries to select operator menu and store indices for comparison
	m_SelectOperatorMenuOp1Index = m_SelectOperatorModel.addEntry( "Edit operator 1" );
	m_SelectOperatorMenuOp2Index = m_SelectOperatorModel.addEntry( "Edit operator 2" );
	m_SelectOperatorMenuOp3Index = m_SelectOperatorModel.addEntry( "Edit operator 3" );
	m_SelectOperatorMenuOp4Index = m_SelectOperatorModel.addEntry( "Edit operator 4" );
	m_SelectOperatorMenuExitMenuIndex = m_SelectOperatorModel.addEntry( "> Exit menu" );

	// add entries to select waveform menu and store indices for comparison
	m_SelectWaveformMenuSineIndex = m_SelectWaveformModel.addEntry( "Use sine" );
	m_SelectWaveformMenuTriIndex = m_SelectWaveformModel.addEntry( "Use tri" );
	m_SelectWaveformMenuSquareIndex = m_SelectWaveformModel.addEntry( "Use square" );
	m_SelectWaveformMenuSawIndex = m_SelectWaveformModel.addEntry( "Use saw" );
	m_SelectWaveformMenuExitMenuIndex = m_SelectWaveformModel.addEntry( "> Exit menu" );
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
	else if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
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
	else if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
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
	else if ( m_CurrentMenu == ARMOR8_MENUS::SETTINGS_MAIN )
	{
		this->drawScrollableMenu( m_SettingsMainModel, &ARMor8UiManager::shouldTickSettingsMenu, *this );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT )
	{
		this->drawScrollableMenu( m_AssignEffectPotModel, nullptr, *this );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_OPERATOR )
	{
		this->drawScrollableMenu( m_SelectOperatorModel, nullptr, *this );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_WAVEFORM )
	{
		this->drawScrollableMenu( m_SelectWaveformModel, nullptr, *this );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::WRITE_PRESET_CONFIRMATION )
	{
		m_Graphics->setColor( false );
		m_Graphics->fill();

		m_Graphics->setColor( true );

		m_Graphics->drawText( 0.05f, 0.2f, "Overwrite preset?", 1.0f );

		m_Graphics->drawCircleFilled( 0.15f, 0.5f, 0.05f );
		m_Graphics->drawText( 0.25f, 0.475f, "Effect1: Yes", 1.0f );

		m_Graphics->drawCircleFilled( 0.15f, 0.75f, 0.05f );
		m_Graphics->drawText( 0.25f, 0.725f, "Effect2: No", 1.0f );
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

void ARMor8UiManager::endLoading()
{
	this->returnToStatusMenu();
}

void ARMor8UiManager::tickForChangingBackToStatus()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
	{
		if ( m_TicksForChangingBackToStatus >= m_MaxTicksForChangingBackToStatus )
		{
			this->returnToStatusMenu();
			m_TicksForChangingBackToStatus = 0;
		}

		m_TicksForChangingBackToStatus++;
	}
}

void ARMor8UiManager::onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent)
{
	this->lockAllPots();

	ARMor8VoiceState voiceState = presetEvent.getPreset();
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
	m_Effect1PotAssignmentIndex = voiceState.pot1AssignmentIndex;
	m_Effect1PotAssignmentOp = voiceState.pot1AssignmentOp;
	m_Effect2PotAssignmentIndex = voiceState.pot2AssignmentIndex;
	m_Effect2PotAssignmentOp = voiceState.pot2AssignmentOp;
	m_Effect3PotAssignmentIndex = voiceState.pot3AssignmentIndex;
	m_Effect3PotAssignmentOp = voiceState.pot3AssignmentOp;

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

	this->returnToStatusMenu();
}

void ARMor8UiManager::onPotEvent (const PotEvent& potEvent)
{
	POT_CHANNEL channel = static_cast<POT_CHANNEL>( potEvent.getChannel() );
	float percentage = potEvent.getPercentage();

	float* potStabilizerBuf = nullptr;
	unsigned int* potStabilizerIndex = nullptr;
	float* potStabilizerValue = nullptr;
	float* potStabilizerCachedPer = nullptr;
	bool* lockedStatus = nullptr;
	float* lockCachedVal = nullptr;
	unsigned int assignmentIndex = 0;
	unsigned int assignmentOp = 0;

	switch ( channel )
	{
		case POT_CHANNEL::EFFECT1:
			potStabilizerBuf = m_Pot1StabilizerBuf;
			potStabilizerIndex = &m_Pot1StabilizerIndex;
			potStabilizerValue = &m_Pot1StabilizerValue;
			potStabilizerCachedPer = &m_Pot1StabilizerCachedPer;
			lockedStatus = &m_Effect1PotLocked;
			lockCachedVal = &m_Effect1PotCached;
			assignmentIndex = m_Effect1PotAssignmentIndex;
			assignmentOp = m_Effect1PotAssignmentOp;

			break;
		case POT_CHANNEL::EFFECT2:
			potStabilizerBuf = m_Pot2StabilizerBuf;
			potStabilizerIndex = &m_Pot2StabilizerIndex;
			potStabilizerValue = &m_Pot2StabilizerValue;
			potStabilizerCachedPer = &m_Pot2StabilizerCachedPer;
			lockedStatus = &m_Effect2PotLocked;
			lockCachedVal = &m_Effect2PotCached;
			assignmentIndex = m_Effect2PotAssignmentIndex;
			assignmentOp = m_Effect2PotAssignmentOp;

			break;
		case POT_CHANNEL::EFFECT3:
			potStabilizerBuf = m_Pot3StabilizerBuf;
			potStabilizerIndex = &m_Pot3StabilizerIndex;
			potStabilizerValue = &m_Pot3StabilizerValue;
			potStabilizerCachedPer = &m_Pot3StabilizerCachedPer;
			lockedStatus = &m_Effect3PotLocked;
			lockCachedVal = &m_Effect3PotCached;
			assignmentIndex = m_Effect3PotAssignmentIndex;
			assignmentOp = m_Effect3PotAssignmentOp;

			break;
		default:
			break;
	}

	// stabilize the potentiometer value by averaging all the values in the stabilizer buffers
	float averageValue = percentage;
	for ( unsigned int index = 0; index < ARMOR8_POT_STABIL_NUM; index++ )
	{
		averageValue += potStabilizerBuf[index];
	}
	averageValue = averageValue / ( static_cast<float>(ARMOR8_POT_STABIL_NUM) + 1.0f );

	// only if the current value breaks our 'hysteresis' do we actually set a new pot value
	if ( *potStabilizerValue != averageValue )
	{
		*potStabilizerValue = averageValue;

		if ( ! *lockedStatus ) // if not locked
		{
			bool menuThreshBroken = this->hasBrokenMenuChangeThreshold( *potStabilizerValue, *potStabilizerCachedPer );
			this->sendParamEventFromEffectPot( assignmentIndex, assignmentOp, *potStabilizerValue, menuThreshBroken );
		}
		else // if locked, update locked status
		{
			if ( this->hasBrokenLock(*lockedStatus, *lockCachedVal, *potStabilizerValue) )
			{
				bool menuThreshBroken = this->hasBrokenMenuChangeThreshold( *potStabilizerValue, *potStabilizerCachedPer );
				this->sendParamEventFromEffectPot( assignmentIndex, assignmentOp, *potStabilizerValue, menuThreshBroken );
			}
		}
	}

	// write value to buffer and increment index
	potStabilizerBuf[*potStabilizerIndex] = percentage;
	*potStabilizerIndex = ( *potStabilizerIndex + 1 ) % ARMOR8_POT_STABIL_NUM;
}

void ARMor8UiManager::onButtonEvent (const ButtonEvent& buttonEvent)
{
	static bool ignoreNextReleaseEffect1 = false;
	static bool ignoreNextReleaseEffect2 = false;

	if ( buttonEvent.getButtonState() == BUTTON_STATE::RELEASED )
	{
		if ( buttonEvent.getChannel() == static_cast<unsigned int>(BUTTON_CHANNEL::EFFECT1) )
		{
			if ( ignoreNextReleaseEffect1 )
			{
				ignoreNextReleaseEffect1 = false;
			}
			else
			{
				if ( m_Effect2BtnState == BUTTON_STATE::HELD ) // double button press
				{
					ignoreNextReleaseEffect2 = true;
					this->handleDoubleButtonPress();
				}
				else // single button press
				{
					this->handleEffect1SinglePress();
				}
			}
		}
		else if ( buttonEvent.getChannel() == static_cast<unsigned int>(BUTTON_CHANNEL::EFFECT2) )
		{
			if ( ignoreNextReleaseEffect2 )
			{
				ignoreNextReleaseEffect2 = false;
			}
			else
			{
				if ( m_Effect1BtnState == BUTTON_STATE::HELD ) // double button press
				{
					ignoreNextReleaseEffect1 = true;
					this->handleDoubleButtonPress();
				}
				else // single button press
				{
					this->handleEffect2SinglePress();
				}
			}
		}
	}
}

void ARMor8UiManager::processEffect1Btn (bool pressed)
{
	static BUTTON_STATE prevState = BUTTON_STATE::FLOATING;
	this->updateButtonState( m_Effect1BtnState, pressed );

	if ( prevState != m_Effect1BtnState )
	{
		prevState = m_Effect1BtnState;

		// this button event is then processed by this class' onButtonEvent with logic per menu
		IButtonEventListener::PublishEvent( ButtonEvent(prevState, static_cast<unsigned int>(BUTTON_CHANNEL::EFFECT1)) );
	}
}

void ARMor8UiManager::processEffect2Btn (bool pressed)
{
	static BUTTON_STATE prevState = BUTTON_STATE::FLOATING;
	this->updateButtonState( m_Effect2BtnState, pressed );

	if ( prevState != m_Effect2BtnState )
	{
		prevState = m_Effect2BtnState;

		// this button event is then processed by this class' onButtonEvent with logic per menu
		IButtonEventListener::PublishEvent( ButtonEvent(prevState, static_cast<unsigned int>(BUTTON_CHANNEL::EFFECT2)) );
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
	m_Effect1PotLocked = true;
	m_Effect2PotLocked = true;
	m_Effect3PotLocked = true;
	m_Effect1PotCached = m_Pot1StabilizerValue;
	m_Effect2PotCached = m_Pot2StabilizerValue;
	m_Effect3PotCached = m_Pot3StabilizerValue;
}

bool ARMor8UiManager::hasBrokenLock (bool& potLockedVal, float& potCachedVal, float newPotVal)
{
	bool hasBrokenLowerRange  = newPotVal <= ( potCachedVal - m_PotChangeThreshold );
	bool hasBrokenHigherRange = newPotVal >= ( potCachedVal + m_PotChangeThreshold );

	if ( potLockedVal && (hasBrokenHigherRange || hasBrokenLowerRange) ) // if locked but broke threshold
	{
		potLockedVal = false;
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
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
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
		m_CurrentMenu = ARMOR8_MENUS::STATUS_MAIN;
		this->draw();
	}
}

void ARMor8UiManager::refreshRatioFixed()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.625f, 0.93f, 1.0f, 1.0f );
		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.625f, 0.93f, m_RatioFixedStr, 1.0f );

		this->publishPartialLCDRefreshEvent( 0.625f, 0.93f, 1.0f, 1.0f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS_MAIN;
		this->draw();
	}
}

void ARMor8UiManager::refreshMonoPoly()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.0f, 0.93f, 0.4f, 1.0f );
		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.0f, 0.93f, m_MonoPolyStr, 1.0f );

		this->publishPartialLCDRefreshEvent( 0.0f, 0.93f, 0.4f, 1.0f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS_MAIN;
		this->draw();
	}
}

void ARMor8UiManager::refreshWave()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
	{
		m_Graphics->setColor( false );
		m_Graphics->drawBoxFilled( 0.37f, 0.94f, 0.63f, 1.0f );
		m_Graphics->setColor( true );
		m_Graphics->drawText( 0.37f, 0.94f, m_WaveStr, 1.0f );

		this->publishPartialLCDRefreshEvent( 0.37f, 0.94f, 0.63f, 1.0f );
	}
	else
	{
		m_CurrentMenu = ARMOR8_MENUS::STATUS_MAIN;
		this->draw();
	}
}

void ARMor8UiManager::refreshGlideRetrig()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
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
		m_CurrentMenu = ARMOR8_MENUS::STATUS_ADDITIONAL;
		this->draw();
	}
}

void ARMor8UiManager::returnToStatusMenu()
{
	// cache pot values
	m_Pot1StabilizerCachedPer = m_Pot1StabilizerValue;
	m_Pot2StabilizerCachedPer = m_Pot2StabilizerValue;
	m_Pot3StabilizerCachedPer = m_Pot3StabilizerValue;

	m_CurrentMenu = ARMOR8_MENUS::STATUS_MAIN;
	this->draw();
}

void ARMor8UiManager::enterStatusAdditionalMenu()
{
	// cache pot values
	m_Pot1StabilizerCachedPer = m_Pot1StabilizerValue;
	m_Pot2StabilizerCachedPer = m_Pot2StabilizerValue;
	m_Pot3StabilizerCachedPer = m_Pot3StabilizerValue;

	m_CurrentMenu = ARMOR8_MENUS::STATUS_ADDITIONAL;
	this->draw();
}

void ARMor8UiManager::enterSettingsMenu()
{
	m_CurrentMenu = ARMOR8_MENUS::SETTINGS_MAIN;
	m_SettingsMainModel.returnToTop();
	this->draw();
}

void ARMor8UiManager::enterAssignEffectPotMenu()
{
	m_CurrentMenu = ARMOR8_MENUS::ASSIGN_EFFECT_POT;

	// navigate to correct assignment
	unsigned int positionToNavigateTo = 0;
	if ( m_EffectPotToAssign == 1 )
	{
		positionToNavigateTo = m_Effect1PotAssignmentIndex;
	}
	else if ( m_EffectPotToAssign == 2 )
	{
		positionToNavigateTo = m_Effect2PotAssignmentIndex;
	}
	else if ( m_EffectPotToAssign == 3 )
	{
		positionToNavigateTo = m_Effect3PotAssignmentIndex;
	}

	m_AssignEffectPotModel.returnToTop();
	unsigned int cursorIndex = 0;
	while ( cursorIndex != positionToNavigateTo )
	{
		m_AssignEffectPotModel.advanceCursor();
		cursorIndex = m_AssignEffectPotModel.getEntryIndex();
	}

	this->draw();
}

void ARMor8UiManager::enterSelectOperatorMenu()
{
	m_CurrentMenu = ARMOR8_MENUS::SELECT_OPERATOR;
	m_SelectOperatorModel.returnToTop();
	this->draw();
}

void ARMor8UiManager::enterSelectWaveformMenu()
{
	m_CurrentMenu = ARMOR8_MENUS::SELECT_WAVEFORM;
	m_SelectWaveformModel.returnToTop();
	this->draw();
}

void ARMor8UiManager::enterWritePresetConfirmation()
{
	m_CurrentMenu = ARMOR8_MENUS::WRITE_PRESET_CONFIRMATION;
	this->draw();
}

void ARMor8UiManager::assignEffectPot()
{
	unsigned int cursorIndex = m_AssignEffectPotModel.getEntryIndex();

	// remember assignment
	if ( m_EffectPotToAssign == 1 )
	{
		m_Effect1PotAssignmentIndex = cursorIndex;
		m_Effect1PotAssignmentOp = m_OpCurrentlyBeingEdited;
		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(m_Effect1PotAssignmentIndex),
					m_Effect1PotAssignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::POT1_ASSIGNMENT)) );
	}
	else if ( m_EffectPotToAssign == 2 )
	{
		m_Effect2PotAssignmentIndex = cursorIndex;
		m_Effect2PotAssignmentOp = m_OpCurrentlyBeingEdited;
		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(m_Effect2PotAssignmentIndex),
					m_Effect2PotAssignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::POT2_ASSIGNMENT)) );
	}
	else if ( m_EffectPotToAssign == 3 )
	{
		m_Effect3PotAssignmentIndex = cursorIndex;
		m_Effect3PotAssignmentOp = m_OpCurrentlyBeingEdited;
		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(m_Effect3PotAssignmentIndex),
					m_Effect3PotAssignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::POT3_ASSIGNMENT)) );
	}

	this->lockAllPots();
}

void ARMor8UiManager::sendParamEventFromEffectPot (unsigned int assignmentIndex, unsigned int assignmentOp, float val, bool menuThreshBroken)
{
	unsigned int bufferLen = 20;
	char buffer[bufferLen];

	if ( assignmentIndex == m_AssignEffectPotMenuFreqIndex ) // frequency
	{
		val = AUDIO_TAPER( val );
		float frequencyAmount = val * ARMOR8_FREQUENCY_MAX;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(frequencyAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::FREQUENCY)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateFrequencyStr( frequencyAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.74f, 0.62f, 0.82f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.74f, m_FreqStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.74f, 0.62f, 0.82f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateFrequencyStr( frequencyAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuDetuneIndex ) // detune
	{
		int detuneAmount = std::round( (val * ARMOR8_DETUNE_MAX * 2.0f) - ARMOR8_DETUNE_MAX );
		float detuneAmountFloat = static_cast<float>( detuneAmount );
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(detuneAmountFloat, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::DETUNE)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateDetuneStr( detuneAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.0f, 1.0f, 0.08f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.0f, m_DetuneStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.0f, 1.0f, 0.08f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateDetuneStr( detuneAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuAttackIndex ) // eg attack
	{
		val = AUDIO_TAPER( val );
		float attackAmount = ( val * (ARMOR8_ATTACK_MAX - ARMOR8_ATTACK_MIN) ) + ARMOR8_ATTACK_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(attackAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_ATTACK)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateAttackStr( attackAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.16f, 0.5f, 0.26f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.16f, m_AttackStr,  1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.16f, 0.5f, 0.26f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateAttackStr( attackAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuDecayIndex ) // eg decay
	{
		val = AUDIO_TAPER( val );
		float decayAmount = ( val * (ARMOR8_DECAY_MAX - ARMOR8_DECAY_MIN) ) + ARMOR8_DECAY_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(decayAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_DECAY)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateDecayStr( decayAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.28f, 0.5f, 0.38f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.28f, m_DecayStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.28f, 0.5f, 0.38f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateDecayStr( decayAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuSustainIndex ) // eg sustain
	{
		float sustainAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(sustainAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_SUSTAIN)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateSustainStr( sustainAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.39f, 0.5f, 0.49f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.39f, m_SustainStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.39f, 0.5f, 0.49f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateSustainStr( sustainAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuReleaseIndex ) // eg release
	{
		val = AUDIO_TAPER( val );
		float releaseAmount = ( val * (ARMOR8_RELEASE_MAX - ARMOR8_RELEASE_MIN) ) + ARMOR8_RELEASE_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(releaseAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_RELEASE)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateReleaseStr( releaseAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.5f, 0.5f, 0.6f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.50f, m_ReleaseStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.5f, 0.5f, 0.6f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateReleaseStr( releaseAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuAtkExpoIndex ) // eg attack expo
	{
		float attackExpoAmount = ( val * (ARMOR8_EXPO_MAX - ARMOR8_EXPO_MIN) ) + ARMOR8_EXPO_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(attackExpoAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_ATTACK_EXPO)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateAttackExpoStr( attackExpoAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.16f, 0.6f, 0.26f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.16f, m_AttackExpoStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.16f, 0.6f, 0.26f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateAttackExpoStr( attackExpoAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuDecExpoIndex ) // eg decay expo
	{
		float decayExpoAmount = ( val * (ARMOR8_EXPO_MAX - ARMOR8_EXPO_MIN) ) + ARMOR8_EXPO_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(decayExpoAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_DECAY_EXPO)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateDecayExpoStr( decayExpoAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.3f, 0.6f, 0.4f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.30f, m_DecayExpoStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.3f, 0.6f, 0.4f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateDecayExpoStr( decayExpoAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuRelExpoIndex ) // eg release expo
	{
		float releaseExpoAmount = ( val * (ARMOR8_EXPO_MAX - ARMOR8_EXPO_MIN) ) + ARMOR8_EXPO_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(releaseExpoAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::EG_RELEASE_EXPO)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateReleaseExpoStr( releaseExpoAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.42f, 0.6f, 0.52f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.42f, m_ReleaseExpoStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.42f, 0.6f, 0.52f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateReleaseExpoStr( releaseExpoAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuOp1ModIndex ) // op1 modulation amount
	{
		val = AUDIO_TAPER( val );
		float op1ModAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(op1ModAmount * ARMOR8_OP_MOD_MAX, assignmentOp,
					static_cast<unsigned int>(PARAM_CHANNEL::OP_1_MOD_AMOUNT)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateOpModStr( 1, op1ModAmount, buffer, bufferLen, false );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.52f, 0.16f, 1.1f, 0.26f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.52f, 0.16f, m_Op1Str, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.52f, 0.16f, 1.0f, 0.28f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateOpModStr( 1, op1ModAmount, buffer, bufferLen, false );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuOp2ModIndex ) // op2 modulation amount
	{
		val = AUDIO_TAPER( val );
		float op2ModAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(op2ModAmount * ARMOR8_OP_MOD_MAX, assignmentOp,
					static_cast<unsigned int>(PARAM_CHANNEL::OP_2_MOD_AMOUNT)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateOpModStr( 2, op2ModAmount, buffer, bufferLen, false );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.52f, 0.28f, 1.1f, 0.38f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.52f, 0.28f, m_Op2Str, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.52f, 0.28f, 1.0f, 0.38f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateOpModStr( 2, op2ModAmount, buffer, bufferLen, false );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuOp3ModIndex ) // op3 modulation amount
	{
		val = AUDIO_TAPER( val );
		float op3ModAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(op3ModAmount * ARMOR8_OP_MOD_MAX, assignmentOp,
					static_cast<unsigned int>(PARAM_CHANNEL::OP_3_MOD_AMOUNT)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateOpModStr( 3, op3ModAmount, buffer, bufferLen, false );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.52f, 0.39f, 1.1f, 0.49f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.52f, 0.39f, m_Op3Str, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.52f, 0.39f, 1.0f, 0.49f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateOpModStr( 3, op3ModAmount, buffer, bufferLen, false );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuOp4ModIndex ) // op4 modulation amount
	{
		val = AUDIO_TAPER( val );
		float op4ModAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(op4ModAmount * ARMOR8_OP_MOD_MAX, assignmentOp,
					static_cast<unsigned int>(PARAM_CHANNEL::OP_4_MOD_AMOUNT)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateOpModStr( 4, op4ModAmount, buffer, bufferLen, false );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.52f, 0.50f, 1.1f, 0.60f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.52f, 0.50f, m_Op4Str, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.52f, 0.50f, 1.0f, 0.60f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateOpModStr( 4, op4ModAmount, buffer, bufferLen, false );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuAmplitudeIndex ) // amplitude
	{
		val = AUDIO_TAPER( val );
		float amplitudeAmount = val * ARMOR8_AMPLITUDE_MAX;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(amplitudeAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::AMPLITUDE)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateAmplitudeStr( amplitudeAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.65f, 0.62f, 0.73f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.65f, m_OpAmpStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.65f, 0.62f, 0.73f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateAmplitudeStr( amplitudeAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuFiltFreqIndex ) // filter frequency
	{
		val = AUDIO_TAPER( val );
		float filtFrequencyAmount = ( val * (ARMOR8_FILT_FREQ_MAX - ARMOR8_FILT_FREQ_MIN) ) + ARMOR8_FILT_FREQ_MIN;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(filtFrequencyAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::FILTER_FREQ)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateFiltFreqStr( filtFrequencyAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.83f, 0.62f, 0.92f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.83f, m_FiltFreqStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.83f, 0.62f, 0.92f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
		{
			this->updateFiltFreqStr( filtFrequencyAmount, buffer, bufferLen );

			this->returnToStatusMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuFiltResIndex ) // filter resonance
	{
		float filtResAmount = val * ARMOR8_FILT_RES_MAX;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(filtResAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::FILTER_RES)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateFiltResStr( filtResAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.71f, 0.3f, 0.93f, 0.4f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.71f, 0.3f, m_FiltResStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.71f, 0.3f, 0.93f, 0.4f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited &&  menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateFiltResStr( filtResAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuAmpVelSensIndex ) // amplitude velocity sensitivity
	{
		float amplitudeVelAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(amplitudeVelAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::AMP_VEL_SENS)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateAmplitudeVelStr( amplitudeVelAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.6f, 0.6f, 0.7f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.60f, m_AmplitudeVelStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.6f, 0.6f, 0.7f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateAmplitudeVelStr( amplitudeVelAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuFiltVelSensIndex ) // filter velocity sensitivity
	{
		float filterVelAmount = val;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(filterVelAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::FILT_VEL_SENS)) );

		if ( assignmentOp == m_OpCurrentlyBeingEdited
				&& (m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT) )
		{
			this->updateFilterVelStr( filterVelAmount, buffer, bufferLen );

			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.0f, 0.72f, 0.6f, 0.81f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( -0.02f, 0.72f, m_FiltVelStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.0f, 0.72f, 0.6f, 0.81f );
		}
		else if ( assignmentOp == m_OpCurrentlyBeingEdited && menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->updateFilterVelStr( filterVelAmount, buffer, bufferLen );

			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuPBendSemiIndex ) // pitch bend semitones
	{
		int pitchBendAmount = std::round( val * (ARMOR8_PITCH_BEND_MAX - ARMOR8_PITCH_BEND_MIN) + ARMOR8_PITCH_BEND_MIN );
		float pitchBendAmountFloat = static_cast<float>( pitchBendAmount );
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(pitchBendAmountFloat, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::PITCH_BEND_SEMI)) );
		this->updatePitchBendStr( pitchBendAmount, buffer, bufferLen );

		// operator independent (global value)
		if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT )
		{
			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.03f, 0.93f, 0.97f, 1.0f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.03f, 0.93f, m_PitchBendStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.03f, 0.93f, 0.97f, 1.0f );
		}
		else if ( menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->enterStatusAdditionalMenu();
		}
	}
	else if ( assignmentIndex == m_AssignEffectPotMenuGlideTimeIndex ) // glide time
	{
		float glideTimeAmount = val * ARMOR8_GLIDE_TIME_MAX;
		IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(glideTimeAmount, assignmentOp, static_cast<unsigned int>(PARAM_CHANNEL::GLIDE_TIME)) );
		this->updateGlideStr( glideTimeAmount, buffer, bufferLen );

		// operator independent (global value)
		if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL || m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT )
		{
			m_Graphics->setColor( false );
			m_Graphics->drawBoxFilled( 0.03f, 0.83f, 0.97f, 0.92f );
			m_Graphics->setColor( true );
			m_Graphics->drawText( 0.03f, 0.83f, m_GlideStr, 1.0f );

			this->publishPartialLCDRefreshEvent( 0.03f, 0.83f, 0.97f, 0.92f );
		}
		else if ( menuThreshBroken && m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN )
		{
			this->enterStatusAdditionalMenu();
		}
	}

}

void ARMor8UiManager::handleEffect1SinglePress()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
	{
		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(true, m_OpCurrentlyBeingEdited,
					static_cast<unsigned int>(PARAM_CHANNEL::PREV_PRESET)) );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SETTINGS_MAIN )
	{
		m_SettingsMainModel.reverseCursor();
		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT )
	{
		unsigned int previousCursorIndex = m_AssignEffectPotModel.getEntryIndex();
		if ( previousCursorIndex == 0 ) // if at the top already
		{
			m_AssignEffectPotModel.advanceToBottom();
		}
		else
		{
			m_AssignEffectPotModel.reverseCursor();
		}
		unsigned int cursorIndex = m_AssignEffectPotModel.getEntryIndex();
		unsigned int comparisonIndex1 = 0;
		unsigned int comparisonIndex2 = 0;
		unsigned int comparisonOp1 = 0;
		unsigned int comparisonOp2 = 0;
		if ( m_EffectPotToAssign == 1 )
		{
			comparisonIndex1 = m_Effect2PotAssignmentIndex;
			comparisonIndex2 = m_Effect3PotAssignmentIndex;
			comparisonOp1 = m_Effect2PotAssignmentOp;
			comparisonOp2 = m_Effect3PotAssignmentOp;
		}
		else if ( m_EffectPotToAssign == 2 )
		{
			comparisonIndex1 = m_Effect1PotAssignmentIndex;
			comparisonIndex2 = m_Effect3PotAssignmentIndex;
			comparisonOp1 = m_Effect1PotAssignmentOp;
			comparisonOp2 = m_Effect3PotAssignmentOp;
		}
		else if ( m_EffectPotToAssign == 3 )
		{
			comparisonIndex1 = m_Effect1PotAssignmentIndex;
			comparisonIndex2 = m_Effect2PotAssignmentIndex;
			comparisonOp1 = m_Effect1PotAssignmentOp;
			comparisonOp2 = m_Effect2PotAssignmentOp;
		}

		// ensure 2 effect pots aren't assigned to the same parameter on the same operator
		while ( (cursorIndex == comparisonIndex1 && m_OpCurrentlyBeingEdited == comparisonOp1)
				|| (cursorIndex == comparisonIndex2 && m_OpCurrentlyBeingEdited == comparisonOp2) )
		{
			if ( cursorIndex == 0 )
			{
				m_AssignEffectPotModel.advanceToBottom();
			}
			else
			{
				m_AssignEffectPotModel.reverseCursor();
			}

			cursorIndex = m_AssignEffectPotModel.getEntryIndex();
		}

		this->assignEffectPot();

		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_OPERATOR )
	{
		m_SelectOperatorModel.reverseCursor();
		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_WAVEFORM )
	{
		m_SelectWaveformModel.reverseCursor();
		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::WRITE_PRESET_CONFIRMATION )
	{
		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(true, m_OpCurrentlyBeingEdited,
					static_cast<unsigned int>(PARAM_CHANNEL::WRITE_PRESET)) );
		this->returnToStatusMenu();
	}
}

void ARMor8UiManager::handleEffect2SinglePress()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
	{
		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(true, m_OpCurrentlyBeingEdited,
					static_cast<unsigned int>(PARAM_CHANNEL::NEXT_PRESET)) );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SETTINGS_MAIN )
	{
		m_SettingsMainModel.advanceCursor();
		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT )
	{
		unsigned int previousCursorIndex = m_AssignEffectPotModel.getEntryIndex();
		if ( previousCursorIndex == m_AssignEffectPotModel.getTotalNumEntries() - 1 ) // if at the bottom already
		{
			m_AssignEffectPotModel.returnToTop();
		}
		else
		{
			m_AssignEffectPotModel.advanceCursor();
		}
		unsigned int cursorIndex = m_AssignEffectPotModel.getEntryIndex();
		unsigned int comparisonIndex1 = 0;
		unsigned int comparisonIndex2 = 0;
		unsigned int comparisonOp1 = 0;
		unsigned int comparisonOp2 = 0;
		if ( m_EffectPotToAssign == 1 )
		{
			comparisonIndex1 = m_Effect2PotAssignmentIndex;
			comparisonIndex2 = m_Effect3PotAssignmentIndex;
			comparisonOp1 = m_Effect2PotAssignmentOp;
			comparisonOp2 = m_Effect3PotAssignmentOp;
		}
		else if ( m_EffectPotToAssign == 2 )
		{
			comparisonIndex1 = m_Effect1PotAssignmentIndex;
			comparisonIndex2 = m_Effect3PotAssignmentIndex;
			comparisonOp1 = m_Effect1PotAssignmentOp;
			comparisonOp2 = m_Effect3PotAssignmentOp;
		}
		else if ( m_EffectPotToAssign == 3 )
		{
			comparisonIndex1 = m_Effect1PotAssignmentIndex;
			comparisonIndex2 = m_Effect2PotAssignmentIndex;
			comparisonOp1 = m_Effect1PotAssignmentOp;
			comparisonOp2 = m_Effect2PotAssignmentOp;
		}

		// ensure 2 effect pots aren't assigned to the same parameter on the same operator
		while ( (cursorIndex == comparisonIndex1 && m_OpCurrentlyBeingEdited == comparisonOp1)
				|| (cursorIndex == comparisonIndex2 && m_OpCurrentlyBeingEdited == comparisonOp2) )
		{
			if ( cursorIndex == m_AssignEffectPotModel.getTotalNumEntries() - 1 )
			{
				m_AssignEffectPotModel.returnToTop();
			}
			else
			{
				m_AssignEffectPotModel.advanceCursor();
			}

			cursorIndex = m_AssignEffectPotModel.getEntryIndex();
		}

		this->assignEffectPot();

		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_OPERATOR )
	{
		m_SelectOperatorModel.advanceCursor();
		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_WAVEFORM )
	{
		m_SelectWaveformModel.advanceCursor();
		this->draw();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::WRITE_PRESET_CONFIRMATION )
	{
		this->returnToStatusMenu();
	}
}

void ARMor8UiManager::handleDoubleButtonPress()
{
	if ( m_CurrentMenu == ARMOR8_MENUS::STATUS_MAIN || m_CurrentMenu == ARMOR8_MENUS::STATUS_ADDITIONAL )
	{
		this->enterSettingsMenu();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SETTINGS_MAIN )
	{
		unsigned int cursorIndex = m_SettingsMainModel.getEntryIndex();
		if ( cursorIndex == m_SettingsMenuExitMenuIndex ) // exit settings menu
		{
			this->returnToStatusMenu();
		}
		else if ( cursorIndex == m_SettingsMenuAssignEffect1Index ) // assign effect pot 1
		{
			m_EffectPotToAssign = 1;
			m_Effect1PotAssignmentOp = m_OpCurrentlyBeingEdited;
			this->lockAllPots();
			this->enterAssignEffectPotMenu();
		}
		else if ( cursorIndex == m_SettingsMenuAssignEffect2Index ) // assign effect pot 2
		{
			m_EffectPotToAssign = 2;
			m_Effect2PotAssignmentOp = m_OpCurrentlyBeingEdited;
			this->lockAllPots();
			this->enterAssignEffectPotMenu();
		}
		else if ( cursorIndex == m_SettingsMenuAssignEffect3Index ) // assign effect pot 3
		{
			m_EffectPotToAssign = 3;
			m_Effect3PotAssignmentOp = m_OpCurrentlyBeingEdited;
			this->lockAllPots();
			this->enterAssignEffectPotMenu();
		}
		else if ( cursorIndex == m_SettingsMenuSelectOperatorIndex ) // select operator to edit
		{
			this->enterSelectOperatorMenu();
		}
		else if ( cursorIndex == m_SettingsMenuSelectWaveformIndex ) // select waveform to use
		{
			this->enterSelectWaveformMenu();
		}
		else if ( cursorIndex == m_SettingsMenuUseRatioFreqIndex ) // toggle ratio for frequency
		{
			m_UsingRatio = !m_UsingRatio;
			this->updateRatioFixedStr();
			this->draw();
			IARMor8ParameterEventListener::PublishEvent(
				ARMor8ParameterEvent(static_cast<float>(m_UsingRatio), m_OpCurrentlyBeingEdited,
					static_cast<unsigned int>(PARAM_CHANNEL::USE_RATIO)) );
		}
		else if ( cursorIndex == m_SettingsMenuEGDestAmpIndex ) // toggle eg to amplitude
		{
			m_EGDestBitmask ^= 0b100;
			this->draw();
			if ( m_EGDestBitmask & 0b100 )
			{
				IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(true), m_OpCurrentlyBeingEdited,
							static_cast<unsigned int>(PARAM_CHANNEL::EG_DEST_AMP)) );
			}
			else
			{
				IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(false), m_OpCurrentlyBeingEdited,
							static_cast<unsigned int>(PARAM_CHANNEL::EG_DEST_AMP)) );
			}
		}
		else if ( cursorIndex == m_SettingsMenuEGDestFreqIndex ) // toggle eg to frequency
		{
			m_EGDestBitmask ^= 0b010;
			this->draw();
			if ( m_EGDestBitmask & 0b010 )
			{
				IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(true), m_OpCurrentlyBeingEdited,
							static_cast<unsigned int>(PARAM_CHANNEL::EG_DEST_FREQ)) );
			}
			else
			{
				IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(false), m_OpCurrentlyBeingEdited,
							static_cast<unsigned int>(PARAM_CHANNEL::EG_DEST_FREQ)) );
			}
		}
		else if ( cursorIndex == m_SettingsMenuEGDestFiltIndex ) // toggle eg to filter
		{
			m_EGDestBitmask ^= 0b001;
			this->draw();
			if ( m_EGDestBitmask & 0b001 )
			{
				IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(true), m_OpCurrentlyBeingEdited,
							static_cast<unsigned int>(PARAM_CHANNEL::EG_DEST_FILT)) );
			}
			else
			{
				IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(false), m_OpCurrentlyBeingEdited,
							static_cast<unsigned int>(PARAM_CHANNEL::EG_DEST_FILT)) );
			}
		}
		else if ( cursorIndex == m_SettingsMenuGlideRetrigIndex ) // toggle glide retrigger
		{
			m_UsingGlideRetrigger = !m_UsingGlideRetrigger;
			this->draw();
			IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(m_UsingGlideRetrigger),
						m_OpCurrentlyBeingEdited, static_cast<unsigned int>(PARAM_CHANNEL::GLIDE_RETRIG)) );
		}
		else if ( cursorIndex == m_SettingsMenuMonophonicIndex ) // toggle monophonic mode
		{
			m_UsingMono = !m_UsingMono;
			float floatVal = ( m_UsingMono ) ? 1.0f : 0.0f;
			this->updateMonoPolyStr();
			this->draw();
			IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(floatVal, m_OpCurrentlyBeingEdited,
									static_cast<unsigned int>(PARAM_CHANNEL::MONOPHONIC)) );
		}
		else if ( cursorIndex == m_SettingsMenuWritePresetIndex ) // write preset (leads to confirmation page)
		{
			this->enterWritePresetConfirmation();
		}
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::ASSIGN_EFFECT_POT )
	{
		this->enterSettingsMenu();
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_OPERATOR )
	{
		unsigned int cursorIndex = m_SelectOperatorModel.getEntryIndex();
		if ( cursorIndex == m_SelectOperatorMenuOp1Index ) // start editing operator 1
		{
			m_OpCurrentlyBeingEdited = 1;
			this->enterSettingsMenu();
		}
		else if ( cursorIndex == m_SelectOperatorMenuOp2Index ) // start editing operator 2
		{
			m_OpCurrentlyBeingEdited = 2;
			this->enterSettingsMenu();
		}
		else if ( cursorIndex == m_SelectOperatorMenuOp3Index ) // start editing operator 3
		{
			m_OpCurrentlyBeingEdited = 3;
			this->enterSettingsMenu();
		}
		else if ( cursorIndex == m_SelectOperatorMenuOp4Index ) // start editing operator 4
		{
			m_OpCurrentlyBeingEdited = 4;
			this->enterSettingsMenu();
		}
		else if ( cursorIndex == m_SelectOperatorMenuExitMenuIndex ) // exit back to settings menu
		{
			this->enterSettingsMenu();
			return;
		}

		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(0.0f, // doesn't matter
					m_OpCurrentlyBeingEdited, static_cast<unsigned int>(PARAM_CHANNEL::SELECT_OPERATOR)) );
	}
	else if ( m_CurrentMenu == ARMOR8_MENUS::SELECT_WAVEFORM )
	{
		unsigned int cursorIndex = m_SelectWaveformModel.getEntryIndex();
		OscillatorMode mode = OscillatorMode::SINE;
		if ( cursorIndex == m_SelectWaveformMenuSineIndex ) // use sine wave
		{
			m_WaveNumCurrentlyBeingEdited = 1;
			this->enterSettingsMenu();
			mode = OscillatorMode::SINE;
		}
		else if ( cursorIndex == m_SelectWaveformMenuTriIndex ) // use tri wave
		{
			m_WaveNumCurrentlyBeingEdited = 2;
			this->enterSettingsMenu();
			mode = OscillatorMode::TRIANGLE;
		}
		else if ( cursorIndex == m_SelectWaveformMenuSquareIndex ) // use square wave
		{
			m_WaveNumCurrentlyBeingEdited = 3;
			this->enterSettingsMenu();
			mode = OscillatorMode::SQUARE;
		}
		else if ( cursorIndex == m_SelectWaveformMenuSawIndex ) // use saw wave
		{
			m_WaveNumCurrentlyBeingEdited = 4;
			this->enterSettingsMenu();
			mode = OscillatorMode::SAWTOOTH;
		}
		else if ( cursorIndex == m_SelectWaveformMenuExitMenuIndex ) // exit back to settings menu
		{
			this->enterSettingsMenu();
			return;
		}

		IARMor8ParameterEventListener::PublishEvent( ARMor8ParameterEvent(static_cast<float>(mode),
					m_OpCurrentlyBeingEdited, static_cast<unsigned int>(PARAM_CHANNEL::SELECT_WAVEFORM)) );
	}
}

bool ARMor8UiManager::shouldTickSettingsMenu (unsigned int entryIndex)
{

	if ( (entryIndex == m_SettingsMenuUseRatioFreqIndex && m_UsingRatio)
		|| (entryIndex == m_SettingsMenuEGDestAmpIndex && (m_EGDestBitmask & 0b100))
		|| (entryIndex == m_SettingsMenuEGDestFreqIndex && (m_EGDestBitmask & 0b010))
		|| (entryIndex == m_SettingsMenuEGDestFiltIndex && (m_EGDestBitmask & 0b001))
		|| (entryIndex == m_SettingsMenuGlideRetrigIndex && m_UsingGlideRetrigger)
		|| (entryIndex == m_SettingsMenuMonophonicIndex && m_UsingMono) )
	{
		return true;
	}

	return false;
}

void ARMor8UiManager::drawScrollableMenu (ScrollableMenuModel& menu, bool (ARMor8UiManager::*shouldTickFunc)(unsigned int), ARMor8UiManager& ui)
{
	m_Graphics->setColor( false );
	m_Graphics->fill();

	m_Graphics->setColor( true );

	// draw cursor
	float yOffset = (0.15f * menu.getCursorIndex());
	m_Graphics->drawTriangleFilled( 0.075f, 0.1f + yOffset, 0.075f, 0.2f + yOffset, 0.125f, 0.15f + yOffset );

	// draw entries
	char** entries = menu.getEntries();
	unsigned int firstEntryIndex = menu.getFirstVisibleIndex();
	unsigned int entryNum = 0;
	char* entry = entries[entryNum];
	yOffset = 0.1f;
	const float tickOffset = 0.1f;
	while ( entry != nullptr && entryNum < SETTINGS_NUM_VISIBLE_ENTRIES )
	{
		const unsigned int actualIndex = firstEntryIndex + entryNum;
		if ( menu.indexIsTickable(actualIndex) )
		{
			bool fillCircle = false;
			if ( shouldTickFunc != nullptr && (ui.*shouldTickFunc)(actualIndex) )
			{
				fillCircle = true;
			}

			if ( fillCircle )
			{
				m_Graphics->drawCircleFilled( 0.175f, 0.05f + yOffset, 0.025f );
			}
			else
			{
				m_Graphics->drawCircle( 0.175f, 0.05f + yOffset, 0.025f );
			}

			m_Graphics->drawText( 0.15f + tickOffset, yOffset, entry, 1.0f );
		}
		else
		{
			m_Graphics->drawText( 0.15f, yOffset, entry, 1.0f );
		}
		yOffset += 0.15f;
		entryNum++;
		entry = entries[entryNum];
	}
}

bool ARMor8UiManager::hasBrokenMenuChangeThreshold (float newVal, float cachedVal)
{
	if ( std::abs(newVal - cachedVal) > ARMOR8_POT_MENU_CHANGE_THRESH )
	{
		return true;
	}

	return false;
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
