
/* ****************************************************************************
 *
 * FILE            ThreadController
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
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_THREAD_CONTROLLER
#define _H_AU_THREAD_CONTROLLER

#include <map>		// std::map
#include <list>		// std::list
#include <vector>	// std::vector
#include <set>		// std::set
#include <pthread.h>

#include "au/string.h"
#include "au/Cronometer.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "logMsg/logMsg.h"					 // LM_M()

#include "au/containers/map.h"
#include "au/containers/StringVector.h"

namespace au {
  
  typedef void* (* thread_function)(void* p);		
  void* run_ThreadInfo( void* p );
  void* run_NonDetachedThreadInfo( void* p );

  class ThreadManager;
  
  // Information about a thread
  class ThreadInfo
  {
    
  public:
    
    ThreadInfo( const std::string& name , thread_function f , void * __restrict p  )
    {
      name_ = name;
      f_ = f;
      p_ = p;
    }
    
  private:
    
    friend std::ostream& operator<<( std::ostream& o , const ThreadInfo& thread_info );
  
    au::Cronometer cronometer_;      // Cronometer for thsi thread
    std::string name_;               // Name of this thread
    pthread_t t_;                    // Id of this thread

    // Real thread information to call this thread
    thread_function f_;            // Function to be executed
    void * __restrict p_;          // Parameter to be passed        

    // Function to run a thread info
    friend void* run_ThreadInfo( void* p );
    friend void* run_NonDetachedThreadInfo( void* p );
    friend class ThreadManager;
  };
  
  // ------------------------------------------------------------
  // Manager to controls all running threads
  // ------------------------------------------------------------
  
  class ThreadManager
  {
    
    ThreadManager();
    
  public:
    
    static ThreadManager * shared();
    static void wait_all_threads(std::string title);
    
    // Add a thread to the manager
    int addThread( 
                  std::string thread_name , 
                  pthread_t * __restrict t  , 
                  const pthread_attr_t * __restrict attr_t, 
                  thread_function f , 
                  void * __restrict p 
                  );

    // Add a thread to the manager
    int addNonDetachedThread(
                             std::string thread_name ,
                             pthread_t * __restrict t  ,
                             const pthread_attr_t * __restrict attr_t,
                             thread_function f ,
                             void * __restrict p
                             );
    
    
    
    // Get the number of running threads
    int getNumThreads();
    
    // Get name of all running threads
    au::StringVector getThreadNames();
    
    // Wait for all threads to finish ( except the calling thread )
    void wait( std::string title );
    
    // Internal function used to notify that a particular threads has finished
    void notify_finish_thread( ThreadInfo* thread_info );
    
    
  private:
    
    friend std::ostream& operator<<( std::ostream& o , ThreadManager& thread_manager );
    
    // "Name" of the running threads
    au::map< pthread_t , ThreadInfo > threads_;
    
    // Mutex protection 
    au::Token token_;
    
    
  };
  
  
  // Class encapsulating a thread
  // 
  // This thread is implemented in method run()
  //
  // Method run should return "quickly" after method cancel_thread is executed in a paralel thread
  // Method thread_should_quit() can be executed inside run() to check if I have to finish
  
  void* run_Thread( void* p );
  
  class Thread
  {
    
  public:
    
    Thread( const std::string& name )
    {
      name_ = name;
      stoping_ = false;
      pthread_running_ = false;
    }
    void start_thread()
    {
      if( pthread_running_ )
        return; // Already running
      
      // Mark as running
      pthread_running_ = true;
      
      // Run the thread in background
      au::ThreadManager::shared()->addThread(name_, &t_, NULL, run_Thread,this);
    }
    
    virtual void run()=0; // Main function of the thread to be overloaded
    virtual void cancel_thread(){}; // Paralel cancel function ( to wake up the thread for instance )
    
    void stop_thread()
    {
      stoping_ = true;
      if( !pthread_running_ )
        return;
      
      if ( pthread_self() == t_ )
      {
        LM_W(("Not possible to stop a thread from itself"));
        return;
      }
      
      // Set the flag
      stoping_ = true;
      
      // Execute cutom cancel cunfion
      cancel_thread();
      
      // Wait until thread is finished
      au::Cronometer c;
      while ( true )
      {
        if( !pthread_running_ )
          return;
        
        if( c.seconds() > 2 )
        {
          LM_W(("Too mush time waiting for thread %s" , name_.c_str() ));
          c.Reset();
        }
        
        usleep(100000);
      }
    }
    
    bool thread_should_quit()
    {
      return stoping_;
    }
    
    bool isRunning()
    {
      return pthread_running_;
    }
    
  private:
    
    std::string name_;
    pthread_t t_;
    bool pthread_running_;
    bool stoping_; // Flag to indicate 
    
    friend void* run_Thread( void* p );
    
    
  };
  
  //
  // RepeateObjectCallThread
  //
  // Simple thread to repeatelly call a method over an object
  //
  // Example:  RepeateObjectCallThread<A,&A::run> t;
  
  template< class C , void (C::*f)() >
  class RepeateObjectCallThread : public Thread
  {
    C* object_;
    
  public:
    
    RepeateObjectCallThread( C* object )
    {
      object_ = object;
    }
    
    virtual void run()
    {
      while( true )
      {
        if( thread_should_quit() )
          return; // Quit this thread when necessary
        object_->f();
      }
    }
    
  };
  
  
}

#endif