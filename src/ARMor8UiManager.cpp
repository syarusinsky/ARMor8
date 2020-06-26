#include "ARMor8UiManager.hpp"

#include "ARMor8Constants.hpp"
#include "Graphics.hpp"
#include "IPotEventListener.hpp"

#include <iostream>

ARMor8UiManager::ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	Surface( width, height, format ),
	m_Logo( nullptr ),
	m_Alt1State( BUTTON_STATE::FLOATING ),
	m_Alt2State( BUTTON_STATE::FLOATING ),
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
	m_GlidePotCached( 0.0f )
{
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
	m_Graphics->setColor( false );
	m_Graphics->fill();

	m_Graphics->setColor( true );
	m_Graphics->drawText( 0.2f, 0.45f, "Test string!", 1.0f );

	m_Graphics->drawLine( 0.1f, 0.9f, 0.8f, 0.95f );

	m_Graphics->drawSprite( 0.0f, 0.0f, *m_Logo );
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
}

void ARMor8UiManager::processNextOpBtn (bool pressed)
{
}

void ARMor8UiManager::processNextWaveBtn (bool pressed)
{
}

void ARMor8UiManager::processGlideRetrigBtn (bool pressed)
{
}

void ARMor8UiManager::processMonoBtn (bool pressed)
{
}

void ARMor8UiManager::processPrevPresetBtn (bool pressed)
{
}

void ARMor8UiManager::processNextPresetBtn (bool pressed)
{
}

void ARMor8UiManager::processWritePresetBtn (bool pressed)
{
}

void ARMor8UiManager::updateButtonState (BUTTON_STATE& buttonState, bool pressed)
{
	if ( pressed )
	{
		if ( buttonState == BUTTON_STATE::FLOATING || buttonState == BUTTON_STATE::RELEASED )
		{
			std::cout << "PRESSED" << std::endl;
			buttonState = BUTTON_STATE::PRESSED;
		}
		else
		{
			std::cout << "HELD" << std::endl;
			buttonState = BUTTON_STATE::HELD;
		}
	}
	else
	{
		if ( buttonState == BUTTON_STATE::PRESSED || buttonState == BUTTON_STATE::HELD )
		{
			std::cout << "RELEASED" << std::endl;
			buttonState == BUTTON_STATE::RELEASED;
		}
		else
		{
			std::cout << "FLOATING" << std::endl;
			buttonState == BUTTON_STATE::FLOATING;
		}

	}
}
