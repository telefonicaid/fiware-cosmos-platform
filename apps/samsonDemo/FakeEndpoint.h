#ifndef _H_FAKE_ENDPOINT
#define _H_FAKE_ENDPOINT

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "Endpoint.h"			// ss::EndPoint



namespace ss {
	
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
