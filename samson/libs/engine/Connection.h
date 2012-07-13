

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "logMsg/logMsg.h"

#ifndef _H_ENGINE_CONNECTION
#define _H_ENGINE_CONNECTION

namespace engine 
{
    
    class Connection
    {
        
        au::Token token_; // Mutex protection for multuple threads
                

        bool locked_;          // Flag to indicate that this connection is locked
        pthread_t thread_id_;  // Identifier of the locking thread ( debugging )
        
    public:

        Connection()
        {
            locked_ = false;
        }
        
        // Check if a read/write operation is needed
        // Non blocking method
        // Instance response
        virtual bool check( )=0;
        
        // Performs io operation
        // Non blocking method
        // Non instance response
        virtual void process( )=0;
      

        // Lock unlock mechanism for this connection
        bool lock()
        {
            au::TokenTaker tt(&token_);
            if( locked_ )
                return false;
            
            locked_ = true;
            thread_id_ = pthread_self();
            return true;
        }
        
        void unlock()
        {
            au::TokenTaker tt(&token_);
            
            if( !locked_ )
            {
                LM_W(("Unlocking a non locked engine::Connection" ));
                return;
            }
            
            if( thread_id_ != pthread_self() )
            {
                LM_W(("Unlocking a engine::Connection locked by another thread" ));
                return;
            }
            
            locked_ = false;
        }
        
        bool is_locked()
        {
            au::TokenTaker tt(&token_);
            return locked_;
        }
        
    };
    
}

#endif