#include "ARMor8Filter.hpp"

#include "AudioConstants.hpp"
#include <math.h>
#include <algorithm>

ARMor8Filter::ARMor8Filter() :
#ifdef OLD_FILTER
	m_Filter1(),
	m_Filter2(),
	m_Filter3(),
	m_Filter4(),
	m_PrevSample( 0.0f ),
#else
	m_A0( 0.0f ),
	m_B1( 0.0f ),
	m_B2( 0.0f ),
	m_B3( 0.0f ),
	m_B4( 0.0f ),
	m_PrevSample0( 0.0f ),
	m_PrevSample1( 0.0f ),
	m_PrevSample2( 0.0f ),
	m_PrevSample3( 0.0f ),
	m_PrevSample4( 0.0f ),
#endif // OLD_FILTER
	m_Resonance( 0.0f )
{
	this->setCoefficients( 20000.0f );
}

ARMor8Filter::~ARMor8Filter()
{
}

float ARMor8Filter::processSample (float sample)
{
#ifdef OLD_FILTER
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
#else
	m_PrevSample0 = ( m_PrevSample0 * -m_Resonance ) + sample;
	const float fOut = ( m_A0 * m_PrevSample0 )
			+ ( m_B1 * m_PrevSample1 )
			+ ( m_B2 * m_PrevSample2 )
			+ ( m_B3 * m_PrevSample3 )
			+ ( m_B4 * m_PrevSample4 );
	m_PrevSample4 = m_PrevSample3;
	m_PrevSample3 = m_PrevSample2;
	m_PrevSample2 = m_PrevSample1;
	m_PrevSample1 = fOut;
	m_PrevSample0 = fOut;

	// soft clipping
	if ( m_PrevSample0 > 1.0f )
	{
		m_PrevSample0 = 1.0f;
	}
	else if ( m_PrevSample0 < -1.0f )
	{
		m_PrevSample0 = -1.0f;
	}
	m_PrevSample0 = ( 1.5f * m_PrevSample0 ) - ( 0.5f * m_PrevSample0 * m_PrevSample0 * m_PrevSample0 );

	return m_PrevSample0;
#endif // OLD_FILTER
}

void ARMor8Filter::setCoefficients (float frequency)
{
#ifdef OLD_FILTER
	m_Filter1.setCoefficients( frequency );
	m_Filter2.setCoefficients( frequency );
	m_Filter3.setCoefficients( frequency );
	m_Filter4.setCoefficients( frequency );
#else
	// do to rounding error, any lower will cause instability
	frequency = std::max( frequency, 70.0f );
	constexpr float sampleRateDiv2 = SAMPLE_RATE / 2.0f;
	const float x = expf( -14.445f * (frequency / sampleRateDiv2) );
	const float a = 1.0f - x;
	m_A0 = a * a * a * a;
	m_B1 = x * 4.0f;
	const float xSquared = x * x;
	m_B2 = -6.0f * xSquared;
	const float xCubed = xSquared * x;
	m_B3 = 4.0f * xCubed;
	m_B4 = -1.0f * ( xCubed * x );
#endif // OLD_FILTER
}

void ARMor8Filter::setResonance (float resonance)
{
	m_Resonance = resonance;
}

float ARMor8Filter::getResonance()
{
	return m_Resonance;
}

void ARMor8Filter::call (float* writeBuffer)
{
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = this->processSample( writeBuffer[sample] );
	}
}
