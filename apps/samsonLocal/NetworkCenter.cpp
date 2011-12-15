

#include "NetworkCenter.h"	// Own interface

namespace samson
{
	void* runNetworkFakeCenter(void*p)
	{
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());
        
		((NetworkFakeCenter*)p)->run();		
		return NULL;
	}

	void NetworkFakeCenter::runInBackground()
	{
		pthread_t t;
		pthread_create( &t , 0 , runNetworkFakeCenter , this );	
	}

}