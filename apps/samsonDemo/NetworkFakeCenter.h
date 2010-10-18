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

namespace ss {
	
	
	class NetworkFakeCenterPacket
	{
	public:
		
		Packet packet; 
		Endpoint* me;
		Endpoint* endpoint;
		PacketSenderInterface* sender;
		
		NetworkFakeCenterPacket( 	Packet _packet,Endpoint*_me ,  Endpoint* _endpoint , PacketSenderInterface* _sender )
		{
			me = _me;
			endpoint = _endpoint;
			
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
		
		NetworkFakeCenter( int num_workers )
		{
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
			std::cout << "New packet\n";
			lock.lock();
			pendingPackets.push_back(p);
			lock.unlock();
		}
		
		
		void processPendingPacket(NetworkFakeCenterPacket *p)
		{
			std::cout << "Sending packet\n";
			// We look the endpoint worker id and use that to send the packet
			FakeEndpoint *e = (FakeEndpoint*) p->endpoint;
			NetworkFake* network = getNetwork( e->worker_id  );
			network->receiver->receive(&p->packet, p->me );
		}
		
		
	};
}

#endif
