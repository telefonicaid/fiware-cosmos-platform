#ifndef SAMSON_LOCKER_H
#define SAMSON_LOCKER_H

/**
 Easy way to create a mutex.
 It allows to check if someone is blocked using lock
 */

#include <assert.h>
#include <sys/time.h>
#include <set>
#include <map>
#include <iostream>

#include "StopLock.h"       /* StopLock                 */


namespace au {

	
class Lock;

/**
 Class to implement the classical mehcanism to be stoped for a condition
 Works in conjuntion with Lock
 */

class LockDebugger
{
	pthread_mutex_t _lock;
	std::map< pthread_t , std::set<Lock*>* > locks;

	
	LockDebugger();
	~LockDebugger();
	
public:
	pthread_key_t key_title;	// A title for each thread
	
	static LockDebugger* shared();
	
private:

	std::set<Lock*> * getLocksVector();
	bool cross_blocking( Lock* new_lock );
	std::string getTitle();
	
public:
	void add_lock( Lock* new_lock );
	void remove_lock(  Lock* new_lock );
	

	static void setThreadTitle(std::string);
	

};


/**
 ThreadStopPoint is the classical mehcanish to stop some threads to wait for something.
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
