
#ifndef _H_AU_STOPPER
#define _H_AU_STOPPER

#include <pthread.h>             /* pthread_mutex_t                          */

namespace au
{
	
	class Stopper
	{
		int num_threads_stopped;		// Counter of elements stopped here
		
		pthread_mutex_t _lock;			// Mutex to protect this token
		pthread_cond_t _condition;		// Condition to block threads that did not get the token
		
	public:
		
		Stopper();
		~Stopper();
		
        
        // One step stop operation
		void stop( int max_seconds );

        // Two steps stop to protect operation before stop
        void stop_begin();
        void stop_finish( int max_seconds );    // Time in seconds
		
        // Wake up operations
		void wakeUpAll();
		void wakeUp();
		
        
        // Simple lock and unlock
        void lock();
        void unlock();
        
	};
	
}
#endif
