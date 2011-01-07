
#ifndef _H_AU_TOCKEN
#define _H_AU_TOCKEN

#include <pthread.h>             /* pthread_mutex_t                          */
#include <assert.h>              /* assert                                   */



namespace au
{

	class Token
	{
	  
		bool taken;						// Flag to indicate if the token is taken

		pthread_mutex_t _lock;			// Mutex to protect this tocken
		pthread_cond_t _condition;		// Condition to block threads that did not get the tocken
	  
	 public:
	  
		Token();
		~Token();
	  
		void retain();
		void release();
	  
	};
	
}
#endif
