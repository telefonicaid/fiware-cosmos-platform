#ifndef _H_NETWORK_FAKE
#define _H_NETWORK_FAKE

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "samson/common/status.h"
#include "samson/network/Message.h"         // samson::Message::MessageCode
#include "samson/network/Packet.h"   
#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker
#include "samson/delilah/Delilah.h"			// ss:Delilah

namespace samson {
	
	class NetworkFakeCenter;
	
	class NetworkFake : public NetworkInterface
	{
	public:
		
		NetworkFakeCenter *center;
        
        // My identification
        NodeIdentifier node_identifier;
		
		NetworkFake( NodeIdentifier node_identifier , NetworkFakeCenter *_center );
		
		// Send a packet (return a unique id to inform the notifier later)
        Status send( Packet* packet );
				
		// Syspend the network interface, close everything and return the "run" call
		virtual void quit();	
        
        virtual std::vector<size_t> getWorkerIds();
        
        virtual std::vector<size_t> getDelilahIds();
        
        virtual NodeIdentifier getMynodeIdentifier()
        {
            return node_identifier;
        }
        
	};
	
}


#endif
