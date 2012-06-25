
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
        static void wait_all_threads(std::string title);
        
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
        void wait( std::string title );
        
        // Internal function used to notify that a particular threads has finished
        void notify_finish_thread( ThreadInfo* thread_info );

                     
    private:
        std::string _str();

        
    };
    

    // Funciton to execute the Thread::run()
    void* run_Thread( void* p );
    
    
    // Class encapsulating a thread
    // 
    // This thread is implemented in method run()
    //
    // Method run should return "quickly" after method cancel_thread is executed in a paralel thread
    // Method thread_should_quit() can be executed inside run() to check if I have to finish
    
    class Thread
    {
        pthread_t t;
        bool pthread_running;
        bool stoping; // Flag to indicate 
        
        friend void* run_Thread( void* p );
        
        std::string name_;
        
    public:
        
        Thread( std::string name )
        {
            // Default values
            stoping = false;
            pthread_running = false;
            name_ = name;
            
        }
        void start_thread()
        {
            if( pthread_running )
                return; // Already running

            // Mark as running
            pthread_running = true;
            
            // Run the thread in background
            au::ThreadManager::shared()->addThread(name_, &t, NULL, run_Thread,this);
        }
        
        virtual void run()=0; // Main function of the thread to be overloaded
        virtual void cancel_thread(){}; // Paralel cancel function ( to wake up the thread for instance )
        
        void stop_thread()
        {
            stoping = true;
            if( !pthread_running )
                return;
            
            if ( pthread_self() == t )
            {
                LM_W(("Not possible to stop a thread from itself"));
                return;
            }
            
            // Set the flag
            stoping = true;
            
            // Execute cutom cancel cunfion
            cancel_thread();
            
            // Wait until thread is finished
            au::Cronometer c;
            while ( true )
            {
                if( !pthread_running )
                    return;
                
                if( c.diffTimeInSeconds() > 2 )
                {
                    LM_W(("Too mush time waiting for thread %s" , name_.c_str() ));
                    c.reset();
                }
                
                usleep(100000);
            }
        }
        
        bool thread_should_quit()
        {
            return stoping;
        }
        
        bool isRunning()
        {
            return pthread_running;
        }
        
    };
    
    
}

#endif