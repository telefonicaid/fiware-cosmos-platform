


#include <assert.h>
#include <sys/time.h>           // gettimeofday()

#include "LockDebugger.h"		// LockDebugger
#include "logMsg/logMsg.h"		// LM_M()
#include "au/string.h"

#include "au/mutex/Token.h"				// Own interface


//#define DEBUG_AU_TOKEN

namespace au {
  
  std::string get_thread_id( pthread_t t )
  {
    std::ostringstream output;
    
    unsigned char *base = (unsigned char*) &t;
    size_t i;
    for (i = 0; i < sizeof(t); i ++) 
    {
      output << au::str("%02x", base[i]);
    }
    return output.str();
  }
  
  Token::Token( const char * _name )
  {
    
    // Take the name for debuggin
    name = _name;
    
    //LM_LM(("Token %s created" , name ));
    
    int r_init = pthread_mutex_init(&_lock, 0);
    
    if( r_init != 0 )
      LM_X(1, ("pthread_mutex_init for '%s' returned %d" , name , r_init ));
    
    int t_init_cond = pthread_cond_init(&_block, NULL);
    
    if( t_init_cond != 0 )
      LM_X(1, ("pthread_cond_init for '%s' returned %d" , name , r_init ));
    
    locked = false;
  }
  
  Token::~Token()
  {
    //LM_LM(("Token %s was destroyed" , name ));
    
    //LM_M(("Destroying token '%s' (%p)", name, this));
    pthread_mutex_destroy(&_lock);
    pthread_cond_destroy(&_block);
  }
  
  void Token::Retain(  )
  {
    //LM_LM(("Thread [%s] trying to token %s..." , get_thread_id( pthread_self() ).c_str() ,  name ));
    
    if( locked && pthread_equal(pthread_self(), t ) )
    {
      counter++;
      //LM_LM(("token %s was retained by me ( thread [%s] ) . Just updating counter to %d " , name , get_thread_id( pthread_self() ).c_str() ,  counter ));
      return;
    }
    
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
    
    
    if( locked )
      LM_X(1,("Internal error: Thread [%s] has retained au::Token (%s) previously locked by thread [%s]" , get_thread_id( pthread_self() ).c_str() , name , get_thread_id( t ).c_str() ));
    
    t = pthread_self();
    counter = 1;
    locked = true;
    
    //LM_LM(("Thread [%s] has retained token %s..." , get_thread_id( pthread_self() ).c_str() ,  name ));
    
  }
  
  void Token::Release( )
  {
    //LM_LM(("Thread [%s] trying to releases token %s..." , get_thread_id( pthread_self() ).c_str() ,  name ));
    
    if( !locked )
      LM_E(("Internal error: Releasing a non-locked au::Token"));
    
    if( ! pthread_equal(pthread_self(), t) )
      LM_E(("Internal error: Releasing an au::Token not locked by me"));
    
    counter--;
    if( counter > 0 )
    {
      //LM_LM(("Token %s is still locked by thread [%s] with counter %d" , name , get_thread_id( pthread_self() ).c_str() , counter ));
      return;
    }
    
    // Flag this as unlocked
    locked = false;
    
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
    
    //LM_LM(("Thread [%s] completelly releases by token %s..." , get_thread_id( pthread_self() ).c_str() ,  name ));
    
  }
  
  
  void Token::Stop()
  {
    //LM_LM(("Thread [%s] is being stopeed at token %s..." , get_thread_id( pthread_self() ).c_str() ,  name ));
    
    // You are suppoused to be retaining this lock
    if( !locked )
      LM_E(("Internal error: Releasing a non-locked au::Token"));
    
    if( !pthread_equal(pthread_self(), t) )
      LM_E(("Internal error: Releasing an au::Token not locked by me"));
    
    int tmp_counter = counter; // Keep counter of retains
    locked = false;            // We are temporally releasing this token
    
    // This unlock the mutex and froze the process in the condition
    if( pthread_cond_wait(&_block, &_lock) != 0 )
      LM_X(1, ("Internal error at au::TokenTaker"));
    
    //LM_LM(("Thread [%s] is back from stopeed at token %s..." , get_thread_id( pthread_self() ).c_str() ,  name ));
    
    // Now you are retaining again
    locked = true;
    t = pthread_self();
    counter = tmp_counter;
    
  }
  
}
