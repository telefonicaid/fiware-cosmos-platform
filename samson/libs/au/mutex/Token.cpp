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

TokenOwner::TokenOwner() : locked_(false) {
  int r_init = pthread_mutex_init(&lock_, 0);

  if (r_init != 0) {
    LM_X(1, ("pthread_mutex_init for TokenOwner returned %d", r_init ));
  }
}

void TokenOwner::SetMe() {
  // Lock local mutex
  int ans = pthread_mutex_lock(&lock_);

  if (ans) {
    LM_X(1, ("Error in pthread_mutex_lock in TokenOwner  %d", ans));
  }
  if (locked_) {
    LM_X(1, ("Error SetMeAsOwner in when it was locked"));
  }

  locked_ = true;
  token_owner_thread_t_ = pthread_self();
  token_owner_thread_description_ = GetThreadId(token_owner_thread_t_);

  // Unlock local mutex
  ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_X(1, ("Error in pthread_mutex_unlock in TokenOwner  %d", ans));
  }
}

std::string TokenOwner::str() {
  std::ostringstream output;

  // Lock local mutex
  int ans = pthread_mutex_lock(&lock_);

  if (ans) {
    LM_X(1, ("Error in pthread_mutex_lock in TokenOwner  %d", ans));
  }

  if (!locked_) {
    output << "[Not locked]";
  } else {
    output << "[Locked by " << GetThreadId(token_owner_thread_t_) << "]";
  }

  // Unlock local mutex
  ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_X(1, ("Error in pthread_mutex_unlock in TokenOwner  %d", ans));
  }
  return output.str();
}

bool TokenOwner::HasOwner() {
  // Lock local mutex
  int ans = pthread_mutex_lock(&lock_);

  if (ans) {
    LM_X(1, ("Error in pthread_mutex_lock in TokenOwner  %d", ans));
  }

  bool locked_by_anyone = locked_;

  // Unlock local mutex
  ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_X(1, ("Error in pthread_mutex_unlock in TokenOwner  %d", ans));
  }

  return locked_by_anyone;
}

bool TokenOwner::IsMe() {
  // Lock local mutex
  int ans = pthread_mutex_lock(&lock_);

  if (ans) {
    LM_X(1, ("Error in pthread_mutex_lock in TokenOwner  %d", ans));
  }

  bool locked_by_me = true;

  if (!locked_) {
    locked_by_me = false;
  } else if (!pthread_equal(pthread_self(), token_owner_thread_t_)) {
    locked_by_me = false;
  }

  // Unlock local mutex
  ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_X(1, ("Error in pthread_mutex_unlock in TokenOwner  %d", ans));
  }

  return locked_by_me;
}

void TokenOwner::ClearMe() {
  // Lock local mutex
  int ans = pthread_mutex_lock(&lock_);

  if (ans) {
    LM_X(1, ("Error in pthread_mutex_lock in TokenOwner  %d", ans));
  }

  if (!locked_) {
    LM_X(1, ("Error ClearOwnership when it was not locked"));
  }

  if (!pthread_equal(pthread_self(), token_owner_thread_t_)) {
    LM_X(1, ("Error ClearOwnership when I am not the owner"));
  }

  locked_ = false;
  token_owner_thread_description_ = "none";

  // Unlock local mutex
  ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_X(1, ("Error in pthread_mutex_unlock in TokenOwner  %d", ans));
  }
}

Token::Token(const std::string& name) : name_(name) {
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
  if (owner_.IsMe()) {
    ++counter_;
    return;
  }

#ifdef DEBUG_AU_TOKEN
  LockDebugger::shared()->AddMutexLock(this);
#endif

  // LOCK the mutex
  int ans = pthread_mutex_lock(&lock_);
  if (ans) {
    LM_X(1, ("Token (%s): pthread_mutex_lock returned error %d (owner %s)"
             , name_.c_str(), ans, owner_.str().c_str()));
  }

  if (owner_.HasOwner()) {
    LM_X(1, ("Token (%s): Retaining an owened token (owner %s)"
             , name_.c_str(), owner_.str().c_str()));
  }

  // Set me as the owner of this mutex
  owner_.SetMe();
  counter_ = 1;
}

void Token::Release() {
  // You are supposed to be retaining this lock
  if (!owner_.IsMe()) {
    LM_X(1, ("Token (%s) Error release a token not owned by me (owner %s)", name_.c_str(), owner_.str().c_str()));
  }

  --counter_;
  if (counter_ > 0) {
    return;
  }

  // I am not the owner anymore
  owner_.ClearMe();

#ifdef DEBUG_AU_TOKEN
  LockDebugger::shared()->RemoveMutexLock(this);
#endif

  // UNLOCK the mutex
  int ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_LE(("Error %d releasing mutex (EINVAL:%d, EFAULT:%d, EPERM:%d", ans, EINVAL, EFAULT, EPERM));
    // Goyo. The segmentation fault when quitting delilah seems to be related to a corruption in name (SAMSON-314)
    LM_X(1, ("Token %p: pthread_mutex_unlock returned error %d (%p)", name_.c_str(), ans, this));
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
  if (!owner_.IsMe()) {
    LM_X(1, ("Internal error: Stop called in a au::Token while not locked by me (%s)", name_.c_str(), str_debug().c_str()));
  }

  // information about my retain
  int tmp_counter = counter_;  // Keep the counter

  owner_.ClearMe();  // I am not the owner temporally

  // This unlock the mutex and froze the process in the condition
  if (pthread_cond_wait(&block_, &lock_) != 0) {
    LM_X(1, ("Internal error at au::TokenTaker  pthread_cond_wait error"));
  }

  // I am the owner again
  if (owner_.HasOwner()) {
    LM_X(1, ("Internal error recovering ownership of au::Token"));
  }

  owner_.SetMe();
  counter_ = tmp_counter;
}

std::string Token::str_debug() {
  return au::str("Token %s (Owner %s:%d) ", name_.c_str(), owner_.str().c_str(), counter_);
}
}
