


#include <sys/time.h>           // gettimeofday()

#include "LockDebugger.h"		// LockDebugger
#include "logMsg/logMsg.h"		// LM_M()

#include "au/mutex/Token.h"				// Own interface

#include <assert.h>

//#define DEBUG_AU_TOKEN

NAMESPACE_BEGIN(au)

Token::Token( const char * _name )
{
    // Take the name for debuggin
    name = _name;
    
    int r_init = pthread_mutex_init(&_lock, 0);
    int t_init_cond = pthread_cond_init(&_block, NULL);
    
    if( r_init != 0 )
        LM_X(1, ("pthread_mutex_init for '%s' returned %d" , name , r_init ));
    
    if( t_init_cond != 0 )
        LM_X(1, ("pthread_cond_init for '%s' returned %d" , name , t_init_cond ));
    
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
    {
        LM_LE(("Error %d getting mutex  (EINVAL:%d, EFAULT:%d, EDEADLK:%d", ans, EINVAL, EFAULT, EDEADLK));
        //assert(false);
        if ((name != NULL) && (name != (char *)0xffffffff) && (name > (char *)0x10000000) && ((name[0] > 'a') && (name[0] < 'Z')) && ((name[1] > 'a') && (name[1] < 'Z')))
        {
            LM_LE(("Token %s: pthread_mutex_lock returned error %d (%p)", name, ans, this));
        }
        else
        {
            LM_LE(("Token (wrong name:%p): pthread_mutex_lock returned error %d (%p)", name, ans, this));
        }
    }
    
}

void Token::release( )
{
    
#ifdef DEBUG_AU_TOKEN
    LockDebugger::shared()->remove_lock( this );
#endif		
    // UNLOCK the mutex
    int ans = pthread_mutex_unlock(&_lock);
    if( ans )
    {
        LM_E(("Error %d releasing mutex (EINVAL:%d, EFAULT:%d, EPERM:%d", ans, EINVAL, EFAULT, EPERM));
        // Goyo. The segmentation fault when quitting delilah seems to be related to a corruption in name (SAMSON-314)
        if (name != NULL)
        {
            LM_E(("Token %p: pthread_mutex_unlock returned error %d (%p)", name, ans, this));
        }
        else
        {
            LM_E(("Token (NULL name:%p): pthread_mutex_lock returned error %d (%p)", name, ans, this));
        }
    }
}

NAMESPACE_END
