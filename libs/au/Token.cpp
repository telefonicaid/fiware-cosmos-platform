


#include <sys/time.h>           // gettimeofday()

#include "LockDebugger.h"		// LockDebugger
#include "logMsg/logMsg.h"					 // LM_M()

#include "au/Token.h"				// Own interface

#define DEBUG_AU_TOKEN

namespace au
{
	Token::Token( const char * _name )
	{
        // Take the name for debuggin
        name = _name;
        
		pthread_mutex_init(&_lock, 0);
        pthread_cond_init(&_block, NULL);
        
	}
	
	Token::~Token()
	{
		//LM_M(("Destroying token '%s' (%p)", name, this));
		pthread_mutex_destroy(&_lock);
		pthread_cond_destroy(&_block);
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
		
	}
	
	void Token::release( )
	{
        
#ifdef DEBUG_AU_TOKEN
		LockDebugger::shared()->remove_lock( this );
#endif		
		// LOCK the mutex
		int ans = pthread_mutex_unlock(&_lock);
		if( ans )
			LM_X(1,("token %s: pthread_mutex_lock return an error",name));
	}
    
#pragma mark TokenTaker

    TokenTaker::TokenTaker( Token* _token  )
    {
        token = _token;
        name = "Unknown";
        
        //LM_M(("New TokenTaker %s for token %s", name ,  token->name));
        token->retain();
    }
    
    TokenTaker::TokenTaker( Token* _token , const char* _name )
    {
        token = _token;
        name = _name;
        
        //LM_M(("New TokenTaker %s for token %s", name ,  token->name));
        token->retain();
    }
    
    TokenTaker::~TokenTaker()
    {
        //LM_M(("Destroy TokenTaker %s for token %s", name ,  token->name));
        token->release();
    }
    
    
    void TokenTaker::stop( int time_out )
    {
        //LM_M(("Stop TokenTaker %s during %d for token %s", name , time_out, token->name));
        
        // Stop the process while 
        if( time_out < 0 )
            time_out = 0;
        
        if( time_out == 0)
        {
            // This unlock the mutex and froze the process in the condition
            if( pthread_cond_wait(&token->_block, &token->_lock) != 0)
                LM_X(1, ("Internal error at au::TokenTaker"));
        }
        else
        {
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + time_out;
            ts.tv_nsec = 0;
            
            
            // This unlock the mutex and froze the process in the condition
            int ans = pthread_cond_timedwait(&token->_block, &token->_lock , &ts);
            if( (ans != 0) && (ans != ETIMEDOUT) ) 
                LM_X(1, ("Internal error at au::TokenTaker. pthread_cond_timedwait returned %d" , ans));

        }            
        
        //LM_M(("Finish Stop TokenTaker %s during %d for token %s", name , time_out, token->name));
        
        
    }
    
    void TokenTaker::wakeUp()
    {
        // Wake up a thread that has been "stop"
        //LM_M(("Wake up for token %s", token->name ));
        
        if( pthread_cond_signal(&token->_block) != 0)
            LM_X(1, ("Internal error at au::TokenTaker"));
            
        
    }
    
    void TokenTaker::wakeUpAll()
    {
        // Wake up all stopped threads
        //LM_M(("Wake up all for token %s", token->name ));
        
        if( pthread_cond_broadcast(&token->_block) != 0)
            LM_X(1, ("Internal error at au::TokenTaker"));

    }
    
    
    
}
