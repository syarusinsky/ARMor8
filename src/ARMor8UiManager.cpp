#include "ARMor8UiManager.hpp"

#include "ARMor8Constants.hpp"
#include "Graphics.hpp"
#include "Sprite.hpp"
#include "IARMor8LCDRefreshEventListener.hpp"
#include "IPotEventListener.hpp"
#include "IButtonEventListener.hpp"

#include <iostream>

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
		m_Graphics->drawText( 0.0f, 0.0f, "[PRST: 1][OP: 1]", 1.0f );

		m_Graphics->drawLine( 0.0f, 0.12f, 1.0f, 0.12f );
		m_Graphics->drawLine( 0.0f, 0.14f, 1.0f, 0.14f );

		m_Graphics->drawText( -0.02f, 0.16f, "ATK:0.002", 1.0f );
		m_Graphics->drawText( -0.02f, 0.28f, "DEC:0.002", 1.0f );
		m_Graphics->drawText( -0.02f, 0.39f, "SUS:1.000", 1.0f );
		m_Graphics->drawText( -0.02f, 0.50f, "REL:0.002", 1.0f );

		m_Graphics->drawLine( 0.5f, 0.14f, 0.5f, 0.6f );

		m_Graphics->drawText( 0.52f, 0.16f, "OP1:0.002", 1.0f );
		m_Graphics->drawText( 0.52f, 0.28f, "OP2:0.005", 1.0f );
		m_Graphics->drawText( 0.52f, 0.39f, "OP3:0.002", 1.0f );
		m_Graphics->drawText( 0.52f, 0.50f, "OP4:0.000", 1.0f );

		m_Graphics->drawLine( 0.1f, 0.61f, 0.9f, 0.61f );

		m_Graphics->drawText( 0.14f, 0.65f, "OP AMP: 1.002", 1.0f );

		m_Graphics->drawText( 0.06f, 0.74f, "FILT FREQ: 20000", 1.0f );

		m_Graphics->drawText( 0.0f, 0.92f, "[MONO]", 1.0f );

		m_Logo->setRotationAngle( 0 );
		m_Logo->setScaleFactor( 0.25f );
		m_Graphics->drawSprite( 0.45f, 0.41f, *m_Logo );

		m_Graphics->drawText( 0.625f, 0.92f, "[RATIO]", 1.0f );
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

void ARMor8UiManager::onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent)
{
	m_CurrentMenu = ARMOR8_MENUS::STATUS;
	this->draw();

	ARMor8VoiceState voiceState = presetEvent.getPreset();
	m_OpCurrentlyBeingEdited = presetEvent.getOpToEdit() + 1;
	m_CurrentPresetNum = presetEvent.getPresetNum() + 1;

	bool egDestAmpl = false;
	bool egDestFreq = false;
	bool egDestFilt = false;
	bool usingRatio = false;
	bool usingGlide = voiceState.glideRetrigger;
	bool usingMono  = voiceState.monophonic;

	switch ( m_OpCurrentlyBeingEdited )
	{
		case 1:
			egDestAmpl = voiceState.egAmplitudeMod1;
			egDestFreq = voiceState.egFrequencyMod1;
			egDestFilt = voiceState.egFilterMod1;
			usingRatio = voiceState.useRatio1;

			break;
		case 2:
			egDestAmpl = voiceState.egAmplitudeMod2;
			egDestFreq = voiceState.egFrequencyMod2;
			egDestFilt = voiceState.egFilterMod2;
			usingRatio = voiceState.useRatio2;

			break;
		case 3:
			egDestAmpl = voiceState.egAmplitudeMod3;
			egDestFreq = voiceState.egFrequencyMod3;
			egDestFilt = voiceState.egFilterMod3;
			usingRatio = voiceState.useRatio3;

			break;
		case 4:
			egDestAmpl = voiceState.egAmplitudeMod4;
			egDestFreq = voiceState.egFrequencyMod4;
			egDestFilt = voiceState.egFilterMod4;
			usingRatio = voiceState.useRatio4;

			break;
		default:
			break;
	}

	m_EGDestBitmask = 0b000;

	if ( egDestAmpl ) m_EGDestBitmask = m_EGDestBitmask | 0b100;
	if ( egDestFreq ) m_EGDestBitmask = m_EGDestBitmask | 0b010;
	if ( egDestFilt ) m_EGDestBitmask = m_EGDestBitmask | 0b001;

	m_UsingRatio = usingRatio;
	m_UsingGlideRetrigger = usingGlide;
	m_UsingMono = usingMono;

	std::cout << "OP TO EDIT: " << m_OpCurrentlyBeingEdited << std::endl;
	std::cout << "PRESET NUM: " << m_CurrentPresetNum << std::endl;
	std::cout << "EG DEST: " << static_cast<unsigned int>(m_EGDestBitmask) << std::endl;
	std::cout << "RATIO: " << std::to_string( m_UsingRatio ) << std::endl;
	std::cout << "GLIDE RETRIG: " << std::to_string( m_UsingGlideRetrigger ) << std::endl;
	std::cout << "MONO: " << std::to_string( m_UsingMono ) << std::endl;
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
}

