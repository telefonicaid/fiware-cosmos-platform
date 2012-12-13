
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

/*
 * FILE            ThreadManager
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Classes to controll current threads working for this app.
 */

#ifndef SAMSON_LIBS_AU_THREADMANAGER_H_
#define SAMSON_LIBS_AU_THREADMANAGER_H_

#include <pthread.h>
#include <unistd.h>

#include <list>         // std::list
#include <map>          // std::map
#include <set>          // std::set
#include <string>       //  std::string
#include <vector>       // std::vector

#include "au/containers/StringVector.h"
#include "au/containers/map.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/singleton/Singleton.h"
#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"

#define AU_MAX_NUM_THREADS 100

namespace au {
typedef void * (*thread_function)(void *p);
void *run_ThreadInfo(void *p);
void *run_NonDetachedThreadInfo(void *p);

class ThreadManager;

// Information about a thread
class ThreadInfo {
public:

  ThreadInfo(const std::string& name, thread_function f, void *__restrict p) {
    name_ = name;
    f_ = f;
    p_ = p;
  }

  std::string str() {
    return au::str("%s:%s", name_.c_str(), cronometer_.str().c_str());
  }

private:

  au::Cronometer cronometer_;        // Cronometer for thsi thread
  std::string name_;                 // Name of this thread
  pthread_t t_;                      // Id of this thread

  // Real thread information to call this thread
  thread_function f_;                // Function to be executed
  void *__restrict p_;               // Parameter to be passed

  // Function to run a thread info
  friend void *run_ThreadInfo(void *p);
  friend void *run_NonDetachedThreadInfo(void *p);
  friend class ThreadManager;
};

// ------------------------------------------------------------
// Manager to controls all running threads
// ------------------------------------------------------------

class ThreadManager {
  ThreadManager();
  friend class au::Singleton<au::ThreadManager>;

public:

  static void wait_all_threads(std::string title);

  // Add a thread to the manager
  int AddThread(
    std::string thread_name,
    pthread_t *__restrict t,
    const pthread_attr_t *__restrict attr_t,
    thread_function f,
    void *__restrict p
    );

  // Add a thread to the manager
  int AddNonDetachedThread(
    std::string thread_name,
    pthread_t *__restrict t,
    const pthread_attr_t *__restrict attr_t,
    thread_function f,
    void *__restrict p
    );


  // Get name of all running threads
  au::StringVector getThreadNames();

  // Wait for all threads to finish ( except the calling thread )
  void wait(std::string title);

  // Internal function used to notify that a particular threads has finished
  void notify_finish_thread(ThreadInfo *thread_info);

  // Get the number of active threads
  int num_threads() const;

  // Debug string
  std::string str() const;

  // Debug string
  std::string str_table() const;

  // Singleton access handy function
  static ThreadManager *shared() {
    return au::Singleton<au::ThreadManager>::shared();
  }

private:

  // Mutex protection
  mutable au::Token token_;

  // "Name" of the running threads
  ThreadInfo *threads_[AU_MAX_NUM_THREADS];

  void AddThread(ThreadInfo *thread_info);
  void RemoveThread(ThreadInfo *thread_info);
};
}

#endif  // SAMSON_LIBS_AU_THREADMANAGER_H_
