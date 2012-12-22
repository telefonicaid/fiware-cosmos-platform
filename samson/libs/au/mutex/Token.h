/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

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
* ****************************************************************************/

#ifndef _H_AU_TOCKEN
#define _H_AU_TOCKEN

#include <pthread.h>             /* pthread_mutex_t                          */
#include <string>
#include <sys/types.h>

namespace au {
// Handy method to identify threads with strings
std::string GetThreadId(pthread_t t);

class TokenOwner {
public:

  TokenOwner();

  void SetMe();
  bool IsMe();
  void ClearMe();
  bool HasOwner();

  // Debug string
  std::string str();

private:

  pthread_mutex_t lock_;     // Mutex to protect this token

  volatile pthread_t token_owner_thread_t_;
  std::string token_owner_thread_description_;
  volatile bool locked_;
};

class Token {
public:

  explicit Token(const std::string& name);
  ~Token();

  std::string name() const {
    return name_;
  }

  bool IsRetainedByMe() {
    return owner_.IsMe();
  }

  void WakeUpAll();
  void WakeUp();

private:

  // To avoid missing releases, we use helper class TokenTaker
  friend class TokenTaker;
  friend class MultipleTokenTaker;

  void Retain();
  void Release();
  void Stop();


  pthread_mutex_t lock_;   // Main mutex
  pthread_cond_t block_;   // Condition to block threads that call "Stop"

  std::string name_;       // Name of the token for debugging

  TokenOwner owner_;  // Owner of this mutex

  volatile int counter_;   // Number of times this token is taken

  // syscall(SYS_gettid) is not supported on mac, so we come back to pthread_t
  // Specific implementation for solaris may be necessary.

  std::string str_debug();
};
}

#endif  // ifndef _H_AU_TOCKEN
