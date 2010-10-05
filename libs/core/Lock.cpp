/* ****************************************************************************
*
* FILE                 Lock.cpp - 
*
* AUTHOR               Ken Zangelin, Andreu Urruela
*
* CREATION DATE        Oct 5 2010
*
*/
#include <sys/time.h>            /* struct timeval                  */
#include <pthread.h>             /* pthread_mutex_init, ...         */

#include "samsonLogMsg.h"        /* LOG_ERROR, ...                  */
#include "StopLock.h"            /* StopLock                        */
#include "Lock.h"                /* Own interface                   */



namespace au
{
	Lock::Lock()
	{
		pthread_mutex_init(&_lock, 0);
	}
	
	Lock::~Lock()
	{
		pthread_mutex_destroy(&_lock);
	}
	
	void Lock::lock()
	{
#ifdef DEBUG_SS_THREADS
		LockDebugger::shared()->add_lock( this );
#endif
		int ans = pthread_mutex_lock(&_lock);	// Block until the mutex is free

		if (ans != 0)
		{
			LOG_ERROR(("pthread_mutex_lock error"));
			assert(ans == 0);
		}
	}
	
	void Lock::unlock()
	{
		
#ifdef DEBUG_SS_THREADS		
		LockDebugger::shared()->remove_lock( this );
#endif		
		pthread_mutex_unlock(&_lock);
	}
	
	
	void Lock::unlock_waiting_in_stopLock( StopLock *stopLock )
	{
#ifdef DEBUG_SS_THREADS		
		LockDebugger::shared()->remove_lock( this );		
#endif
		
		pthread_cond_wait(&stopLock->condition, &_lock);	// This unlock the mutex and froze the process in the condition
		pthread_mutex_unlock(&_lock);						// This unocks the mutes because it has gained again
		
	}
	
	void Lock::unlock_waiting_in_stopLock( StopLock *stopLock , int max_seconds )
	{
#ifdef DEBUG_SS_THREADS		
		LockDebugger::shared()->remove_lock( this );		
#endif
		
        struct timeval tv;
        struct timespec ts;
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + max_seconds;
        ts.tv_nsec = 0;
		
		pthread_cond_timedwait(&stopLock->condition, &_lock, &ts);
		pthread_mutex_unlock(&_lock);						// This unocks the mutes because it has gained again
		
	}
	
	
	void Lock::wakeUpStopLock( StopLock *stopLock )
	{
		pthread_mutex_lock(&_lock);
		pthread_cond_signal(&stopLock->condition);
		pthread_mutex_unlock(&_lock);
	}
	
	void Lock::wakeUpAllStopLock( StopLock *stopLock )
	{
		pthread_mutex_lock(&_lock);
		pthread_cond_broadcast(&stopLock->condition);
		pthread_mutex_unlock(&_lock);
	}
}
