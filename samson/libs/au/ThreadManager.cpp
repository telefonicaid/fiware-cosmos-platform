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


#include "ThreadManager.h" // Own interface


namespace au {

    ThreadManager thread_manager;
    
    ThreadManager * ThreadManager::shared()
    {
        return &thread_manager;
    }
    
    void ThreadManager::wait_all_threads(std::string title)
    {
        thread_manager.wait(title);
    }

    
    ThreadManager::ThreadManager() : token("ThreadManager")
    {
        
    }
        
    int ThreadManager::addThread( std::string thread_name , pthread_t * __restrict t  , const pthread_attr_t * __restrict attr_t, thread_function f , void * __restrict p )
    {
        // Mutex protection
        au::TokenTaker tt( &token );
        
        // Create a thread info structure
        ThreadInfo* thread_info = new ThreadInfo( this , thread_name , f , p );
        
        // Create the thread with our "run_ThreadInfo" function
        int s = pthread_create( t , attr_t , run_ThreadInfo , thread_info );

        // Record the thread id
        thread_info->t = *t;
        
        if( s == 0 )
        {
            LM_T(LmtThreadManager, ("Thread '%s' created and inserted in map", thread_name.c_str()));
            threads.insertInMap( thread_info->t , thread_info );
        }
        else
        {
            LM_W(("Not possible to create thread %s %d " , thread_name.c_str(), s));
            delete thread_info;
        }
        
        return s;
        
    }
    
    void ThreadManager::notify_finish_thread( ThreadInfo* thread_info )
    {
        // Mutex protection
        au::TokenTaker tt( &token );
        
        LM_T(LmtThreadManager,("Thread '%s' extracted from map", thread_info->name.c_str()));

        if ( threads.extractFromMap( thread_info->t ) == NULL )
            LM_X(1,("Error in ThreadManager")); 
    }
    
    int ThreadManager::getNumThreads()
    {
        // Mutex protection
        au::TokenTaker tt( &token );
        
        return threads.size();
    }
    
    au::StringVector ThreadManager::getThreadNames()
    {
        // Mutex protection
        au::TokenTaker tt( &token );

        au::StringVector names;
        
        au::map< pthread_t , ThreadInfo >::iterator it_threads;
        for ( it_threads = threads.begin() ; it_threads != threads.end() ; it_threads++ )
            names.push_back( it_threads->second->name );
        
        return names;
    }

    std::string ThreadManager::str()
    {
        // Mutex protection
        au::TokenTaker tt( &token );
        return _str();
    }
    
    std::string ThreadManager::_str()
    {
        std::ostringstream output;
        output << "Running threads\n";
        output << "------------------------------------\n";
        au::map< pthread_t , ThreadInfo >::iterator it_threads;
        for ( it_threads = threads.begin() ; it_threads != threads.end() ; it_threads++ )
            output << it_threads->second->str() << "\n";
        
        return output.str();
    }
    
    void ThreadManager::wait( std::string title )
    {
        //std::cerr << "Waiting all threads to finish\n";
        
        au::Cronometer c;
        while( true )
        {
            {
                au::TokenTaker tt(&token);
                
                if( threads.size() == 0 )
                {
                    //std::cerr << "All threads finished\n";
                    return;
                }
                
                if( threads.size() == 1 )
                    if( threads.begin()->second->t == pthread_self() )
                    {
                        //std::cerr << "All threads finished\n";
                        return;
                    }
                
                if( c.diffTimeInSeconds() > 5 )
                {
                    LM_W(("%s: Waiting all threads to finish", title.c_str()));
                    LM_W(("Stll running %d threads", threads.size()));
                    LM_W(("%s", _str().c_str()));

                    std::cerr << title << ": Waiting all threads to finish\n";
                    std::cerr << _str() << "\n";
                    std::cerr << au::str( "Still running %d threads ...\n" , threads.size() );

                    c.reset();
                }
                
            }
            
            usleep(100000);
            
        }
        
    }

    // --------------------------------------------------------------------------------
    // Function to run a thread info
    // --------------------------------------------------------------------------------
    
    void* run_ThreadInfo( void* p )
    {
        
        // Detach myself.... noone is waiting for me...
        pthread_detach(pthread_self());
        
        ThreadInfo* thread_info = (ThreadInfo*) p;

        LM_VV(("Running thread %s" , thread_info->name.c_str() ));
        
        // Execute the real function
        void* ans = thread_info->f( thread_info->p );
        
        //Notify my ThreadRunner
        thread_info->thread_manager->notify_finish_thread( thread_info );

        LM_VV(("Finished thread %s" , thread_info->name.c_str() ));
        
        // Delete this structure
        delete thread_info;
        
        return ans;
    }

    
    void* run_Thread( void* p )
    {
        Thread* t = (Thread*)p;
        t->run();
        t->pthread_running = false;
        return NULL;
    }

    
    
}
