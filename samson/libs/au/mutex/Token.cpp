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
#include "au/mutex/Token.h"       // Own interface



#include "LockDebugger.h"       // LockDebugger
#include "au/string/StringUtilities.h"
#include "logMsg/logMsg.h"      // LM_M()
#include <assert.h>
#include <sys/syscall.h>
#include <sys/time.h>           // gettimeofday()
#include <sys/types.h>
#include <unistd.h>


namespace au {
std::string GetThreadId(pthread_t t) {
  std::ostringstream output;

  unsigned char *base = reinterpret_cast<unsigned char *>(&t);

  for (size_t i = 0; i < sizeof(t); ++i) {
    output << au::str("%02x", base[i]);
  }
  return output.str();
}

Token::Token(const std::string& name) : name_(name), locked_(false) {
  // Take the name for debugging

  int r_init = pthread_mutex_init(&lock_, 0);

  if (r_init != 0) {
    LM_X(1, ("pthread_mutex_init for '%s' returned %d", name_.c_str(), r_init ));
  }

  int t_init_cond = pthread_cond_init(&block_, NULL);

  if (t_init_cond != 0) {
    LM_X(1, ("pthread_cond_init for '%s' returned %d", name_.c_str(), r_init ));
  }
}

Token::~Token() {
  pthread_mutex_destroy(&lock_);
  pthread_cond_destroy(&block_);
}

void Token::Retain() {

  if (IsRetainedByMe()) {
    ++counter_;
    return;
  }

#ifdef DEBUG_AU_TOKEN
  LockDebugger::shared()->AddMutexLock(this);
#endif
  // LOCK the mutex
  int ans = pthread_mutex_lock(&lock_);
  if (ans) {
    LM_LE(("Error %d getting mutex  (EINVAL:%d, EFAULT:%d, EDEADLK:%d", ans, EINVAL, EFAULT, EDEADLK));
    LM_X(1,("Token (name:%p): pthread_mutex_lock returned error %d (%p)", name_.c_str(), ans, this));
  }
  pthread_t my_own_pthread_t = pthread_self();

  if (locked_) {
    LM_X(1, ("Internal error: Thread [%s] has retained au::Token (%s) previously locked by thread [%s]",
              GetThreadId(my_own_pthread_t).c_str(), name_.c_str(), GetThreadId(token_owner_thread_t_).c_str()));
  }
  token_owner_thread_t_ = my_own_pthread_t;
  counter_ = 1;
  locked_ = true;
}

void Token::Release() {


  // You are supposed to be retaining this lock
  if (!IsRetainedByMe()) {
    LM_X(1,("Internal error: au::Token %s not locked by me ()", name_.c_str() ));
  }

  --counter_;
  if (counter_ > 0) {
    return;
  }

  // Flag this as unlocked
  locked_ = false;

#ifdef DEBUG_AU_TOKEN
  LockDebugger::shared()->RemoveMutexLock(this);
#endif
  // UNLOCK the mutex
  int ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_LE(("Error %d releasing mutex (EINVAL:%d, EFAULT:%d, EPERM:%d", ans, EINVAL, EFAULT, EPERM));
    // Goyo. The segmentation fault when quitting delilah seems to be related to a corruption in name (SAMSON-314)
    LM_X(1,("Token %p: pthread_mutex_unlock returned error %d (%p)", name_.c_str(), ans, this));
  }
}

void Token::WakeUpAll() {
  if (pthread_cond_broadcast(&block_) != 0) {
    LM_X(1, ("Internal error at au::Token. pthread_cond_broadcast error"));
  }
}

void Token::WakeUp() {
  if (pthread_cond_signal(&block_) != 0) {
    LM_X(1, ("Internal error at au::Token. pthread_cond_signal error"));
  }
}

void Token::Stop() {

  // You are supposed to be retaining this lock
  if (!IsRetainedByMe()) {
    LM_X(1,("Internal error: Stop called in a au::Token not locked by me", name_.c_str()));
  }

  // information about my retain
  pthread_t my_own_pthread_t = pthread_self();
  int tmp_counter = counter_;

  locked_ = false;    // We are temporally releasing this token

  // This unlock the mutex and froze the process in the condition
  if (pthread_cond_wait(&block_, &lock_) != 0) {
    LM_X(1, ("Internal error at au::TokenTaker  pthread_cond_wait error"));
  }
  
  // Now you are retaining again ( recover previous information about this lock )
  locked_ = true;
  token_owner_thread_t_ = my_own_pthread_t;
  counter_ = tmp_counter;
}

bool Token::IsRetainedByMe() const {
  if (locked_ && ( pthread_equal(token_owner_thread_t_, pthread_self()))) {
    return true;
  } else {
    return false;
  }
}
}
