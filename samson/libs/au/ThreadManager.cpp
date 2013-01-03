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

#include "ThreadManager.h"  // Own interface

#include <iomanip>
#include <iostream>

#include "au/Log.h"
#include "au/log/LogMain.h"
#include "au/string/S.h"


namespace au {
void ThreadManager::wait_all_threads(std::string title) {
  au::Singleton<au::ThreadManager>::shared()->wait(title);
}

ThreadManager::ThreadManager() : token_("ThreadManager") {
  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    threads_[i] = NULL;
  }
}

int ThreadManager::AddThread(std::string thread_name, pthread_t *__restrict t, const pthread_attr_t *__restrict attr_t,
                             thread_function f,
                             void *__restrict p) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Create a thread info structure
  ThreadInfo *thread_info = new ThreadInfo(thread_name, f, p);

  // Create the thread with our "run_ThreadInfo" function
  int s = pthread_create(t, attr_t, run_ThreadInfo, thread_info);

  // Record the thread id
  thread_info->t_ = *t;

  if (s == 0) {
    LOG_V(logs.thread_manager, ("Thread '%s' created and inserted in map", thread_name.c_str()));
    AddThread(thread_info);
  } else {
    LOG_SW(("Not possible to create thread %s %d ", thread_name.c_str(), s));
    delete thread_info;
  }

  return s;
}

int ThreadManager::AddNonDetachedThread(std::string thread_name
                                        , pthread_t *__restrict t
                                        , const pthread_attr_t *__restrict attr_t
                                        , thread_function f
                                        , void *__restrict p) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Create a thread info structure
  ThreadInfo *thread_info = new ThreadInfo(thread_name, f, p);

  // Create the thread with our "run_ThreadInfo" function
  int s = pthread_create(t, attr_t, run_NonDetachedThreadInfo, thread_info);

  // Record the thread id
  thread_info->t_ = *t;

  if (s == 0) {
    LOG_V(logs.thread_manager, ("Thread '%s' created and inserted in map", thread_name.c_str()));
    AddThread(thread_info);
  } else {
    LOG_SW(("Not possible to create thread %s %d ", thread_name.c_str(), s));
    delete thread_info;
  }

  return s;
}

int ThreadManager::num_threads() const {
  int total = 0;

  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    if (threads_[i] != NULL) {
      total++;
    }
  }
  return total;
}

void ThreadManager::notify_finish_thread(ThreadInfo *thread_info) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  RemoveThread(thread_info);
}

au::StringVector ThreadManager::getThreadNames() {
  // Mutex protection
  au::TokenTaker tt(&token_);

  au::StringVector names;

  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    if (threads_[i] != NULL) {
      names.push_back(threads_[i]->name_);
    }
  }
  return names;
}

std::string ThreadManager::str() const {
  // Mutex protection
  au::TokenTaker tt(&token_);


  std::ostringstream o;

  o << "Running threads\n";
  o << "------------------------------------\n";
  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    if (threads_[i] != NULL) {
      o << threads_[i]->name_ << " " << threads_[i]->cronometer_ << "\n";
    }
  }

  return o.str();
}

std::string ThreadManager::str_table() const {
  // Mutex protection
  au::TokenTaker tt(&token_);

  au::tables::Table table("Name,left|time,left");

  table.setTitle("Running threads");

  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    if (threads_[i] != NULL) {
      table.addRow(au::StringVector(threads_[i]->name_, threads_[i]->cronometer_.str()));
    }
  }
  return table.str();
}

void ThreadManager::wait(std::string title) {
  // std::cerr << "Waiting all threads to finish\n";

  au::Cronometer cronometer;

  while (true) {
    {
      au::TokenTaker tt(&token_);

      if (num_threads() == 0) {
        // std::cerr << "All threads finished\n";
        return;
      }

      if (cronometer.seconds() > 5) {
        LOG_SW(("%s: Waiting all threads to finish", title.c_str()));
        LOG_SW(("Stll running %d threads", num_threads()));
        LOG_SW((str().c_str()));
        cronometer.Reset();
      }
    }

    usleep(100000);
  }
}

void ThreadManager::AddThread(ThreadInfo *thread_info) {
  LOG_V(logs.thread_manager, ("Adding  Thread '%s'", thread_info->str().c_str()));

  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    if (threads_[i] == NULL) {
      threads_[i] = thread_info;
      return;
    }
  }

  LM_X(1, ("No space for more threads"));
}

void ThreadManager::RemoveThread(ThreadInfo *thread_info) {
  LOG_V(logs.thread_manager, ("Remove thead %s ", thread_info->str().c_str()));
  for (int i = 0; i < AU_MAX_NUM_THREADS; ++i) {
    if (threads_[i] == thread_info) {
      threads_[i] = NULL;
      return;
    }
  }

  LOG_W(logs.thread_manager, ("Not possible to remove thread. Possibly singleton has been reset"));
}

// --------------------------------------------------------------------------------
// Function to run a thread info
// --------------------------------------------------------------------------------

void *run_ThreadInfo(void *p) {
  // Detach myself.... no one is waiting for me...
  pthread_detach(pthread_self());

  ThreadInfo *thread_info = (ThreadInfo *)p;

  LOG_V(logs.thread_manager, ("Running thread %s", thread_info->name_.c_str()));

  // Execute the real function
  void *ans = thread_info->f_(thread_info->p_);

  // Notify my ThreadRunner
  au::Singleton<au::ThreadManager>::shared()->notify_finish_thread(thread_info);

  LOG_V(logs.thread_manager, ("Finished thread %s", thread_info->name_.c_str()));

  // Delete this structure
  delete thread_info;

  return ans;
}

void *run_NonDetachedThreadInfo(void *p) {
  ThreadInfo *thread_info = (ThreadInfo *)p;

  LOG_V(logs.thread_manager, ("Running thread %s", thread_info->name_.c_str()));

  // Execute the real function
  void *ans = thread_info->f_(thread_info->p_);

  // Notify my ThreadRunner
  au::Singleton<au::ThreadManager>::shared()->notify_finish_thread(thread_info);

  LOG_V(logs.thread_manager, ("Finished thread %s", thread_info->name_.c_str()));

  // Delete this structure
  delete thread_info;

  return ans;
}
}
