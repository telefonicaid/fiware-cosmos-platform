
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
#include "au/StringVector.h"

namespace au {

    typedef void* (* thread_function)(void* p);		

    
    class ThreadManager;
    
    // Information about a thread
    class ThreadInfo
    {
        
        au::Cronometer cronometer;
        
    public:
        
        std::string name;               // Name of this thread
        
        pthread_t t;                    // Id of this thread
        
        ThreadManager* thread_manager;  // Pointer to my thread manager to notify finish

        // Real thread information to call this thread
        thread_function f;            // Function to be executed
        void * __restrict p;            // Parameter to be passed        
        
        ThreadInfo( ThreadManager* _thread_manager ,   std::string _name ,thread_function _f , void * __restrict _p  )
        {
            thread_manager = _thread_manager;
            name = _name;
            
            f = _f;
            p = _p;
        }
        
        std::string str()
        {
            return au::str("%60s %s"  , name.c_str() , cronometer.str().c_str() );
        }
        
    };
    
    // Function to run a thread info
    void* run_ThreadInfo( void* p );
    
    // ------------------------------------------------------------
    // Manager to controls all running threads
    // ------------------------------------------------------------
    
    class ThreadManager
    {
        
        // "Name" of the running threads
        au::map< pthread_t , ThreadInfo > threads;

        // Mutex protection 
        au::Token token;
                
    public:
        
        ThreadManager();
        
        static ThreadManager * shared();
        static void wait_all_threads();
        
        // Add a thread to the manager
        int addThread( 
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

        // Debug string
        std::string str();
        
        // Wait for all threads to finish ( except the calling thread )
        void wait();
        
        // Internal function used to notify that a particular threads has finished
        void notify_finish_thread( ThreadInfo* thread_info );

                     
    private:
        std::string _str();

        
    };
    
}

#endif