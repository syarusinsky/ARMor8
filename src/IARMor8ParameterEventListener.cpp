#include "IARMor8ParameterEventListener.hpp"

// instantiating IARMor8ParameterEventListener's event dispatcher
EventDispatcher<IARMor8ParameterEventListener, ARMor8ParameterEvent,
		&IARMor8ParameterEventListener::onARMor8ParameterEvent> IARMor8ParameterEventListener::m_EventDispatcher;

ARMor8ParameterEvent::ARMor8ParameterEvent (const char* parameterStr, unsigned int strSize, unsigned int channel) :
	IEvent( channel ),
	m_ParamStr{ 0 },
	m_ParamStrSize( strSize )
{
	for ( unsigned int i = 0; i < strSize; i++ )
	{
		m_ParamStr[i] = parameterStr[i];
	}
}

ARMor8ParameterEvent::~ARMor8ParameterEvent()
{
}

const char* ARMor8ParameterEvent::getParameterStr() const
{
	return m_ParamStr;
}

unsigned int ARMor8ParameterEvent::getParameterStrSize() const
{
	return m_ParamStrSize;
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
