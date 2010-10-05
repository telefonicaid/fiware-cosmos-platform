#include <sstream>

#include "samsonLogMsg.h"
#include "AULocker.h"



//#define FULL_DEBUG_AU_THREADS
#define DEBUG_AU_THREADS


namespace au {

	class LockDebugger;
	LockDebugger* lockDebugger;

	
	LockDebugger* LockDebugger::shared()
	{
		if( !lockDebugger)
			lockDebugger = new LockDebugger();	
		return lockDebugger;
	}
	
	LockDebugger::LockDebugger()
	{
		pthread_mutex_init(&_lock, 0);
		pthread_key_create(&key_title, NULL);
	}
	
	LockDebugger::~LockDebugger()
	{
		pthread_mutex_destroy(&_lock);
	}
	
	std::set<Lock*> * LockDebugger::getLocksVector()
	{
		pthread_t p  = pthread_self();
		
		std::set<Lock*> *locksVector;		
		std::map< pthread_t , std::set<Lock*>* >::iterator i = locks.find(p);
		if( i == locks.end() )
		{
			locksVector =  new std::set<Lock*>();
			locks.insert( std::pair< pthread_t , std::set<Lock*>* >( p , locksVector) );
		}
		else
			locksVector= i->second;
		
		
		return locksVector;
	}
	
	bool LockDebugger::cross_blocking( Lock* new_lock )
	{
		std::set<Lock*> *myLocks = getLocksVector();
		
		std::map< pthread_t , std::set<Lock*>* >::iterator i;
		for (i = locks.begin() ; i != locks.end() ; i++)
		{
			if( i->first != pthread_self() )
			{
				// Check if it have my new lock
				if( i->second->find(new_lock) != i->second->end() )
				{
					// It contains the lock I am traying to get
					// Let see if they have any of my previous locks
					std::set<Lock*>::iterator j;
					for (j = myLocks->begin() ;  j != myLocks->end() ; j++)
						if( i->second->find( *j ) != i->second->end() )
						{
#ifdef FULL_DEBUG_AU_THREADS
							std::cout << "Cross lock detected. Me: " << myLocks->size() << " Other: " << i->second->size() << std::endl;
#endif							
							return true;
						}
					
				}
				
			}
		}
		
		
		return false;
		
	}
	
	void LockDebugger::add_lock( Lock* new_lock )
	{
		// Lock private data

		// Block until the mutex is free
		int ans = pthread_mutex_lock(&_lock);

		// Make sure there are no errors with lock
		if (ans != 0)
		{
			LOG_ERROR(("pthread_mutex_lock error"));
			assert(ans == 0);
		}
		
		std::set<Lock*> *locksVector = getLocksVector();

		
#ifdef FULL_DEBUG_AU_THREADS
		std::ostringstream o;
		o << "Add thread \"" << getTitle() << "\" [LOCKS: " << locksVector->size() << "] to lock \"" << new_lock->description << "\"" <<std::endl;
		std::cout << o.str();
#endif
		
		
		// Make some checks here...
		
		// We do not autoblock
		if (locksVector->find( new_lock ) !=  locksVector->end() )
			std::cout << "Autolock detected " << locksVector->size() << std::endl;

		assert( locksVector->find( new_lock ) ==  locksVector->end() );
		
		// We are not blocked
		assert( !cross_blocking(new_lock) );
		
		// Add the new lock
		locksVector->insert( new_lock );
		
		
		// Unlock
		pthread_mutex_unlock(&_lock);
		
	}
	
	void LockDebugger::remove_lock(  Lock* new_lock )
	{
		
		
		// Lock private data
		int ans = pthread_mutex_lock(&_lock);	// Block until the mutex is free

		if (ans != 0)
		{
			LOG_ERROR(("pthread_mutex_lock error"));
			assert(ans == 0);
		}
		
		std::set<Lock*> *locksVector = getLocksVector();

#ifdef FULL_DEBUG_AU_THREADS
		std::ostringstream o;
		o << "Removing thread \"" << getTitle() << "\" [LOCKS: " << locksVector->size() << "] to lock \"" << new_lock->description << "\"" <<std::endl;
		std::cout << o.str();
#endif		
		// Make sure it was there
		assert( locksVector->find( new_lock ) != locksVector->end() );
		
		locksVector->erase( new_lock );
		
		
		// Unlock
		pthread_mutex_unlock(&_lock);
		
		
	}
	
	std::string LockDebugger::getTitle()
	{
		void *data = pthread_getspecific( lockDebugger->key_title );
		if( data )
			return *((std::string*)data);
		else
			return "Unknown";
	}
	
	void LockDebugger::setThreadTitle(std::string title)
	{
		void *data = pthread_getspecific( LockDebugger::shared()->key_title );

		if (!data)
		{
			LOG_ERROR(("pthread_getspecific returned NULL"));
			assert(!data);  // Only put the name on the thread once
		}
	
		pthread_setspecific( lockDebugger->key_title  , new std::string( title ) );
	}

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
#ifdef DEBUG_AU_THREADS		
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
		
#ifdef DEBUG_AU_THREADS		
		LockDebugger::shared()->remove_lock( this );
#endif		
		pthread_mutex_unlock(&_lock);
	}
	
	
	void Lock::unlock_waiting_in_stopLock( StopLock *stopLock )
	{
#ifdef DEBUG_AU_THREADS		
		LockDebugger::shared()->remove_lock( this );		
#endif
		
		pthread_cond_wait(&stopLock->condition, &_lock);	// This unlock the mutex and froze the process in the condition
		pthread_mutex_unlock(&_lock);						// This unocks the mutes because it has gained again
		
	}
	
	void Lock::unlock_waiting_in_stopLock( StopLock *stopLock , int max_seconds )
	{
#ifdef DEBUG_AU_THREADS		
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
