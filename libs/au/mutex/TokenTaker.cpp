

#include <sys/time.h>                   // gettimeofday()

#include "logMsg/logMsg.h"              // LM_M()

#include "LockDebugger.h"               // LockDebugger
#include "au/mutex/Token.h"                   // au::Token
#include "au/Cronometer.h"              // au::Cronometer
#include "TokenTaker.h"				    // Own interface

//#define DEBUG_AU_TOKEN

NAMESPACE_BEGIN(au)

TokenTaker::TokenTaker( Token* _token  )
{
    au::ExecesiveTimeAlarm alarm("TokenTaker::TokenTaker");
    
    token = _token;
    name = "Unknown";
    
    //LM_M(("New TokenTaker %s for token %s", name ,  token->name));
    token->retain();
}

TokenTaker::TokenTaker( Token* _token , const char* _name )
{
    au::ExecesiveTimeAlarm alarm("TokenTaker::TokenTaker");
    
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


void TokenTaker::stop(  )
{
    // This unlock the mutex and froze the process in the condition
    if( pthread_cond_wait(&token->_block, &token->_lock) != 0)
        LM_X(1, ("Internal error at au::TokenTaker"));
}

/*
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
 */

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

NAMESPACE_END
