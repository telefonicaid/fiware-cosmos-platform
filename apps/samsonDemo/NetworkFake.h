
#ifndef _H_NETWORK_FAKE
#define _H_NETWORK_FAKE

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include "Endpoint.h"			// ss::EndPoint

namespace ss {
	
	class NetworkFakeCenter;
	
	class NetworkFake : public NetworkInterface
	{
	public:
		
		// Pointer to the controller
		NetworkFakeCenter *center;
		int worker_id;							// To know who is me
		PacketReceiverInterface *receiver;
		
		NetworkFake( int _worker_id , NetworkFakeCenter *_center );
		
		virtual void initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> peers);
		virtual void initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint);
		virtual void initAsDelilah(Endpoint controllerEndpoint);
		virtual void setPacketReceiverInterface( PacketReceiverInterface* _receiver);
		virtual bool ready();
		virtual Endpoint* meGet();
		virtual Endpoint* controllerGet();
		virtual Endpoint* workerGet(int i);
		virtual int worker( Endpoint endPoint );
		virtual std::vector<Endpoint> endPoints();
		virtual std::vector<Endpoint> samsonWorkerEndpoints();
		virtual size_t send(Packet* packet, Endpoint* endpoint, PacketSenderInterface* sender);
		virtual void run();
		virtual void quit();
		
	};
	
}


#endif
