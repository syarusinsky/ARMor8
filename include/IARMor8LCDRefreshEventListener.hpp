#ifndef IARMOR8LCDREFRESHEVENTLISTENER_HPP
#define IARMOR8LCDREFRESHEVENTLISTENER_HPP

/*******************************************************************
 * An IARMor8LCDRefreshEventListener specifies a simple interface
 * which a subclass can use to be notified of events informing
 * the listener that the LCD (or LCD representation) needs to be
 * refreshed. The events also include the 'dirty' rectangle, in
 * case the only a portion of the lcd needs to be refreshed.
*******************************************************************/

#include "IEventListener.hpp"

class ARMor8LCDRefreshEvent : public IEvent
{
	public:
		ARMor8LCDRefreshEvent (unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd,
					unsigned int channel);
		~ARMor8LCDRefreshEvent() override;

		unsigned int getXStart() const;
		unsigned int getYStart() const;
		unsigned int getXEnd() const;
		unsigned int getYEnd() const;

	private:
		unsigned int m_XStart;
		unsigned int m_YStart;
		unsigned int m_XEnd;
		unsigned int m_YEnd;
};

class IARMor8LCDRefreshEventListener : public IEventListener
{
	public:
		virtual ~IARMor8LCDRefreshEventListener();

		virtual void onARMor8LCDRefreshEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent) = 0;

		void bindToARMor8LCDRefreshEventSystem();
		void unbindFromARMor8LCDRefreshEventSystem();

		static void PublishEvent (const ARMor8LCDRefreshEvent& lcdRefreshEvent);

	private:
		static EventDispatcher<IARMor8LCDRefreshEventListener, ARMor8LCDRefreshEvent,
					&IARMor8LCDRefreshEventListener::onARMor8LCDRefreshEvent> m_EventDispatcher;
};

#endif // IARMOR8LCDREFRESHEVENTLISTENER_HPP
