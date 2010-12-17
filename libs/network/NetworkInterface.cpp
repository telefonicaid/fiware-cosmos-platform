
#include "NetworkInterface.h"	// Own interface

namespace ss
{
	
	void* run_network_interface_background_thread(void *p)
	{
		((NetworkInterface*)p)->run();
		return NULL;
	}
	
	void NetworkInterface::runInBackground()
	{
		// Run a differente thread with the "run" method
		pthread_t t;
		pthread_create(&t, NULL, run_network_interface_background_thread, this);
		
	}
	
}