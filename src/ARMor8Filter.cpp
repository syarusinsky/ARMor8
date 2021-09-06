#include "ARMor8Filter.hpp"

ARMor8Filter::ARMor8Filter() :
	m_Filter1(),
	m_Filter2(),
	m_Filter3(),
	m_Filter4(),
	m_Resonance( 0.0f ),
	m_PrevSample( 0.0f )
{
	this->setCoefficients( 20000.0f );
}

ARMor8Filter::~ARMor8Filter()
{
}

float ARMor8Filter::processSample (float sample)
{
	m_PrevSample = ( m_PrevSample * -m_Resonance ) + sample;
	float out1 = m_Filter1.processSample( m_PrevSample );
	float out2 = m_Filter2.processSample( out1 );
	float out3 = m_Filter3.processSample( out2 );
	float out4 = m_Filter4.processSample( out3 );
	m_PrevSample = out4;

	// soft clipping
	if ( m_PrevSample > 1.0f )
	{
		m_PrevSample = 1.0f;
	}
	if ( m_PrevSample < -1.0f )
	{
		m_PrevSample = -1.0f;
	}
	m_PrevSample = ( 1.5f * m_PrevSample ) - ( 0.5f * m_PrevSample * m_PrevSample * m_PrevSample );

	return m_PrevSample;
}

void ARMor8Filter::setCoefficients (float frequency)
{
	m_Filter1.setCoefficients( frequency );
	m_Filter2.setCoefficients( frequency );
	m_Filter3.setCoefficients( frequency );
	m_Filter4.setCoefficients( frequency );
}

void ARMor8Filter::setResonance (float resonance)
{
	m_Resonance = resonance;
}

float ARMor8Filter::getResonance()
{
	return m_Resonance;
}
