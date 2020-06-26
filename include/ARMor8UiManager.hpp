#ifndef ARMOR8UIMANAGER_HPP
#define ARMOR8UIMANAGER_HPP

/*************************************************************************
 * The ARMor8UiManager is a SIGL Surface that receives events when
 * the ARMor8VoiceManager changes a parameter and refreshes the frame
 * buffer accordingly. In order to be efficient as possible it also
 * sends screen refresh events indicating the part of the screen that
 * needs to be refreshed. This way the actual lcd HAL can send only the
 * relevant data, instead of an entire frame buffer.
*************************************************************************/

#include "Surface.hpp"

#include "IButtonEventListener.hpp"

class Font;
class Sprite;

class ARMor8UiManager : public Surface
{
	public:
		ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format);
		~ARMor8UiManager() override;

		void setFont (Font* font);
		void setLogo (Sprite* logo);

		void draw() override;

		void processFreqOrDetunePot               (float percentage);
		void processAtkOrAtkExpoOrOp1ModPot       (float percentage);
		void processDecOrDecExpoOrOp2ModPot       (float percentage);
		void processSusOrOp3ModPot                (float percentage);
		void processRelOrRelExpoOrOp4ModPot       (float percentage);
		void processAmpOrAmpVelPot                (float percentage);
		void processFiltFreqOrFiltResOrFiltVelPot (float percentage);
		void processPitchBendOrGlidePot           (float percentage);

		void processAlt1Btn         (bool pressed);
		void processAlt2Btn         (bool pressed);
		void processRatioOrFixedBtn (bool pressed);
		void processNextOpBtn       (bool pressed);
		void processNextWaveBtn     (bool pressed);
		void processGlideRetrigBtn  (bool pressed);
		void processMonoBtn         (bool pressed);
		void processPrevPresetBtn   (bool pressed);
		void processNextPresetBtn   (bool pressed);
		void processWritePresetBtn  (bool pressed);

	private:
		Sprite* 	m_Logo;

		BUTTON_STATE 	m_Alt1State;
		BUTTON_STATE 	m_Alt2State;
		float 		m_FreqPotCached;
		float 		m_DetunePotCached;
		float 		m_AttackPotCached;
		float 		m_AttackExpoPotCached;
		float 		m_Op1ModPotCached;
		float 		m_DecayPotCached;
		float 		m_DecayExpoPotCached;
		float 		m_Op2ModPotCached;
		float 		m_SustainPotCached;
		float 		m_Op3ModPotCached;
		float 		m_ReleasePotCached;
		float 		m_ReleaseExpoPotCached;
		float 		m_Op4ModPotCached;
		float 		m_AmplitudePotCached;
		float 		m_AmplitudeVelPotCached;
		float 		m_FiltFreqPotCached;
		float 		m_FiltResPotCached;
		float 		m_FiltVelPotCached;
		float 		m_PitchBendPotCached;
		float 		m_GlidePotCached;

		void updateButtonState (BUTTON_STATE& buttonState, bool pressed); // note: buttonState is an output variable
};

#endif // ARMOR8UIMANAGER_HPP
