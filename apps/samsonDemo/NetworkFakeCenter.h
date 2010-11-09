#ifndef _H_NETWORK_FAKE_CENTER
#define _H_NETWORK_FAKE_CENTER

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "FakeEndpoint.h"		// ss::EndPoint
#include "NetworkFake.h"        // NetworkFake
#include "Packet.h"				// ss::Packet
#include <list>					// std::list
#include "traces.h"				// LMT_FAKE_NETWORK
#include <iostream>				// std::cout

namespace ss {
	
	
	class NetworkFakeCenterPacket
	{
	public:

		Message::MessageCode msgCode;
		Packet packet; 
		int from;
		int to;
		PacketSenderInterface* sender;
		
		NetworkFakeCenterPacket(Message::MessageCode _msgCode, Packet _packet, int _from , int _to , PacketSenderInterface* _sender )
		{
			msgCode = _msgCode;
			from = _from;
			to = _to;
			packet = _packet;
			sender = _sender;
		}
								 
	};
	
	class NetworkFakeCenter
	{
	public:
		
		std::map<int,NetworkFake*> network; 
		std::map<int,FakeEndpoint*> endpoint; 

		au::Lock lock; // Lock to protect the pending packet list
		std::vector<NetworkFakeCenterPacket*> pendingPackets;
		
		int num_workers;
		
		NetworkFakeCenter( int _num_workers )
		{
			num_workers = _num_workers;
			
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
		
		
		void run(bool *finish)
		{
			while( !(*finish) )
			{
				// Send packets to the rigth directions
		
				
				lock.lock();
				
				std::vector<NetworkFakeCenterPacket*> sendingPackets;
				sendingPackets.insert( sendingPackets.end() , pendingPackets.begin() , pendingPackets.end() );
				pendingPackets.clear();
				
				lock.unlock();
				
				if( sendingPackets.size() > 0)
				
				for (std::vector<NetworkFakeCenterPacket*>::iterator p = sendingPackets.begin() ; p < sendingPackets.end() ; p++)
				{
					NetworkFakeCenterPacket* pp = *p;
					processPendingPacket(pp);
					delete pp;
				}
				
				sleep(1);
			}
		}
		
		void addPacket(NetworkFakeCenterPacket *p)
		{
			lock.lock();
			pendingPackets.push_back(p);
			lock.unlock();
		}
		
		
		void processPendingPacket(NetworkFakeCenterPacket *p)
		{
			// We look the endpoint worker id and use that to send the packet
			// NetworkFake* network = getNetwork(p->to);
			// network->receiver->receive( ... );

			//LM_X(1, ("This call to receive must be reimplemented after last changes by KZ. Sorry ... !"));
			
			NetworkFake* network = getNetwork( p->to );
			network->receiver->receive( p->from, p->msgCode, &p->packet );			

		}
	};
}

#endif
