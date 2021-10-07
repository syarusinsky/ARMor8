#ifndef MUTEX_HPP
#define MUTEX_HPP

/**************************************************************************
 * A Mutex is a lightweight mutex for use in multi-core/multi-threaded
 * applications. When running on ARM Cortex M3/M4/M7 platforms, this is
 * implemented by exclusive instructions with data memory barriers. In the
 * case of running on host, this class is simply a wrapper for the stl
 * mutex class.
**************************************************************************/

#ifndef TARGET_BUILD

#include <mutex>

class Mutex
{
	public:
		Mutex (unsigned int semNum) : m_Mutex() {}
		~Mutex() {}

		bool tryLock() { return m_Mutex.try_lock(); }
		void unlock() { m_Mutex.unlock(); }

	private:
		std::mutex 	m_Mutex;
};

#else

#include <stdint.h>

#include "LLPD.hpp"

class Mutex
{
	public:
		Mutex (unsigned int semNum) : m_SemaphoreNumber( semNum ) {}
		~Mutex() {}

		bool tryLock() { return LLPD::hsem_try_take( m_SemaphoreNumber ); }

		void unlock() { LLPD::hsem_release( m_SemaphoreNumber ); }

	private:
		unsigned int m_SemaphoreNumber;
};

#endif // #ifndef TARGET_BUILD

#endif // MUTEX_HPP
