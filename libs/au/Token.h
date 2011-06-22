
#ifndef _H_AU_TOCKEN
#define _H_AU_TOCKEN

#include <pthread.h>             /* pthread_mutex_t                          */

namespace au
{

	class Token
	{
        const char * name;
        
		bool taken;						// Flag to indicate if the token is taken

		pthread_mutex_t _lock;			// Mutex to protect this tocken
		pthread_cond_t _condition;		// Condition to block threads that did not get the tocken
	  
	 public:
	  
		Token( const char * name );
		~Token();
	  
		void retain();
		void release();
	  
	};
	
    // Class to hold the lock during the life-span
    class TokenTaker
    {
        Token* token;
    public:
        
        TokenTaker( Token* _token )
        {
            token = _token;
            token->retain();
        }
        
        ~TokenTaker()
        {
            token->release();
        }
    };
    
    
}
#endif
