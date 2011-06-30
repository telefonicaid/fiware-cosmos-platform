
#ifndef _H_AU_TOCKEN
#define _H_AU_TOCKEN

#include <pthread.h>             /* pthread_mutex_t                          */

namespace au
{

	class Token
	{
        const char * name;              // Name of the token

		pthread_mutex_t _lock;			// Mutex to protect this tocken
		pthread_cond_t _block;          // Condition to block threads that call stop
        
	 public:
	  
		Token( const char * name );
		~Token();
	  
    private:

        // It is only possible to retain teh token with the class TokenTaker
        friend class TokenTaker;
        
		void retain();
		void release();
	  
	};
	
    // Class to hold the lock during the life-span
    
    class TokenTaker
    {
        const char* name;
        Token* token;
        
    public:
        
        TokenTaker( Token* _token );
        TokenTaker( Token* _token, const char* name );
        ~TokenTaker();
        
        void stop( int time_out );
        
        void wakeUp();
        void wakeUpAll();

        
        
    };
    
    
}
#endif
