#include "ARMor8UiManager.hpp"

#include "Graphics.hpp"

ARMor8UiManager::ARMor8UiManager (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	Surface( width, height, format ),
	m_Logo( nullptr )
{
}

ARMor8UiManager::~ARMor8UiManager()
{
}

void ARMor8UiManager::setFont (Font* font)
{
	m_Graphics->setFont( font );
}

void ARMor8UiManager::setLogo (Sprite* logo)
{
	m_Logo = logo;
}

void ARMor8UiManager::draw()
{
	m_Graphics->setColor( false );
	m_Graphics->fill();

	m_Graphics->setColor( true );
	m_Graphics->drawText( 0.2f, 0.45f, "Test string!", 1.0f );

	m_Graphics->drawLine( 0.1f, 0.9f, 0.8f, 0.95f );

	m_Graphics->drawSprite( 0.0f, 0.0f, *m_Logo );
}
