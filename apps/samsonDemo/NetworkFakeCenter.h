#ifndef _H_NETWORK_FAKE_CENTER
#define _H_NETWORK_FAKE_CENTER

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "FakeEndpoint.h"		// ss::EndPoint
#include "NetworkFake.h"        // NetworkFake


namespace ss {
	
	class NetworkFakeCenter
	{
	public:
		
		std::map<int,NetworkFake*> network; 
		std::map<int,FakeEndpoint*> endpoint; 
		
		NetworkFakeCenter( int num_workers )
		{
			for (int i = 0 ; i < num_workers ; i++)
			{
				// Create a fake Network interface element
				network.insert( std::pair<int,NetworkFake*>( i , new NetworkFake(i , this) ) );
				
				// Create the right endpoint
				endpoint.insert( std::pair<int,FakeEndpoint*>( i , new FakeEndpoint("worker" , i ) ) );
			}

			network.insert( std::pair<int,NetworkFake*>( -1 , new NetworkFake(-1 , this) ) );
			endpoint.insert( std::pair<int,FakeEndpoint*>( -1 , new FakeEndpoint("controller" , -1 ) ) );

			network.insert( std::pair<int,NetworkFake*>( -2 , new NetworkFake(-2 , this) ) );
			endpoint.insert( std::pair<int,FakeEndpoint*>( -2 , new FakeEndpoint("dalilah" , -2 ) ) );
			
		}
		
		
		NetworkFake* getNetwork( int worker_id )
		{
			std::map<int,NetworkFake*>::iterator i =  network.find( worker_id );
			assert( i != network.end() );
			return i->second;
		}
		
		FakeEndpoint* getEndpoint( int worker_id )
		{
			std::map<int,FakeEndpoint*>::iterator i =  endpoint.find( worker_id );
			assert( i != endpoint.end() );
			return i->second;
		}
		
		
		
	};
}

#endif
