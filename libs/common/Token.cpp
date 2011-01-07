

#include "Token.h"				// Own interface
#include "LockDebugger.h"		// LockDebugger


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
		LockDebugger::shared()->add_lock( this );
		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		assert(!ans); // We do not accept falling simple mutex

		
		while( true )
		{
			if( !taken )
			{
				taken = true;
				
				// UNLOCK the mutex
				int ans = pthread_mutex_unlock(&_lock);
				assert(!ans); // We do not accept falling simple mutex
				
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
		LockDebugger::shared()->remove_lock( this );
		
		// LOCK the mutex
		int ans = pthread_mutex_lock(&_lock);
		assert(!ans); // We do not accept falling simple mutex
		
		assert(taken);	// We are the taken of this one
		taken = false;
		
		// Wake up one of the sleeping elements
		ans = pthread_cond_signal(&_condition);
		assert( !ans );
		
		// UNLOCK the mutex
		ans = pthread_mutex_unlock(&_lock);
		assert(!ans); // We do not accept falling simple mutex
		
		
	}
	
	
}