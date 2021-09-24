#include "IARMor8PresetEventListener.hpp"

// instantiating IARMor8PresetEventListener's event dispatcher
EventDispatcher<IARMor8PresetEventListener, ARMor8PresetEvent,
		&IARMor8PresetEventListener::onARMor8PresetChangedEvent> IARMor8PresetEventListener::m_EventDispatcher;

ARMor8PresetEvent::ARMor8PresetEvent (const ARMor8VoiceState& preset, unsigned int presetNum, unsigned int channel) :
	IEvent( channel ),
	m_Preset( preset ),
	m_PresetNum( presetNum )
{
}

ARMor8PresetEvent::~ARMor8PresetEvent()
{
}

IARMor8PresetEventListener::~IARMor8PresetEventListener()
{
	this->unbindFromARMor8PresetEventSystem();
}

void IARMor8PresetEventListener::bindToARMor8PresetEventSystem()
{
	m_EventDispatcher.bind( this );
}

void IARMor8PresetEventListener::unbindFromARMor8PresetEventSystem()
{
	m_EventDispatcher.unbind( this );
}

void IARMor8PresetEventListener::PublishEvent (const ARMor8PresetEvent& preset)
{
	m_EventDispatcher.dispatch( preset );
}
