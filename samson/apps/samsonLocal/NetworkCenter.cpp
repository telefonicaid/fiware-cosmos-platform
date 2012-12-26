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

#include "au/ThreadManager.h"

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
        au::ThreadManager::shared()->addThread("NetworkFakeCenter::runInBackground", &t , 0 , runNetworkFakeCenter , this );	
	}

}