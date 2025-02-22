#ifndef ARMOR8VOICEMANAGER_HPP
#define ARMOR8VOICEMANAGER_HPP

/****************************************************************************
 * The ARMor8VoiceManager is responsible for processing MIDI messages and
 * and setting the correct values for each ARMor8Voice. This class will
 * take input from the MIDI handler and other peripheral handlers.
****************************************************************************/

#include "ARMor8Voice.hpp"
#include "ARMor8Constants.hpp"
#include "IARMor8ParameterEventListener.hpp"
#include "Limiter.hpp"
#include <cstdint>

class MidiHandler;
class PresetManager;

constexpr unsigned int MAX_VOICES = 6;

class ARMor8VoiceManager : public IBufferCallback<float>, public IKeyEventListener, public IPitchEventListener,
				public IARMor8ParameterEventListener
{
	public:
		ARMor8VoiceManager (MidiHandler* midiHandler, PresetManager* presetManager, uint16_t* dmaBufferCurrent = nullptr);
		~ARMor8VoiceManager() override;

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
		void loadCurrentPreset();

		ARMor8PresetHeader getPresetHeader();

		void call (float* writeBuffer) override;

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void onPitchEvent (const PitchEvent& pitchEvent) override;

		void onARMor8ParameterEvent (const ARMor8ParameterEvent& paramEvent) override;

		void setCurrentDmaBuffer (uint16_t* dmaBuffer);

	private:
		MidiHandler*   m_MidiHandler;
		PresetManager* m_PresetManager;
		bool           m_Monophonic;
		ARMor8Voice    m_Voice1;
		ARMor8Voice    m_Voice2;
		ARMor8Voice    m_Voice3;
		ARMor8Voice    m_Voice4;
		ARMor8Voice    m_Voice5;
		ARMor8Voice    m_Voice6;

		unsigned int   m_Pot1AssignmentIndex;
		unsigned int   m_Pot1AssignmentOp;
		unsigned int   m_Pot2AssignmentIndex;
		unsigned int   m_Pot2AssignmentOp;
		unsigned int   m_Pot3AssignmentIndex;
		unsigned int   m_Pot3AssignmentOp;

		KeyEvent                m_ActiveKeyEvents[MAX_VOICES];
		unsigned int            m_ActiveKeyEventIndex;

		unsigned int            m_PitchBendSemitones;

		ARMor8PresetHeader      m_PresetHeader;

		Limiter<float>          m_Limiter;

		uint16_t* 		m_DMABufferCurrent;
};

#endif // ARMOR8VOICEMANAGER_HPP
