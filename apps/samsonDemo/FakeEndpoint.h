
#ifndef _H_FAKE_ENDPOINT
#define _H_FAKE_ENDPOINT

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include "Endpoint.h"			// ss::EndPoint

namespace ss {
	
	// Coordinator of the fake network simulation
	
	class FakeEndpoint : public Endpoint
	{
		
	public:
		
		std::string name;
		
		FakeEndpoint( std::string _name ) : Endpoint( Listener , "" )
		{
			name = _name;
		}
		
	};

}

#endif