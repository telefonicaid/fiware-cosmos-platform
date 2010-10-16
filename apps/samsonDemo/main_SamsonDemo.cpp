
#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include "Endpoint.h"			// ss::EndPoint

#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkFakeCenter.h"
#include "NetworkInterface.h"

#define NUM_WORKERS	2


int main(int argc, const char *argv[])
{
	ss::Network network;							// Real network interface element

	ss::NetworkFakeCenter center(NUM_WORKERS);		// Fake network element with 2 workers
	
	ss::SamsonController controller ( argc, argv ,center.controller  );
	ss::Delilah delilah( argc, argv , center.dalilah  );

	std::vector< ss::SamsonWorker* > workers;
	for (int i = 0 ; i < NUM_WORKERS ; i ++ )
		workers.push_back( new ss::SamsonWorker( argc , argv , center.workers[i] ) );
	
	controller.run();
	delilah.run();

	for (int i = 0 ; i < NUM_WORKERS ; i ++ )
		workers[i]->run();
	
	
						  
}
