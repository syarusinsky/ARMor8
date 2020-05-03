#ifndef ARMOR8FILTER_HPP
#define ARMOR8FILTER_HPP

/****************************************************************
 * An ARMor8Filter is four OnePoleFilters in series, with the
 * capability of resonance with its output soft-clipped. Each
 * ARMor8Voice has an ARMor8Filter at it's final output stage.
****************************************************************/

#include "OnePoleFilter.hpp"

class ARMor8Filter : public IFilter
{
	public:
		ARMor8Filter();
		~ARMor8Filter() override;

		float processSample (float sample) override;
		void setCoefficients (float frequency) override;

		void setResonance (float resonance) override;
		float getResonance() override;

	private:
		OnePoleFilter filter1;
		OnePoleFilter filter2;
		OnePoleFilter filter3;
		OnePoleFilter filter4;
		float m_Resonance;
		float m_PrevSample;
};

#endif // ARMOR8FILTER_HPP
