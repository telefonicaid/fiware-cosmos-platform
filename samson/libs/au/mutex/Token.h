
/* ****************************************************************************
*
* FILE            Token
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
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

namespace au {
class Token {
  const char *name;              // Name of the token

  pthread_mutex_t _lock;         // Mutex to protect this tocken
  pthread_cond_t _block;         // Condition to block threads that call stop

  // Mechanism to discover if you have locked this mutex
  pthread_t t;
  bool locked;
  int counter;   // Number of times this token is taken

public:

  Token(const char *name = "no name");
  ~Token();

private:

  void Retain();
  void Release();
  void Stop();

  // It is only possible to retain teh token with the class TokenTaker
  friend class TokenTaker;
};
}

#endif // ifndef _H_AU_TOCKEN
