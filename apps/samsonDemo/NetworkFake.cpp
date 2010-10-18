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
		// Nothing to do
	}
	
	 void NetworkFake::initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint)
	{
		// Nothing to do
	}
	
	 void NetworkFake::initAsDelilah(Endpoint controllerEndpoint)
	{
		// Nothing to do
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
		return center->getEndpoint(worker_id);
	}
	 Endpoint* NetworkFake::controllerGet()
	{
		return center->getEndpoint(-1);
	}
	
	 Endpoint* NetworkFake::workerGet(int i)
	{
		return center->getEndpoint(i);
	}
	
	 int NetworkFake::worker( Endpoint endPoint )
	{
		assert( false );
		return 0;
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
		
		// Add packet in the list of the center

		center->addPacket(new NetworkFakeCenterPacket( *packet, meGet() , endpoint , sender ) );

/*		
		// We look the endpoint worker id and use that to send the packet
		FakeEndpoint *e = (FakeEndpoint*) endpoint;
		NetworkFake* network = center->getNetwork( e->worker_id  );

		// Send to the other side
		network->receiver->receive(packet, meGet() );
*/		
		
		return 0;
	}
	
	 void NetworkFake::run()
	{
		// Nothing to do here
	}
	
	 void NetworkFake::quit()
	{
		// Nothing to do here
	}

}
