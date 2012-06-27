#include "NetworkCenter.h"       // NetworkFakeCenter
#include "NetworkFake.h"             // Own interface



namespace samson {

	NetworkFake::NetworkFake( NodeIdentifier _node_identifier , NetworkFakeCenter *_center )	
	{
        node_identifier = _node_identifier;
		center = _center;
	}

	// Send a packet (return a unique id to inform the notifier later)
    Status NetworkFake::send( Packet* packet )
	{
        // Inform about where it comes from
        packet->from = node_identifier;
        
		// Add packet in the list of the center
        center->addPacket( packet );
        LM_T(LmtNetworkInterface, ("Added packet %p to %s", packet, packet->to.str().c_str()));


		return OK;
	}

	 void NetworkFake::quit()
	{
		// Nothing to do here
	}
    
    std::vector<size_t> NetworkFake::getWorkerIds()
    {
        std::vector<size_t> ids;
        for ( size_t i = 0 ; i < center->workers_network_interface.size() ; i ++ )
            ids.push_back( center->workers_network_interface[i]->node_identifier.id  );
        return ids;
    }
    
    std::vector<size_t> NetworkFake::getDelilahIds()
    {
        std::vector<size_t> ids;
        if ( node_identifier.node_type == WorkerNode )
            ids.push_back( DELILAH_ID ); // Only worker has delilahs in the list of elements
        
        return ids;
    }

    
    
}
