#ifndef ARMOR8FILTER_HPP
#define ARMOR8FILTER_HPP

/****************************************************************
 * An ARMor8Filter is four OnePoleFilters in series, with the
 * capability of resonance with its output soft-clipped. Each
 * ARMor8Voice has an ARMor8Filter at it's final output stage.
****************************************************************/

#include "OnePoleFilter.hpp"
#include "IBufferCallback.hpp"

class ARMor8Filter : public IFilter<float>, public IBufferCallback<float>
{
	public:
		ARMor8Filter();
		~ARMor8Filter() override;

		float processSample (float sample) override;
		void setCoefficients (float frequency) override;

		void setResonance (float resonance) override;
		float getResonance() override;

		void call (float* writeBuffer) override;

	private:
#ifdef OLD_FILTER
		OnePoleFilter<float> 	m_Filter1;
		OnePoleFilter<float> 	m_Filter2;
		OnePoleFilter<float> 	m_Filter3;
		OnePoleFilter<float> 	m_Filter4;
		float 			m_PrevSample;
#else
		float 			m_A0;
		float 			m_B1;
		float 			m_B2;
		float 			m_B3;
		float 			m_B4;
		float 			m_PrevSample0;
		float 			m_PrevSample1;
		float 			m_PrevSample2;
		float 			m_PrevSample3;
		float 			m_PrevSample4;
#endif // OLD_FILTER
		float 			m_Resonance;
};

#endif // ARMOR8FILTER_HPP
