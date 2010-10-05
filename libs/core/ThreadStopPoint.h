#ifndef SAMSON_THREAD_STOP_POINT_H
#define SAMSON_THREAD_STOP_POINT_H

#include <assert.h>
#include <sys/time.h>
#include <set>
#include <map>
#include <iostream>



namespace au {

 /**
 ThreadStopPoint is the classical mechanism to stop some threads to wait for something.
 On the other hand there is a thread-safe function to wake up all elements
 */

class ThreadStopPoint
{
	pthread_cond_t  condition;
	pthread_mutex_t mutex;

public:
	
	void stop()
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition, &mutex);	// This unlock the mutex and froze the process in the condition
		
		// When returning to live we should unlock the lock
		pthread_mutex_unlock(&mutex);
		
	}
	
	void wakeup()
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&condition);
		pthread_mutex_unlock(&mutex);
	}
	
};
}

#endif
