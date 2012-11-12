/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_NETWORK_FAKE_CENTER
#define _H_NETWORK_FAKE_CENTER

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include <list>					// std::list
#include <iostream>				// std::cout

#include "au/mutex/Token.h"				// au::Token
#include "au/mutex/TokenTaker.h"                          // au::TokenTake

#include "samson/network/Packet.h"				// samson::Packet
#include "samson/delilah/Delilah.h"			// ss:Delilah
#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker

#include "FakeEndpoint.h"		// samson::EndPoint
#include "NetworkFake.h"        // NetworkFake

#define DELILAH_ID 724511525

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
            NodeIdentifier delilah_node_identifier( DelilahNode , DELILAH_ID );
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
            LM_T(LmtNetworkInterface, ("getNetwork for packet with node_intifier:%s", node_identifier.str().c_str()));

            if( node_identifier.node_type == DelilahNode )
            {
                if ( node_identifier.id != DELILAH_ID )
                {
                    LM_E(("Error in fake network for DelilahNode with node_identifier:%s", node_identifier.str().c_str()));
                    return NULL;
                }
                
                return delilah_network_interface;
            }
            
            if( node_identifier.node_type == WorkerNode )
            {
                for ( size_t i = 0 ; i < workers_network_interface.size() ; i++ )
                    if( node_identifier == workers_network_interface[i]->node_identifier )
                        return workers_network_interface[i];
                
                     LM_E(("Error in fake network after trying workers_network_interface[%d] with unknown node_identifier:%s", workers_network_interface.size(), node_identifier.str().c_str()));
                return NULL;
            }
            
            LM_E(("Error in fake network, unknown node_type(%d) in node_identifier (DelilahNode(%d), WorkerNode(%d))", node_identifier.node_type, DelilahNode, WorkerNode ));
            return NULL;
		}
		void runInBackground();
		
		void run()
		{
			while( true )
			{
				// Send packets to the right directions
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
						LM_T(LmtNetworkInterface, ("Sending to processing packet %p to %s", packet, packet->to.str().c_str()));
						processPendingPacket( packet );
						packet->release();
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

            p->retain();

			pendingPackets.push_back(p);

            
			// Wake up the background thread to process this packages
			tt.wakeUpAll();
			
		}
		
		void processPendingPacket( Packet *packet )
		{
            LM_T(LmtNetworkInterface, ("Processing packet %p to %s", packet, packet->to.str().c_str()));

			NetworkFake* network = getNetwork( packet->to );

			if (network == NULL)
			{
	            LM_E(("Error processing corrupted packet %p to %s", packet, packet->to.str().c_str()));

			}
			network->schedule_receive( packet );			
		}
	};
}

#endif
