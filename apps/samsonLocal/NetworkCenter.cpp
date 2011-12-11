

#include "NetworkCenter.h"	// Own interface

namespace samson
{
	void* runNetworkFakeCenter(void*p)
	{
		((NetworkFakeCenter*)p)->run();		
		return NULL;
	}

	void NetworkFakeCenter::runInBackground()
	{
		pthread_t t;
		pthread_create( &t , 0 , runNetworkFakeCenter , this );	
	}

}