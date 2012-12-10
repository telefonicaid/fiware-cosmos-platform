#ifndef _H_AU_THREAD
#define _H_AU_THREAD

#include <string>

#include "au/mutex/Token.h"

namespace au {

  /**
   * \brief Class to encapsulate a thread
   
   The thread itself is implemented in the "RunThread" method
   Method IsThreadQuiting() should be periodically checked inside RunThread() to return if required
   */
  
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

    /**
     * \brief Start background thread if it is not already running
     */
    void StartThread();
    
    /**
     * \brief Stop background thread is still running
     */
    void StopThread();
    
    /**
     * \brief Wait for background thead to finish blocking calling threads
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
    bool IsThreadQuiting() const {
      return stoping_;
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
     * \brief MEthod called by real background thread ( to call RunThread )
     */
    friend void *run_Thread(void *p);

    /**
     * \brief Main function of the background thread
     */
    virtual void RunThread() = 0;
    
    std::string name_;        /**< Name of this thread for debugging */
    pthread_t t_;             /**< Identifier of backgroud thread */
    bool pthread_running_;    /**< Flag to indicate background thread is really running */
    bool stoping_;            /**< Flag to indicate background thread to stop */
    au::Token token_;         /**< Mutex to stop threads until background thread is finished */
  };
}

#endif