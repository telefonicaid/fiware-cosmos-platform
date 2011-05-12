#include "Stopper.h"				// Own interface
#include <sys/time.h>            // struct timeval
#include "logMsg.h"					 // LM_M()


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
	
    void Stopper::stop_begin()
    {
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		if ( ans )
			LM_X(1,("pthread_mutex_lock return an error"));
        
        
    }
    
    void Stopper::stop_finish( int max_seconds)
    {
        if( max_seconds < 0 )
            max_seconds = 0;
        
        if( max_seconds == 0)
        {
            // This unlock the mutex and froze the process in the condition
            pthread_cond_wait(&_condition, &_lock);
        }
        else
        {
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + max_seconds;
            ts.tv_nsec = 0;
            
            
            // This unlock the mutex and froze the process in the condition
            pthread_cond_timedwait(&_condition, &_lock , &ts);
        }
		
		// UNLOCK the mutex
		int ans = pthread_mutex_unlock(&_lock);
		if( ans )
			LM_X(1,("pthread_mutex_unlock return an error"));
        
    }
   
    
	void Stopper::stop( int max_seconds  )
	{
		
		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		if ( ans )
			LM_X(1,("pthread_mutex_lock return an error"));

        if( max_seconds < 0 )
            max_seconds = 0;
        
        if( max_seconds == 0)
        {
            // This unlock the mutex and froze the process in the condition
            pthread_cond_wait(&_condition, &_lock);
        }
        else
        {
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + max_seconds;
            ts.tv_nsec = 0;
            
            
            // This unlock the mutex and froze the process in the condition
            pthread_cond_timedwait(&_condition, &_lock , &ts);
        }
		
		// UNLOCK the mutex
		ans = pthread_mutex_unlock(&_lock);
		if ( ans )
			LM_X(1,("pthread_mutex_unlock return an error"));
		
	}
	
	void Stopper::wakeUp(  )
	{
		int ans;
		
		ans = pthread_mutex_lock(&_lock);
		if ( ans )
			LM_X(1,("pthread_mutex_lock return an error"));

		ans = pthread_cond_signal(&_condition);
		if ( ans )
			LM_X(1,("pthread_cond_signal return an error"));

		ans = pthread_mutex_unlock(&_lock);
		if ( ans )
			LM_X(1,("pthread_mutex_unlock return an error"));

	}
	
	void Stopper::wakeUpAll(  )
	{
		pthread_mutex_lock(&_lock);
		pthread_cond_broadcast(&_condition);
		pthread_mutex_unlock(&_lock);
	}	
	
}