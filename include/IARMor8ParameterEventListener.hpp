#ifndef IARMOR8PARAMETEREVENTLISTENER_HPP
#define IARMOR8PARAMETEREVENTLISTENER_HPP

/*******************************************************************
 * An IARMor8ParameterEventListener specifies a simple interface
 * which a subclass can use to be notified of ARMor8 parameter
 * change events. The parameter event only passes a string
 * representation of the parameter value, since it's only used
 * for UI purposes.
*******************************************************************/

#include "IEventListener.hpp"

const unsigned int MAX_PARAMETER_EVENT_STRING_SIZE = 5;

class ARMor8ParameterEvent : public IEvent
{
	public:
		ARMor8ParameterEvent (float value, unsigned int channel);
		~ARMor8ParameterEvent() override;

		float getValue() const;

	private:
		float m_Value;
};

class IARMor8ParameterEventListener : public IEventListener
{
	public:
		virtual ~IARMor8ParameterEventListener();

		virtual void onARMor8ParameterEvent (const ARMor8ParameterEvent& paramEvent) = 0;

		void bindToARMor8ParameterEventSystem();
		void unbindFromARMor8ParameterEventSystem();

		static void PublishEvent (const ARMor8ParameterEvent& paramEvent);

	private:
		static EventDispatcher<IARMor8ParameterEventListener, ARMor8ParameterEvent,
					&IARMor8ParameterEventListener::onARMor8ParameterEvent> m_EventDispatcher;
};

#endif // IARMOR8PARAMETEREVENTLISTENER_HPP
