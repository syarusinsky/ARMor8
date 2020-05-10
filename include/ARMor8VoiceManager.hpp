#ifndef ARMOR8VOICEMANAGER_HPP
#define ARMOR8VOICEMANAGER_HPP

/****************************************************************************
 * The ARMor8VoiceManager is responsible for processing MIDI messages and
 * and setting the correct values for each ARMor8Voice. This class will
 * take input from the MIDI handler and other peripheral handlers.
****************************************************************************/

#include "ARMor8Voice.hpp"
#include "IBufferCallback.hpp"
#include "IMidiEventListener.hpp"
#include "IPitchEventListener.hpp"
#include "IPotEventListener.hpp"
#include "IButtonEventListener.hpp"

class MidiHandler;
class PresetManager;

enum class POT_CHANNEL : unsigned int
{
	ALL          = 0,
	FREQUENCY    = 1,
	DETUNE       = 2,
	ATTACK       = 3,
	ATTACK_EXPO  = 4,
	DECAY        = 5,
	DECAY_EXPO   = 6,
	SUSTAIN      = 7,
	RELEASE      = 8,
	RELEASE_EXPO = 9,
	OP1_MOD_AMT  = 10,
	OP2_MOD_AMT  = 11,
	OP3_MOD_AMT  = 12,
	OP4_MOD_AMT  = 13,
	AMPLITUDE    = 14,
	FILT_FREQ    = 15,
	FILT_RES     = 16,
	VEL_AMP      = 17,
	VEL_FILT     = 18,
	PITCH_BEND   = 19,
	GLIDE_TIME   = 20
};

enum class BUTTON_CHANNEL : unsigned int
{
	ALL          = 0,
	RATIO        = 1,
	OP1          = 2,
	OP2          = 3,
	OP3          = 4,
	OP4          = 5,
	SINE         = 6,
	TRIANGLE     = 7,
	SQUARE       = 8,
	SAWTOOTH     = 9,
	EG_AMP       = 10,
	EG_FREQ      = 11,
	EG_FILT      = 12,
	GLIDE_RETRIG = 13,
	MONOPHONIC   = 14,
	PREV_PRESET  = 15,
	NEXT_PRESET  = 16,
	WRITE_PRESET = 17
};

const unsigned int MAX_VOICES = 6;

class ARMor8VoiceManager : public IBufferCallback, public IKeyEventListener, public IPitchEventListener,
				public IPotEventListener, public IButtonEventListener
{
	public:
		// constant min/max values for UI
		const float FREQUENCY_MIN  = 1.0f;
		const float FREQUENCY_MAX  = 20000.0f;
		const int   DETUNE_MIN     = -1200;
		const int   DETUNE_MAX     = 1200;
		const float EXPO_MIN       = 0.1f;
		const float EXPO_MAX       = 100.0f;
		const float ATTACK_MIN     = 0.002f;
		const float ATTACK_MAX     = 2.0f;
		const float DECAY_MIN      = 0.0f;
		const float DECAY_MAX      = 2.0f;
		const float SUSTAIN_MIN    = 0.0f;
		const float SUSTAIN_MAX    = 1.0f;
		const float RELEASE_MIN    = 0.002f;
		const float RELEASE_MAX    = 3.0f;
		const float OP_MOD_MIN     = 0.0f;
		const float OP_MOD_MAX     = 20000.0f;
		const float AMPLITUDE_MIN  = 0.0f;
		const float AMPLITUDE_MAX  = 5.0f;
		const float FILT_FREQ_MIN  = 1.0f;
		const float FILT_FREQ_MAX  = 20000.0f;
		const float FILT_RES_MIN   = 0.0f;
		const float FILT_RES_MAX   = 3.5f;
		const float VELOCITY_MIN   = 0.0f;
		const float VELOCITY_MAX   = 1.0f;
		const int   PITCH_BEND_MIN = 1;
		const int   PITCH_BEND_MAX = 12;
		const float GLIDE_TIME_MIN = 0.0f;
		const float GLIDE_TIME_MAX = 1.0f;

		ARMor8VoiceManager (MidiHandler* midiHandler, PresetManager* presetManager);
		~ARMor8VoiceManager() override;

		void setOperatorToEdit (unsigned int opToEdit);
		unsigned int getOperatorToEdit();

		void setMonophonic (bool on);

		void setOperatorFreq (unsigned int opNum, float freq);
		void setOperatorDetune (unsigned int opNum, int cents);
		void setOperatorWave (unsigned int opNum, const OscillatorMode& wave);
		void setOperatorEG (unsigned int opNum, IEnvelopeGenerator* eg);
		void setOperatorEGAttack (unsigned int opNum, float seconds, float expo);
		void setOperatorEGDecay (unsigned int opNum, float seconds, float expo);
		void setOperatorEGSustain (unsigned int opNum, float lvl);
		void setOperatorEGRelease (unsigned int opNum, float seconds, float expo);
		void setOperatorEGModDestination (unsigned int opNum, const EGModDestination& modDest, const bool on);
		void setOperatorModulation (unsigned int sourceOpNum, unsigned int destOpNum, float modulationAmount);
		void setOperatorAmplitude (unsigned int opNum, float amplitude);
		void setOperatorFilterFreq (unsigned int opNum, float frequency);
		void setOperatorFilterRes (unsigned int opNum, float resonance);
		void setOperatorRatio (unsigned int opNum, bool useRatio);
		void setOperatorAmpVelSens (unsigned int opNum, float ampVelSens);
		void setOperatorFiltVelSens (unsigned int opNum, float filtVelSens);

		void setGlideTime (const float glideTime);
		void setGlideRetrigger (const bool useRetrigger);
		void setUseGlide (const bool useGlide);

		void setPitchBendSemitones (const unsigned int pitchBendSemitones);
		unsigned int getPitchBendSemitones() { return m_PitchBendSemitones; }

		ARMor8VoiceState getState();
		void setState(const ARMor8VoiceState& state);

		ARMor8PresetHeader getPresetHeader();

		void call (float* writeBuffer) override;

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void onPitchEvent (const PitchEvent& pitchEvent) override;

		void onPotEvent (const PotEvent& potEvent) override;

		void onButtonEvent (const ButtonEvent& buttonEvent) override;

	private:
		MidiHandler*   m_MidiHandler;
		PresetManager* m_PresetManager;
		unsigned int   m_OpToEdit;
		bool           m_Monophonic;
		ARMor8Voice    m_Voice1;
		ARMor8Voice    m_Voice2;
		ARMor8Voice    m_Voice3;
		ARMor8Voice    m_Voice4;
		ARMor8Voice    m_Voice5;
		ARMor8Voice    m_Voice6;
		ARMor8Voice*   m_Voices[MAX_VOICES];

		KeyEvent m_ActiveKeyEvents[MAX_VOICES];
		unsigned int m_ActiveKeyEventIndex;

		unsigned int m_PitchBendSemitones;

		ARMor8PresetHeader m_PresetHeader;
};

#endif // ARMOR8VOICEMANAGER_HPP
