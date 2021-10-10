#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

/**************************************************************************
 * An EventQueue represents a FIFO message queue of a given size that
 * types can be written to and read from. It is thread-safe, and therefore
 * can be used for IPC.
 *
 * Note: The startAddress is the beginning of the FIFO used to read and
 * write messages and the sizeInBytes is the size in bytes of the total
 * FIFO buffer.
**************************************************************************/

#include <stdint.h>

#include "IEventListener.hpp"
#include "Mutex.hpp"

#ifndef TARGET_BUILD
#include <iostream>
#endif // ifndef TARGET_BUILD

template <typename T>
class EventQueue
{
	public:
		EventQueue (uint8_t* startAddress, unsigned int sizeInBytes, unsigned int semNum = 0) :
			m_StartAddress( startAddress ),
			m_SizeInBytes( sizeInBytes ),
			m_SizeOfEvent( sizeof(T) ),
			m_MaxFifoEntries( m_SizeInBytes / m_SizeOfEvent ),
			m_ReadIndex( 0 ),
			m_WriteIndex( 0 ),
			m_NumUnread( 0 ),
			m_Mutex( semNum )
		{
		}

		~EventQueue() {}

		bool writeEvent (const T& event)
		{
			if ( m_Mutex.tryLock() )
			{
				if ( m_NumUnread < m_MaxFifoEntries - 1 )
				{
					new ( const_cast<uint8_t*>(&m_StartAddress[m_WriteIndex * m_SizeOfEvent]) ) T( event );

					m_WriteIndex = ( m_WriteIndex + 1 ) % m_MaxFifoEntries;

					m_NumUnread++;

					m_Mutex.unlock();

					return true;
				}

				m_Mutex.unlock();
			}

			return false;
		}

		bool readEvent (T& event)
		{
			if ( m_Mutex.tryLock() )
			{
				if ( m_ReadIndex != m_WriteIndex )
				{
					T* readEvent = reinterpret_cast<T*>( const_cast<uint8_t*>(&m_StartAddress[m_ReadIndex * m_SizeOfEvent]) );

					event = *readEvent;

					m_ReadIndex = ( m_ReadIndex + 1 ) % m_MaxFifoEntries;

					m_NumUnread--;

					m_Mutex.unlock();

					return true;
				}

				m_Mutex.unlock();

				return false;
			}

			return false;
		}

	private:
		const uint8_t* 		m_StartAddress;
		const unsigned int 	m_SizeInBytes;
		const unsigned int 	m_SizeOfEvent;
		const unsigned int 	m_MaxFifoEntries;

		unsigned int 		m_ReadIndex;
		unsigned int 		m_WriteIndex;

		unsigned int 		m_NumUnread;

		Mutex 			m_Mutex;
};

#ifndef TARGET_BUILD
// void messageQueueTester();
#endif // ifndef TARGET_BUILD

#endif // EVENTQUEUE_HPP
