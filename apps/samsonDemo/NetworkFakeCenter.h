#ifndef _H_NETWORK_FAKE_CENTER
#define _H_NETWORK_FAKE_CENTER

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "FakeEndpoint.h"		// ss::EndPoint

namespace ss {
	
	
	class NetworkFakeCenter
	{
	public:
		
		std::vector<NetworkFake*> workers;
		NetworkFake *controller;
		NetworkFake *dalilah;
		
		std::vector<FakeEndpoint*> workerEndPoint;
		FakeEndpoint *controllerEndPoint;
		FakeEndpoint *dalilahEndPoint;
		
		NetworkFakeCenter( int num_workers )
		{
			for (int i = 0 ; i < num_workers ; i++)
			{
				// Create a fake Network interface element
				workers.push_back( new NetworkFake(i , this) );
				
				// Create the rigth endpoint
				std::ostringstream _name;
				_name << "worker_" << i;
				workerEndPoint.push_back( new FakeEndpoint(_name.str() ) );
			}
			
			controllerEndPoint = new FakeEndpoint("controller");
			controller = new NetworkFake(-1 , this);
			
			dalilahEndPoint = new FakeEndpoint("dalilah");
			dalilah = new NetworkFake(-2 , this);
			
		}
		
	};
}

#endif
