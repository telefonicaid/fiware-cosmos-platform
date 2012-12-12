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
#include "au/Thread.h"  // Own interface

#include "au/ThreadManager.h"
#include "au/log/LogMain.h"
#include "au/mutex/TokenTaker.h"
#include "au/singleton/Singleton.h"

namespace au {
void *run_Thread(void *p) {
  Thread *t = (Thread *)p;

  t->RunThread();

  {
    au::TokenTaker tt(&t->token_);
    t->pthread_running_ = false;
    tt.WakeUpAll();      // Wake up joining threads
  }

  return NULL;
}

void Thread::StartThread() {
  if (pthread_running_) {
    return;                // If already running, nothing to do
  }
  pthread_running_ = true; // Mark as running

  // Run the thread in background
  au::Singleton<au::ThreadManager>::shared()->AddThread(name_, &t_, NULL, run_Thread, this);
}

void Thread::StopThread() {
  stopping_ = true;     // Set the flag
  if (!pthread_running_) {
    return;
  }
  if (pthread_self() == t_) {
    // This is the backgroung thead, so nothing else can be done here.
    // Remember you are responsible for checking "IsThreadQuitting"
    return;
  }

  // Execute custom cancel function
  UnlockThread();

  // Wait until thread is finished
  au::Cronometer c;
  while (true) {
    if (!pthread_running_) {
      return;
    }

    if (c.seconds() > 2) {
      LOG_SW(("Too much time waiting for thread '%s'", name_.c_str()));
      c.Reset();
    }

    usleep(100000);
  }
}

void Thread::JoinThread() {
  au::TokenTaker tt(&token_);

  while (true) {
    if (!pthread_running_) {
      return;     // It is not running, it is not necessary to wait
    }
    tt.Stop();
  }
}
}