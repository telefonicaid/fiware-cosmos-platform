
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

namespace au {
class Token {
    
  public:
    
    Token( const char * name = "no name" );
    ~Token();
    
  private:
    // It is only possible to retain the token with the class TokenTaker
    friend class TokenTaker;

    const char* name_; // Name of the token
    
    void Retain();
    void Release();
    void Stop();
    bool IsValidName() const;

    pthread_mutex_t lock_; // Mutex to protect this token
    pthread_cond_t block_; // Condition to block threads that call stop
    
    
    // Mechanism to discover if you have locked this mutex
    pid_t token_owner_thread_id_;
    volatile bool locked_;
    volatile int counter_; // Number of times this token is taken
  };
}

#endif  // ifndef _H_AU_TOCKEN
