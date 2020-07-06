#ifndef ARMOR8UIMANAGER_HPP
#define ARMOR8UIMANAGER_HPP

/*************************************************************************
 * The ARMor8UiManager is a SIGL Surface that receives events when
 * the ARMor8 changes a parameter and refreshes the contents of the frame
 * buffer accordingly. In order to be efficient as possible it also
 * sends screen refresh events indicating the part of the screen that
 * needs to be refreshed. This way the actual lcd HAL can send only the
 * relevant data, instead of an entire frame buffer.
*************************************************************************/

#include "Surface.hpp"

#include "IARMor8PresetEventListener.hpp"
#include "IARMor8ParameterEventListener.hpp"
#include "IButtonEventListener.hpp"

enum class ARMOR8_MENUS : unsigned int
{
	LOADING,
	STATUS,
	ADDITIONAL
};

class Font;
class Sprite;

class ARMor8UiManager : public Surface, public IARMor8PresetEventListener, public IARMor8ParameterEventListener
{
	public:
		ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format);
		~ARMor8UiManager() override;

		void setFont (Font* font);
		void setLogo (Sprite* logo);

		void draw() override;
		void drawLoadingLogo();

		void tickForChangingBackToStatus();

		void onARMor8PresetChangedEvent (const ARMor8PresetEvent& presetEvent) override;

		void onARMor8ParameterEvent (const ARMor8ParameterEvent& paramEvent) override;

		void setEGDestAmplitude (bool on);
		void setEGDestFrequency (bool on);
		void setEGDestFiltrFreq (bool on);

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
		void processEGDestBtn       (bool pressed);
		void processPrevPresetBtn   (bool pressed);
		void processNextPresetBtn   (bool pressed);
		void processWritePresetBtn  (bool pressed);

	private:
		Sprite* 	m_Logo;

		ARMOR8_MENUS 	m_CurrentMenu;

		unsigned int    m_TicksForChangingBackToStatus;
		const unsigned int m_MaxTicksForChangingBackToStatus = 300;

		unsigned int 	m_CurrentPresetNum;
		unsigned int 	m_OpCurrentlyBeingEdited; 	// 1 for op1, 2 for op2, 3 for op3, 4 for op4
		unsigned int 	m_WaveNumCurrentlyBeingEdited; 	// 1 for sine, 2 for triangle, 3 for square, 4 for sawtooth
		uint8_t 	m_EGDestBitmask; // 0b000 with the leftmost bit representing amplitude, then frequency, then filter
		bool 		m_UsingRatio;
		bool 		m_UsingGlideRetrigger;
		bool 		m_UsingMono;

		char 		m_PrstAndOpStr[18];
		char 		m_AttackStr[10];
		char 		m_DecayStr[10];
		char 		m_SustainStr[10];
		char 		m_ReleaseStr[10];
		char 		m_Op1Str[10];
		char 		m_Op2Str[10];
		char 		m_Op3Str[10];
		char 		m_Op4Str[10];
		char 		m_OpAmpStr[12];
		char 		m_FreqStr[12];
		char 		m_FiltFreqStr[12];
		char 		m_MonoPolyStr[7];
		char 		m_WaveStr[5];
		char 		m_RatioFixedStr[8];
		char 		m_DetuneStr[19];
		char 		m_AttackExpoStr[12];
		char 		m_DecayExpoStr[12];
		char 		m_ReleaseExpoStr[12];
		char 		m_AmplitudeVelStr[12];
		char 		m_FiltVelStr[12];
		char 		m_GlideStr[18];
		char 		m_PitchBendStr[18];
		char 		m_FiltResStr[5];

		// pot cached values for parameter thresholds (so preset parameters don't change unless moved by a certain amount)
		const float     m_PotChangeThreshold = 0.4f; // the pot value needs to break out of this threshold to be applied
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
		// these keep track of whether the given pots are 'locked' by the threshold when switching prsts or ops
		bool 		m_FreqPotLocked;
		bool 		m_DetunePotLocked;
		bool 		m_AttackPotLocked;
		bool 		m_AttackExpoPotLocked;
		bool 		m_Op1ModPotLocked;
		bool 		m_DecayPotLocked;
		bool 		m_DecayExpoPotLocked;
		bool 		m_Op2ModPotLocked;
		bool 		m_SustainPotLocked;
		bool 		m_Op3ModPotLocked;
		bool 		m_ReleasePotLocked;
		bool 		m_ReleaseExpoPotLocked;
		bool 		m_Op4ModPotLocked;
		bool 		m_AmplitudePotLocked;
		bool 		m_AmplitudeVelPotLocked;
		bool 		m_FiltFreqPotLocked;
		bool 		m_FiltResPotLocked;
		bool 		m_FiltVelPotLocked;
		bool 		m_PitchBendPotLocked;
		bool 		m_GlidePotLocked;

		// button states for managing when button events are processed
		BUTTON_STATE 	m_Alt1State;
		BUTTON_STATE 	m_Alt2State;
		BUTTON_STATE 	m_RatioOrFixedBtnState;
		BUTTON_STATE 	m_NextOpBtnState;
		BUTTON_STATE 	m_NextWaveBtnState;
		BUTTON_STATE 	m_GlideRetrigBtnState;
		BUTTON_STATE 	m_MonoBtnState;
		BUTTON_STATE 	m_EGDestBtnState;
		BUTTON_STATE 	m_PrevPresetBtnState;
		BUTTON_STATE 	m_NextPresetBtnState;
		BUTTON_STATE 	m_WritePresetBtnState;

		void updateButtonState (BUTTON_STATE& buttonState, bool pressed); // note: buttonState is an output variable
		void updateEGDestState();
		void publishPartialLCDRefreshEvent (float xStart, float yStart, float xEnd, float yEnd);

		void lockAllPots();

		bool hasBrokenLock (bool& potLockedVal, float& potCachedVal, float newPotVal);

		void updateOpNumberStr (char* buffer, unsigned int bufferLen);
		void updatePrstNumberStr (char* buffer, unsigned int bufferLen);
		void updateMonoPolyStr();
		void updateWaveStr();
		void updateRatioFixedStr();
		void updateAmplitudeStr (float amplitude, char* buffer, unsigned int bufferLen);
		void updateFrequencyStr (float frequency, char* buffer, unsigned int bufferLen);
		void updateFiltFreqStr (float filtFrequency, char* buffer, unsigned int bufferLen);
		void updateOpModStr (unsigned int opNum, float opModAmount, char* buffer, unsigned int bufferLen, bool div = true);
		void updateAttackStr (float attackAmount, char* buffer, unsigned int bufferLen);
		void updateDecayStr (float decayAmount, char* buffer, unsigned int bufferLen);
		void updateSustainStr (float sustainAmount, char* buffer, unsigned int bufferLen);
		void updateReleaseStr (float releaseAmount, char* buffer, unsigned int bufferLen);
		void updateDetuneStr (int detuneAmount, char* buffer, unsigned int bufferLen);
		void updateAttackExpoStr (float attackExpoAmount, char* buffer, unsigned int bufferLen);
		void updateDecayExpoStr (float decayExpoAmount, char* buffer, unsigned int bufferLen);
		void updateReleaseExpoStr (float releaseExpoAmount, char* buffer, unsigned int bufferLen);
		void updateAmplitudeVelStr (float amplitudeVelAmount, char* buffer, unsigned int bufferLen);
		void updateFilterVelStr (float filterVelAmount, char* buffer, unsigned int bufferLen);
		void updateGlideStr (float glideAmount, char* buffer, unsigned int bufferLen);
		void updatePitchBendStr (unsigned int pitchBendAmount, char* buffer, unsigned int bufferLen);
		void updateFiltResStr (float filtResAmount, char* buffer, unsigned int bufferLen);

		void refreshEGDest();
		void refreshRatioFixed();
		void refreshMonoPoly();
		void refreshWave();
		void refreshGlideRetrig();

		// note: this truncates ungracefully if bufferLen is smaller than then needed
		void intToCString (int val, char* buffer, unsigned int bufferLen);

		void concatDigitStr (int val, char* sourceBuffer, char* destBuffer, unsigned int offset, unsigned int digitWidth,
					int decimalPlaceIndex = -1);
};

#endif // ARMOR8UIMANAGER_HPP
