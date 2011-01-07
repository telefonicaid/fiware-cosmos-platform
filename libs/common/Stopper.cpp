#include "Stopper.h"				// Own interface
#include <sys/time.h>            // struct timeval

namespace au
{
	Stopper::Stopper()
	{
		num_threads_stopped = 0;
		pthread_mutex_init(&_lock, 0);
		pthread_cond_init(&_condition, NULL);
	}
	
	Stopper::~Stopper()
	{
		pthread_mutex_destroy(&_lock);
		pthread_cond_destroy(&_condition);
	}
	
	void Stopper::stop(  )
	{
		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		assert(!ans); // We do not accept falling simple mutex

		// This unlock the mutex and froze the process in the condition
		pthread_cond_wait(&_condition, &_lock);
		
		// UNLOCK the mutex
		ans = pthread_mutex_unlock(&_lock);
		assert(!ans); // We do not accept falling simple mutex
		
	}

	void Stopper::stop( int max_seconds  )
	{
		
		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		assert(!ans); // We do not accept falling simple mutex

        struct timeval tv;
        struct timespec ts;
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + max_seconds;
        ts.tv_nsec = 0;
		
		
		// This unlock the mutex and froze the process in the condition
		pthread_cond_timedwait(&_condition, &_lock , &ts);
		
		// UNLOCK the mutex
		ans = pthread_mutex_unlock(&_lock);
		assert(!ans); // We do not accept falling simple mutex
		
	}
	
	void Stopper::wakeUp(  )
	{
		int ans;
		
		ans = pthread_mutex_lock(&_lock);
		assert(!ans); // We do not accept falling simple mutex

		ans = pthread_cond_signal(&_condition);
		assert(!ans); // We do not accept falling simple mutex

		ans = pthread_mutex_unlock(&_lock);
		assert(!ans); // We do not accept falling simple mutex

	}
	
	void Stopper::wakeUpAll(  )
	{
		pthread_mutex_lock(&_lock);
		pthread_cond_broadcast(&_condition);
		pthread_mutex_unlock(&_lock);
	}	
	
}