

#include "au/Token.h"				// Own interface
#include "LockDebugger.h"		// LockDebugger
#include "logMsg/logMsg.h"					 // LM_M()

#define DEBUG_AU_TOKEN

namespace au
{
	Token::Token( const char * _name )
	{
        // Take the name for debuggin
        name = _name;
        
		taken = false;
		pthread_mutex_init(&_lock, 0);
		pthread_cond_init(&_condition, NULL);
		LM_M(("Created token '%s' (%p)", name, this));
	}
	
	Token::~Token()
	{
		LM_M(("Destroying token '%s' (%p)", name, this));
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
			LM_X(1,("Token %s: pthread_mutex_lock returned error %d (%p)", name, ans, this));

		
		while( true )
		{
			if( !taken )
			{
				taken = true;
				
				// UNLOCK the mutex
				int ans = pthread_mutex_unlock(&_lock);
				if( ans )
					LM_X(1,("Token %s: pthread_mutex_unlock return an error",name));
				
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
			LM_X(1,("token %s: pthread_mutex_lock return an error",name));
		
		if( !taken )
			LM_X(1,("token %s: Internal error of the au::token library since we are releasing a token that was never taken",name));

		taken = false;
		
		// Wake up one of the sleeping elements
		ans = pthread_cond_signal(&_condition);
		if( ans ) 
			LM_X(1,("token %s: pthread_cond_signal return an error",name));
		
		// UNLOCK the mutex
		ans = pthread_mutex_unlock(&_lock);
		if( ans ) 
			LM_X(1,("token %s: pthread_mutex_unlock return an error",name));
		
	}
}
