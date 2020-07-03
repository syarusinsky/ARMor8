#include "IARMor8ParameterEventListener.hpp"

// instantiating IARMor8ParameterEventListener's event dispatcher
EventDispatcher<IARMor8ParameterEventListener, ARMor8ParameterEvent,
		&IARMor8ParameterEventListener::onARMor8ParameterEvent> IARMor8ParameterEventListener::m_EventDispatcher;

ARMor8ParameterEvent::ARMor8ParameterEvent (float value, unsigned int channel) :
	IEvent( channel ),
	m_Value{ value }
{
}

ARMor8ParameterEvent::~ARMor8ParameterEvent()
{
}

float ARMor8ParameterEvent::getValue() const
{
	return m_Value;
}

IARMor8ParameterEventListener::~IARMor8ParameterEventListener()
{
	this->unbindFromARMor8ParameterEventSystem();
}

void IARMor8ParameterEventListener::bindToARMor8ParameterEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IARMor8ParameterEventListener::unbindFromARMor8ParameterEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IARMor8ParameterEventListener::PublishEvent (const ARMor8ParameterEvent& paramEvent)
{
	m_EventDispatcher.dispatch( paramEvent );
}

