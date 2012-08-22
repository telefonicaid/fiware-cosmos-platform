#include "au/mutex/Token.h"       // Own interface



#include <assert.h>
#include <sys/time.h>           // gettimeofday()
#include <syscall.h>

#include "au/string.h"
#include "LockDebugger.h"		// LockDebugger
#include "logMsg/logMsg.h"		// LM_M()

namespace au {
  

std::string GetThreadId(pthread_t t) {
  std::ostringstream output;

  unsigned char *base = reinterpret_cast<unsigned char*> (&t);
  for (size_t i = 0; i < sizeof(t); ++i) {
    output << au::str("%02x", base[i]);
  }
  return output.str();
}
  
inline bool Token::IsValidName() const {
  if ((name_ != NULL) && (name_ != (char *) 0xffffffff) &&
      (name_ > (char *) 0x10000000) &&
      ((name_[0] > 'a') && (name_[0] < 'Z')) &&
      ((name_[1] > 'a') && (name_[1] < 'Z'))) {
    return true;
  } else {
    return false;
  }
}
    

Token::Token(const char * name) : name_(name), locked_(false) {
  // Take the name for debugging

  int r_init = pthread_mutex_init(&lock_, 0);

  if (r_init != 0)
    LM_X(1, ("pthread_mutex_init for '%s' returned %d" , name_ , r_init ));

  int t_init_cond = pthread_cond_init(&block_, NULL);

  if (t_init_cond != 0)
    LM_X(1, ("pthread_cond_init for '%s' returned %d" , name_ , r_init ));
}
  
Token::~Token() {
  pthread_mutex_destroy(&lock_);
  pthread_cond_destroy(&block_);
  }
  
void Token::Retain() {
  //LM_LM(("Thread [%s] trying to token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_ ));
    
  pid_t my_own_pid_t = syscall(SYS_gettid);
  if (locked_ && (token_owner_thread_id_ == my_own_pid_t)) {
    ++counter_;
    //LM_LM(("token %s was retained by me ( thread [%s] ) . Just updating counter to %d " , name_ , GetThreadId( pthread_self() ).c_str() ,  counter_ ));
      return;
    }
    
#ifdef DEBUG_AU_TOKEN
    LockDebugger::shared()->add_lock( this );
#endif		
    // LOCK the mutex
  int ans = pthread_mutex_lock(&lock_);
  if (ans) {
      LM_LE(("Error %d getting mutex  (EINVAL:%d, EFAULT:%d, EDEADLK:%d", ans, EINVAL, EFAULT, EDEADLK));
    if (IsValidName()) {
      LM_LE(("Token %s: pthread_mutex_lock returned error %d (%p)", name_, ans, this));
      }
    else {
      LM_LE(("Token (wrong name:%p): pthread_mutex_lock returned error %d (%p)", name_, ans, this));
      }
    }
    
  if (locked_)
    LM_X(1,("Internal error: Thread [%s] has retained au::Token (%s) previously locked by thread [%s]",
            GetThreadId( pthread_self() ).c_str() , name_ , GetThreadId( token_owner_thread_id_ ).c_str() ));
    
  token_owner_thread_id_ = my_own_pid_t;
  counter_ = 1;
  locked_ = true;
    
  //LM_LM(("Thread [%s] has retained token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_ ));
    
  }
  
void Token::Release() {
  //LM_LM(("Thread [%s] trying to releases token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_ ));
    
  if (!locked_)
    LM_LE(("Internal error: Releasing a non-locked au::Token:'%s'", name_));
    
  pid_t my_own_pid_t = syscall(SYS_gettid);
  if (token_owner_thread_id_ != my_own_pid_t)
    LM_LE(("Internal error: Releasing an au::Token '%s' not locked by me, owner:%d, me:%d", name_, token_owner_thread_id_, my_own_pid_t));
    
  --counter_;
  if (counter_ > 0) {
    //LM_LM(("Token %s is still locked by thread [%s] with counter %d" , name , GetThreadId( pthread_self() ).c_str() , counter_ ));
      return;
    }
    
    // Flag this as unlocked
  locked_ = false;
    
#ifdef DEBUG_AU_TOKEN
    LockDebugger::shared()->remove_lock( this );
#endif		
    // UNLOCK the mutex
  int ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_LE(("Error %d releasing mutex (EINVAL:%d, EFAULT:%d, EPERM:%d", ans, EINVAL, EFAULT, EPERM));
      // Goyo. The segmentation fault when quitting delilah seems to be related to a corruption in name (SAMSON-314)
    if (name_ != NULL) {
      LM_LE(("Token %p: pthread_mutex_unlock returned error %d (%p)", name_, ans, this));
      }
    else {
      LM_LE(("Token (NULL name:%p): pthread_mutex_lock returned error %d (%p)", name_, ans, this));
      }
    }
    
  //LM_LM(("Thread [%s] completely releases by token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_ ));
    
  }
  
void Token::Stop() {
  //LM_LM(("Thread [%s] is being stopped at token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_ ));
  
  // You are supposed to be retaining this lock
  if (!locked_)
    LM_LE(("Internal error: Releasing a non-locked au::Token:'%s'", name_));
    
  pid_t my_own_pid_t = syscall(SYS_gettid);
  if (token_owner_thread_id_ != my_own_pid_t)
    LM_LE(("Internal error: Stopping an au::Token '%s' not locked by me, owner:%d, me:%d", name_, token_owner_thread_id_, my_own_pid_t));
    
  int tmp_counter = counter_; // Keep counter of retains
  locked_ = false; // We are temporally releasing this token
    
    // This unlock the mutex and froze the process in the condition
  if (pthread_cond_wait(&block_, &lock_) != 0)
      LM_X(1, ("Internal error at au::TokenTaker"));
    
  //LM_LM(("Thread [%s] is back from stopped at token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_ ));
    
    // Now you are retaining again
  locked_ = true;
  token_owner_thread_id_ = my_own_pid_t;
  counter_ = tmp_counter;
    
  }
}
