
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
  std::string str();

  // Singleton access handy function
  static ThreadManager *shared() {
    return au::Singleton<au::ThreadManager>::shared();
  }

private:

  // "Name" of the running threads
  ThreadInfo *threads_[AU_MAX_NUM_THREADS];

  void AddThread(ThreadInfo *thread_info);
  void RemoveThread(ThreadInfo *thread_info);

  // Mutex protection
  au::Token token_;
};


// Class to encapsulate a thread
//
// The thread itself is implemented in the "run" method
// Method thread_should_quit() should be periodically checked inside run() to return from "run"


class Thread {
public:

  explicit Thread(const std::string& name) {
    name_ = name;
    stoping_ = false;
    pthread_running_ = false;
  }

  ~Thread() {
    StopThread();  // make sure background thread is stoped
  }

  // Main functions to control this thread ( from outputside the thread )
  void StartThread();
  void StopThread();

  // Method to block this thread until background thread is finisd
  void JoinThread();

  // Overload this method to implement whatever is necessary to unlock background thread
  virtual void UnlockThread() {
  };

  bool IsThreadQuiting() const {
    return stoping_;
  }

  bool IsThreadRunning() const {
    return pthread_running_;
  }

  /**
   * \brief Check if I am the background thread
   */

  bool IsBackgroundThread() const {
    if (!pthread_running_) {
      return false;
    }
    return pthread_equal(pthread_self(), t_);
  }

private:

  // Main function of the thread to be overloaded
  virtual void RunThread() = 0;

  std::string name_;        // Name of this thread for debugging
  pthread_t t_;             // Identifier of backgroud thread
  bool pthread_running_;    // Flag to indicate background thread is really running
  bool stoping_;            // Flag to indicate background thread to stop

  friend void *run_Thread(void *p);

  au::Token token_;  // Mutex to stop threads until background thread is finished
};
}

#endif  // SAMSON_LIBS_AU_THREADMANAGER_H_
