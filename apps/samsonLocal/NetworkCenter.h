#ifndef _H_NETWORK_FAKE_CENTER
#define _H_NETWORK_FAKE_CENTER

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include <list>					// std::list
#include <iostream>				// std::cout

#include "au/Token.h"				// au::Token
#include "au/TokenTaker.h"                          // au::TokenTake

#include "samson/network/Packet.h"				// samson::Packet
#include "samson/delilah/Delilah.h"			// ss:Delilah
#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker

#include "FakeEndpoint.h"		// samson::EndPoint
#include "NetworkFake.h"        // NetworkFake

namespace samson {
	
	/**
	 Center of operation of the NetworkFakeCenter
	 */
	
	class NetworkFakeCenter
	{
        
	public:
		

        // Lock to protect the pending packet list
        au::Token token;			

        // List of network interfaces
        NetworkFake* delilah_network_interface;
        std::vector<NetworkFake*> workers_network_interface;
		
        // List of pending packets
		std::vector<Packet*> pendingPackets; 
		
		int num_workers;
		
		NetworkFakeCenter( int _num_workers ) : token("NetworkFakeCenter")
		{
			num_workers = _num_workers;
			
			for (int i = 0 ; i < num_workers ; i++)
			{
                NodeIdentifier node_identifier( WorkerNode , 10 + i );
                
				// Create a fake Network interface element
				workers_network_interface.push_back( new NetworkFake( node_identifier , this) );
				
			}
            
            // Delilah client
            NodeIdentifier delilah_node_identifier( DelilahNode , 0 );
            delilah_network_interface =  new NetworkFake( delilah_node_identifier , this );
			
		}
		
		~NetworkFakeCenter()
		{
			// Clear everything
            for ( size_t i = 0 ; i < workers_network_interface.size() ; i ++ )
                delete workers_network_interface[i];
            workers_network_interface.clear();
            
            delete delilah_network_interface;

			// Just in case there are pending packets
			for ( size_t i = 0 ; i < pendingPackets.size() ; i++ )
				delete pendingPackets[i];
			pendingPackets.clear();
			
		}

		NetworkFake* getNetworkForDelilah( )
        {
            return delilah_network_interface;
        }
        
		NetworkFake* getNetworkForWorker( int i )
        {
            if( (i<0)||(i>=(int)workers_network_interface.size() ) )
                LM_X(1, ("Error in NetworkFake"));
            return workers_network_interface[i];
        }
        
		NetworkFake* getNetwork( NodeIdentifier node_identifier )
		{
            if( node_identifier.node_type == DelilahNode )
            {
                if ( node_identifier.id != 0 )
                    LM_X(1,("Error in fake network"));
                
                return delilah_network_interface;
            }
            
            if( node_identifier.node_type == WorkerNode )
            {
                for ( size_t i = 0 ; i < workers_network_interface.size() ; i++ )
                    if( node_identifier == workers_network_interface[i]->node_identifier )
                        return workers_network_interface[i];
                
                     LM_X(1,("Error in fake network"));
                return NULL;
            }
            
            LM_X(1,("Error in fake network"));
            return NULL;
		}
		void runInBackground();
		
		void run()
		{
			while( true )
			{
				// Send packets to the rigth directions
                std::vector<Packet*> sendingPackets;
                
                {
                    // Protect retaining the token
                    au::TokenTaker tt( &token );
                    
                    sendingPackets.insert( sendingPackets.end() , pendingPackets.begin() , pendingPackets.end() );
                    pendingPackets.clear();
                    
                }
				
				if( sendingPackets.size() > 0 )
				{
					
					for (std::vector<Packet*>::iterator p = sendingPackets.begin() ; p < sendingPackets.end() ; p++)
					{
						Packet* packet = *p;
						processPendingPacket( packet );
					}
				}
				else
                {
                    au::TokenTaker tt(&token);
                    tt.stop();
                }
			}
		}
		
		void addPacket( Packet *p)
		{
            au::TokenTaker tt( &token );

			pendingPackets.push_back(p);

            
			// Wake up the background thread to process this packages
			tt.wakeUpAll();
			
		}
		
		
		void processPendingPacket( Packet *packet )
		{
			NetworkFake* network = getNetwork( packet->to );
            
			network->network_interface_receiver->schedule_receive( packet );			
		}
	};
}

#endif
