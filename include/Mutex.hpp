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
		Mutex() : m_Mutex() {}
		~Mutex() {}

		bool tryLock() { return m_Mutex.try_lock(); }
		void unlock() { m_Mutex.unlock(); }

	private:
		std::mutex 	m_Mutex;
};

#else

#include <stdint.h>

#include "LLPD.hpp"

#define SEMAPHORE_NUM 1

class Mutex
{
	public:
		Mutex() {}
		~Mutex() {}

		bool tryLock() { return LLPD::hsem_try_take( SEMAPHORE_NUM ); }

		void unlock() { LLPD::hsem_release( SEMAPHORE_NUM ); }
};

#endif // #ifndef TARGET_BUILD

#endif // MUTEX_HPP
