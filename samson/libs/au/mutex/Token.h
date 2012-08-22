
/* ****************************************************************************
*
* FILE            Token
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            September 2011
*
* DESCRIPTION
*
*  Wrapper of the mutex included in pthread library.
*  Used to simplify development of multi-thread apps
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_AU_TOCKEN
#define _H_AU_TOCKEN

#include <pthread.h>             /* pthread_mutex_t                          */
#include <string>
#include <sys/types.h>

namespace au {
class Token {
public:

  Token(const std::string& name = "no name");
  ~Token();

  // Check if I am retaining this token
  bool IsRetainedByMe() const;

private:

  // To avoid missing releases, we use helper class TokenTaker
  friend class TokenTaker;

  void Retain();
  void Release();
  void Stop();
  void WakeUpAll();
  void WakeUp();

  pthread_mutex_t lock_;   // Mutex to protect this token
  pthread_cond_t block_;   // Condition to block threads that call stop

  std::string name_;  // Name of the token for debugging

  // Mechanism to discover if you have locked this mutex
  // Allowing multiple Retains from the same thread
  pid_t token_owner_thread_id_;
  volatile bool locked_;
  volatile int counter_;   // Number of times this token is taken
};
}

#endif  // ifndef _H_AU_TOCKEN
