#ifndef _H_FAKE_ENDPOINT
#define _H_FAKE_ENDPOINT

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "samson/delilah/Delilah.h"			// ss:Delilah
#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker
#include "samson/network/Endpoint.h"			// samson::EndPoint



namespace samson {
	
	// Coordinator of the fake network simulation
	
	class FakeEndpoint : public Endpoint
	{
		
	public:
		
		std::string name;
		int worker_id;
		
		FakeEndpoint( std::string _name , int _worker_id ) : Endpoint( Listener , 1234 )
		{
			worker_id = _worker_id;
			name = _name;
			
		}
		
	};

}

#endif
