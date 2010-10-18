#include "NetworkFakeCenter.h"       // NetworkFakeCenter
#include "NetworkFake.h"             // Own interface



namespace ss {

	NetworkFake::NetworkFake( int _worker_id , NetworkFakeCenter *_center )	// -1 controller -2 dalila
	{
		worker_id = _worker_id;
		center = _center;
	}
	
	
	 void NetworkFake::initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> peers)
	{
		
	}
	
	 void NetworkFake::initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint)
	{
		
	}
	
	 void NetworkFake::initAsDelilah(Endpoint controllerEndpoint)
	{
		
	}
	
	// Set the receiver element
	 void NetworkFake::setPacketReceiverInterface( PacketReceiverInterface* _receiver)
	{
		receiver = _receiver;
	}
	
	 bool NetworkFake::ready()
	{
		return true;
	}
	
	 Endpoint* NetworkFake::meGet()
	{
		if( worker_id == -1)
			return center->controllerEndPoint;
		if( worker_id == -2)
			return center->dalilahEndPoint;
		
		return center->workerEndPoint[ worker_id ];
		
	}
	 Endpoint* NetworkFake::controllerGet()
	{
		return center->controllerEndPoint;
	}
	
	 Endpoint* NetworkFake::workerGet(int i)
	{
		return center->workerEndPoint[ i ];
	}
	
	 int NetworkFake::worker( Endpoint endPoint )
	{
		assert(false);	// Not implemented
		return -1;
	}
	
	 std::vector<Endpoint> NetworkFake::endPoints()
	{
		assert(false);	// Not implemented
		std::vector<Endpoint> a;
		return a;
	}
	
	 std::vector<Endpoint> NetworkFake::samsonWorkerEndpoints()
	{
		assert(false);	// Not implemented
		std::vector<Endpoint> a;
		return a;
	}
	
	 size_t NetworkFake::send(Packet* packet, Endpoint* endpoint, PacketSenderInterface* sender)
	{
		return 0;
	}
	
	 void NetworkFake::run()
	{
	}
	
	 void NetworkFake::quit()
	{
	}

}
