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

#ifndef _H_AU_THREAD
#define _H_AU_THREAD

#include <string>

#include "au/mutex/Token.h"

namespace au {
/**
 * \brief Class to encapsulate a thread
 *
 * The thread itself is implemented in the "RunThread" method
 * Method IsThreadQuitting() should be periodically checked inside RunThread() to return if required
 */

class Thread {
public:

  explicit Thread(const std::string& name) {
    name_ = name;
    stopping_ = false;
    pthread_running_ = false;
  }

  ~Thread() {
    StopThread();    // make sure background thread is stoped
  }

  /**
   * \brief Start background thread, unless it's running already
   */
  void StartThread();

  /**
   * \brief Stop background thread if still running
   */
  void StopThread();

  /**
   * \brief Wait for background thread to finish blocking calling threads
   */
  void JoinThread();

  /**
   * \brief Overload this method to implement whatever is necessary to unlock background thread
   */
  virtual void UnlockThread() {
  };

  /**
   * \brief Check if background thread should finish ( as soon as possible )
   */
  bool IsThreadQuitting() const {
    return stopping_;
  }

  /**
   * \brief Check if background thread is still running
   */
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

  /**
   * \brief Method called by real background thread ( to call RunThread )
   */
  friend void *run_Thread(void *p);

  /**
   * \brief Main function of the background thread
   */
  virtual void RunThread() = 0;

  std::string name_;          /**< Name of this thread for debugging */
  pthread_t t_;               /**< Identifier of backgroud thread */
  bool pthread_running_;      /**< Flag to indicate background thread is really running */
  bool stopping_;             /**< Flag to indicate background thread to stop */
  au::Token token_;           /**< Mutex to stop threads until background thread is finished */
};
}

#endif  // ifndef _H_AU_THREAD