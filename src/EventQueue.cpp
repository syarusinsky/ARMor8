#include "EventQueue.hpp"

/*
#ifndef TARGET_BUILD

// TODO delete all this after testing
#include <thread>

#define NUM_MESSAGES 1000

class MessageEvent : public IEvent
{
	public:
		MessageEvent (const std::string& message, unsigned int channel) : IEvent( channel ), m_Message( message ) {}
		~MessageEvent() override {}

		std::string getMessage() { return m_Message; }

	private:
		std::string 	m_Message;
};

void write_message_thread (EventQueue<MessageEvent>* eventQueue)
{
	for ( unsigned int incr = 0; incr < NUM_MESSAGES; )
	{
		MessageEvent message( "This is message number " + std::to_string(incr), 0 );
		if ( eventQueue->writeEvent(message) )
		{
			incr++;
		}
	}

	std::cout << "END OF WRITING THREAD" << std::endl;
}

void read_message_thread (EventQueue<MessageEvent>* eventQueue)
{
	for ( unsigned int incr = 0; incr < NUM_MESSAGES; )
	{
		MessageEvent message( "temp", 0 );
		bool readCorrectly = eventQueue->readEvent( message );
		if ( readCorrectly )
		{
			std::cout << message.getMessage() << std::endl;
			incr++;
		}
	}

	std::cout << "END OF READING THREAD" << std::endl;
}

void messageQueueTester()
{
	uint8_t memoryRegion[sizeof(EventQueue<MessageEvent>) + (sizeof(MessageEvent) * 50)] = { 0 };

	EventQueue<MessageEvent>* eventQueue = new ( memoryRegion ) EventQueue<MessageEvent>( memoryRegion + sizeof(EventQueue<MessageEvent>), sizeof(memoryRegion) - sizeof(EventQueue<MessageEvent>) );

	std::thread writeThread( write_message_thread, eventQueue );
	std::thread readThread( read_message_thread, eventQueue );

	writeThread.join();
	readThread.join();
}

#endif // ifndef TARGET_BUILD

*/