void ARMor8UiManager::processFreqOrDetunePot (float percentage)
{
	if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		// detune
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::DETUNE)) );
	}
	else
	{
		// frequency
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::FREQUENCY)) );
	}
}

void ARMor8UiManager::processAtkOrAtkExpoOrOp1ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		// op 1 mod
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP1_MOD_AMT)) );
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		// attack expo
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::ATTACK_EXPO)) );
	}
	else
	{
		// attack
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::ATTACK)) );
	}
}

void ARMor8UiManager::processDecOrDecExpoOrOp2ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		// op 2 mod
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP2_MOD_AMT)) );
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		// decay expo
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::DECAY_EXPO)) );
	}
	else
	{
		// decay
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::DECAY)) );
	}
}

void ARMor8UiManager::processSusOrOp3ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		// op 3 mod
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP3_MOD_AMT)) );
	}
	else
	{
		// sustain
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::SUSTAIN)) );
	}
}

void ARMor8UiManager::processRelOrRelExpoOrOp4ModPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		// op 4 mod
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::OP4_MOD_AMT)) );
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		// release expo
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::RELEASE_EXPO)) );
	}
	else
	{
		// release
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::RELEASE)) );
	}
}

void ARMor8UiManager::processAmpOrAmpVelPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		// amplitude velocity
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::VEL_AMP)) );
	}
	else
	{
		// amplitude
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::AMPLITUDE)) );
	}
}

void ARMor8UiManager::processFiltFreqOrFiltResOrFiltVelPot (float percentage)
{
	if ( m_Alt2State == BUTTON_STATE::HELD )
	{
		// filter velocity
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::VEL_FILT)) );
	}
	else if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		// filter resonance
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::FILT_RES)) );
	}
	else
	{
		// filter frequency
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::FILT_FREQ)) );
	}
}

void ARMor8UiManager::processPitchBendOrGlidePot (float percentage)
{
	if ( m_Alt1State == BUTTON_STATE::HELD )
	{
		// pitch bend
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::PITCH_BEND)) );
	}
	else
	{
		// glide time
		IPotEventListener::PublishEvent( PotEvent(percentage, static_cast<unsigned int>(POT_CHANNEL::GLIDE_TIME)) );
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
			std::cout << "SETTING RATIO FREQ ON" << std::endl;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::RATIO)) );
		}
		else
		{
			std::cout << "SETTING FIXED FREQ ON" << std::endl;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::RATIO)) );
		}
	}
}

