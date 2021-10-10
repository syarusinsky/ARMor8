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
		OnePoleFilter<float> 	m_Filter1;
		OnePoleFilter<float> 	m_Filter2;
		OnePoleFilter<float> 	m_Filter3;
		OnePoleFilter<float> 	m_Filter4;
		float 			m_Resonance;
		float 			m_PrevSample;
};

#endif // ARMOR8FILTER_HPP
