#ifndef ARMOR8OPERATOR_HPP
#define ARMOR8OPERATOR_HPP

/*******************************************************************************
 * An ARMor8Operator describes an oscillator who's frequency can be modulated
 * by another ARMor8Operator. It also has an ADSREnvelopeGenerator which can
 * provide additional modulation to amplitude and frequency.
*******************************************************************************/

#include <set>
#include <map>

#include "ARMor8Filter.hpp"
#include "IKeyEventListener.hpp"
#include "IPitchEventListener.hpp"
#include "ExponentialResponse.hpp"
#include "ADSREnvelopeGenerator.hpp"
#include "ARMor8Constants.hpp"

#define EG_RESPONSE ExponentialResponse

class PolyBLEPOsc;
enum class OscillatorMode : unsigned int;

enum class EGModDestination : unsigned int
{
	AMPLITUDE,
	FREQUENCY,
	FILT_FREQUENCY
};

class ARMor8Operator : public IKeyEventListener, public IPitchEventListener, public IBufferCallback<float>
{
	public:
		ARMor8Operator (PolyBLEPOsc& osc, ADSREnvelopeGenerator<EG_RESPONSE>& eg, ARMor8Filter& filt, float amplitude,
				float frequency, ARMor8Operator& mod1, ARMor8Operator& mod2, ARMor8Operator& mod3, ARMor8Operator& mod4);
		~ARMor8Operator() override;

		void cachePerBlockValues(); // this should be called once per call(), since it caches eg val, freq val, filt freq, ect

		float nextSample();
		float currentValue();

		void call (float* writeBuffer) override;

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void onPitchEvent (const PitchEvent& pitchEvent) override;

		OscillatorMode getWave();
		void setWave (const OscillatorMode& wave);
		ADSREnvelopeGenerator<EG_RESPONSE>& getEnvelopeGenerator();
		void setModSourceAmplitude (ARMor8Operator* modSource, float amplitude = 1.0f);
		void setEGModDestination (const EGModDestination& modDest, const bool on);
		void setFrequency (const float frequency);
		void setDetune (const int cents);
		void setEGFreqModAmount (const float modAmount);
		void setAmplitude (const float amplitude);
		void setFilterFreq (const float frequency);
		void setFilterRes (const float resonance);
		void setRatio (const bool useRatio);
		void setAmpVelSens (const float ampVelSens);
		void setFiltVelSens (const float filtVelSens);
		void setFrequencyOffset (const float freqOffset);
		void setGlideTime (const float glideTime);
		void setGlideRetrigger (const bool useRetrigger);
		void setUseGlide (const bool useGlide);

		float getFrequency() { return m_Frequency; }
		int getDetune() { return m_Detune; }
		bool egModAmplitudeSet() { if (m_UseAmplitudeMod) { return true; } return false; }
		bool egModFrequencySet() { if (m_UseFrequencyMod) { return true; } return false; }
		bool egModFilterSet(){ if (m_UseFiltFreqMod){ return true; } return false; }
		float getModulationAmount (unsigned int opNum)
		{
			return m_ModOperatorAmplitudes[opNum];
		}
		float getAmplitude() { return m_Amplitude; }
		float getFilterFreq() { return m_FilterCenterFreq; }
		float getFilterRes() { return m_Filter.getResonance(); }
		bool getRatio() { return m_UseRatio; }
		float getAmpVelSens() { return m_AmpVelSens; }
		float getFiltVelSens() { return m_FiltVelSens; }
		float getRatioFrequency() { return m_RatioFrequency; }
		float getGlideTime() { return m_GlideTime; }
		bool getGlideRetrigger() { return m_UseGlideRetrigger; }
		bool getUseGlide() { return m_UseGlide; }

	private:
		PolyBLEPOsc& 				m_Osc;
		ADSREnvelopeGenerator<EG_RESPONSE>& 	m_EG;
		ARMor8Filter& 				m_Filter;
		ARMor8Operator& 			m_ModOperator1;
		ARMor8Operator& 			m_ModOperator2;
		ARMor8Operator& 			m_ModOperator3;
		ARMor8Operator& 			m_ModOperator4;
		float 					m_ModOperatorAmplitudes[ARMOR8_NUM_OPERATORS_PER_VOICE];
		float 					m_FilterCenterFreq;
		bool 					m_UseRatio;
		bool 					m_UseAmplitudeMod;
		bool 					m_UseFrequencyMod;
		bool 					m_UseFiltFreqMod;
		float         m_Amplitude;
		float         m_AmplitudeCached;
		float         m_Frequency;
		float         m_FrequencyCached;
		int           m_Detune;
		float         m_DetuneCached;
		float         m_Ratio;
		float         m_RatioFrequency;
		float         m_CurrentValue;
		float         m_AmpVelSens;
		float         m_FiltVelSens;
		float         m_CurrentVelocity;
		float         m_FrequencyOffset;
		float         m_GlideFrequency;
		float         m_GlideTime;
		unsigned int  m_GlideNumSamples;
		float         m_GlideIncr;
		bool          m_UseGlideRetrigger;
		bool          m_UseGlide;
};

#endif // ARMOR8OPERATOR_HPP