void ARMor8UiManager::processNextOpBtn (bool pressed)
{
	this->updateButtonState( m_NextOpBtnState, pressed );

	if ( m_NextOpBtnState == BUTTON_STATE::RELEASED )
	{
		if ( m_OpCurrentlyBeingEdited == 1 )
		{
			std::cout << "NOW ON OP2" << std::endl;
			m_OpCurrentlyBeingEdited = 2;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP2)) );
		}
		else if ( m_OpCurrentlyBeingEdited == 2 )
		{
			std::cout << "NOW ON OP3" << std::endl;
			m_OpCurrentlyBeingEdited = 3;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP3)) );
		}
		else if ( m_OpCurrentlyBeingEdited == 3 )
		{
			std::cout << "NOW ON OP4" << std::endl;
			m_OpCurrentlyBeingEdited = 4;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::OP4)) );
		}
		else if ( m_OpCurrentlyBeingEdited == 4 )
		{
			std::cout << "NOW ON OP1" << std::endl;
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
			std::cout << "NOW ON TRIANGLE" << std::endl;
			m_WaveNumCurrentlyBeingEdited = 2;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::TRIANGLE)) );
		}
		else if ( m_WaveNumCurrentlyBeingEdited == 2 )
		{
			std::cout << "NOW ON SQUARE" << std::endl;
			m_WaveNumCurrentlyBeingEdited = 3;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::SQUARE)) );
		}
		else if ( m_WaveNumCurrentlyBeingEdited == 3 )
		{
			std::cout << "NOW ON SAWTOOTH" << std::endl;
			m_WaveNumCurrentlyBeingEdited = 4;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::SAWTOOTH)) );
		}
		else if ( m_WaveNumCurrentlyBeingEdited == 4 )
		{
			std::cout << "NOW ON SINE" << std::endl;
			m_WaveNumCurrentlyBeingEdited = 1;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
								static_cast<unsigned int>(BUTTON_CHANNEL::SINE)) );
		}
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
			std::cout << "SETTING GLIDE RETRIGGER" << std::endl;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
								static_cast<unsigned int>(BUTTON_CHANNEL::GLIDE_RETRIG)) );
		}
		else
		{
			std::cout << "DISABLING GLIDE RETRIGGER" << std::endl;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
								static_cast<unsigned int>(BUTTON_CHANNEL::GLIDE_RETRIG)) );
		}
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
			std::cout << "SETTING MONOPHONIC" << std::endl;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
								static_cast<unsigned int>(BUTTON_CHANNEL::MONOPHONIC)) );
		}
		else
		{
			std::cout << "SETTING POLYPHONIC" << std::endl;
			IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
								static_cast<unsigned int>(BUTTON_CHANNEL::MONOPHONIC)) );
		}
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
	}
}

void ARMor8UiManager::processPrevPresetBtn (bool pressed)
{
	this->updateButtonState( m_PrevPresetBtnState, pressed );

	if ( m_PrevPresetBtnState == BUTTON_STATE::RELEASED )
	{
		std::cout << "PREVIOUS PRESET BUTTON PRESSED" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::PREV_PRESET)) );
	}
}

void ARMor8UiManager::processNextPresetBtn (bool pressed)
{
	this->updateButtonState( m_NextPresetBtnState, pressed );

	if ( m_NextPresetBtnState == BUTTON_STATE::RELEASED )
	{
		std::cout << "NEXT PRESET BUTTON PRESSED" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::RELEASED,
							static_cast<unsigned int>(BUTTON_CHANNEL::NEXT_PRESET)) );
	}
}

void ARMor8UiManager::processWritePresetBtn (bool pressed)
{
	this->updateButtonState( m_WritePresetBtnState, pressed );

	if ( m_WritePresetBtnState == BUTTON_STATE::RELEASED )
	{
		std::cout << "WRITE PRESET BUTTON PRESSED" << std::endl;
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
		std::cout << "SETTING EG DEST TO AMPLITUDE" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_AMP)) );
	}
	else
	{
		std::cout << "DISABLING EG DEST TO AMPLITUDE" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_AMP)) );
	}

	if ( frequencyActive )
	{
		std::cout << "SETTING EG DEST TO FREQUENCY" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FREQ)) );
	}
	else
	{
		std::cout << "DISABLING EG DEST TO FREQUENCY" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::FLOATING,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FREQ)) );
	}

	if ( filtrFreqActive )
	{
		std::cout << "SETTING EG DEST TO FILTER" << std::endl;
		IButtonEventListener::PublishEvent( ButtonEvent(BUTTON_STATE::HELD,
							static_cast<unsigned int>(BUTTON_CHANNEL::EG_FILT)) );
	}
	else
	{
		std::cout << "DISABLING EG DEST TO FILTER" << std::endl;
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
