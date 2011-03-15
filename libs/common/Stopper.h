
#ifndef _H_AU_STOPPER
#define _H_AU_STOPPER

#include <pthread.h>             /* pthread_mutex_t                          */

namespace au
{
	
	class Stopper
	{
		int num_threads_stopped;		// Counter of elements stopped here
		
		pthread_mutex_t _lock;			// Mutex to protect this tocken
		pthread_cond_t _condition;		// Condition to block threads that did not get the tocken
		
	public:
		
		Stopper();
		~Stopper();
		
		void stop();
		void stop( int max_seconds );
		
		void wakeUpAll();
		void wakeUp();
		
	};
	
}
#endif
