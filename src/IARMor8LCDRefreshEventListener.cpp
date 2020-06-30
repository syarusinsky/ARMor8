#include "IARMor8LCDRefreshEventListener.hpp"

// instantiating IARMor8LCDRefreshEventListener's event dispatcher
EventDispatcher<IARMor8LCDRefreshEventListener, ARMor8LCDRefreshEvent,
		&IARMor8LCDRefreshEventListener::onARMor8LCDRefreshEvent> IARMor8LCDRefreshEventListener::m_EventDispatcher;

ARMor8LCDRefreshEvent::ARMor8LCDRefreshEvent (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd,
						unsigned int channel) :
	IEvent( channel ),
	m_XStart( xStart ),
	m_YStart( yStart ),
	m_XEnd( xEnd ),
	m_YEnd( yEnd )
{
}

ARMor8LCDRefreshEvent::~ARMor8LCDRefreshEvent()
{
}

unsigned int ARMor8LCDRefreshEvent::getXStart() const
{
	return m_XStart;
}

unsigned int ARMor8LCDRefreshEvent::getYStart() const
{
	return m_YStart;
}

unsigned int ARMor8LCDRefreshEvent::getXEnd() const
{
	return m_XEnd;
}

unsigned int ARMor8LCDRefreshEvent::getYEnd() const
{
	return m_YEnd;
}

IARMor8LCDRefreshEventListener::~IARMor8LCDRefreshEventListener()
{
	this->unbindFromARMor8LCDRefreshEventSystem();
}

void IARMor8LCDRefreshEventListener::bindToARMor8LCDRefreshEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IARMor8LCDRefreshEventListener::unbindFromARMor8LCDRefreshEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IARMor8LCDRefreshEventListener::PublishEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent)
{
	m_EventDispatcher.dispatch( lcdRefreshEvent );
}
