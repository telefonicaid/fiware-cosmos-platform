

#include "au/Token.h"				// Own interface
#include "LockDebugger.h"		// LockDebugger
#include "logMsg.h"					 // LM_M()

#define DEBUG_AU_TOKEN

namespace au
{
	Token::Token()
	{
		taken = false;
		pthread_mutex_init(&_lock, 0);
		pthread_cond_init(&_condition, NULL);
	}
	
	Token::~Token()
	{
		pthread_mutex_destroy(&_lock);
		pthread_cond_destroy(&_condition);
	}
	
	void Token::retain(  )
	{
#ifdef DEBUG_AU_TOKEN
		LockDebugger::shared()->add_lock( this );
#endif		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		if( ans )
			LM_X(1,("pthread_mutex_lock return an error"));

		
		while( true )
		{
			if( !taken )
			{
				taken = true;
				
				// UNLOCK the mutex
				int ans = pthread_mutex_unlock(&_lock);
				if( ans )
					LM_X(1,("pthread_mutex_unlock return an error"));
				
				return;// Now we have the tocken
			}
			else
			{
				// Lock until
				pthread_cond_wait(&_condition, &_lock);// This unlock the mutex and froze the process in the condition
			}
		}
		
	}
	
	void Token::release( )
	{
#ifdef DEBUG_AU_TOKEN
		LockDebugger::shared()->remove_lock( this );
#endif		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		if( ans )
			LM_X(1,("pthread_mutex_lock return an error"));
		
		if( !taken )
			LM_X(1,("Internal error of the au::token library since we are releasing a token that was never taken"));

		taken = false;
		
		// Wake up one of the sleeping elements
		ans = pthread_cond_signal(&_condition);
		if( ans ) 
			LM_X(1,("pthread_cond_signal return an error"));
		
		// UNLOCK the mutex
		ans = pthread_mutex_unlock(&_lock);
		if( ans ) 
			LM_X(1,("pthread_mutex_unlock return an error"));
		
	}
}
