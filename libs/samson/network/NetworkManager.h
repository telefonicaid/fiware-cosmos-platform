


#ifndef _H_SAMSON_NETWORK_MANAGER
#define _H_SAMSON_NETWORK_MANAGER

#include "au/Token.h"
#include "au/TokenTaker.h"

#include "au/map.h"
#include "tables/Table.h"

#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/network/ClusterInformation.h"
#include "samson/network/Packet.h"
#include "samson/network/PacketQueue.h"
#include "samson/network/NetworkListener.h"

namespace samson {
    

    class NetworkConnection;
    class SocketConnection;
    class NetworkListener;
    
    class NetworkManager : public NetworkListenerInterface
    {

        // Pending packets ( only used while disconnected )
        au::map<std::string , PacketQueue> packet_queues;
        au::Token token_packet_queues;

    protected:
        
        // All managed connection
        au::map<std::string , NetworkConnection> connections;
        
    public:
        
        NetworkManager() : token_packet_queues("token_packet_queues")
        {
        }
        
        // notifications from Engine
        void notify( engine::Notification* notification );

        // Main interface to inform avout a new connection comming from a listener
        virtual void newSocketConnection( NetworkListener* listener , SocketConnection * socket_connetion )
        {
            LM_W(("NetworkManager::newSocketConnection not implemented"));
        }
        
        // Interface to inform about a received packet from a network connection
        virtual void receive( NetworkConnection* connection, Packet* packet )
        {
            LM_W(("NetworkManager::receive not implemented"));
        }

        // Add a network connection
        Status add( NetworkConnection * network_connection );
        
        // Mode a connection to another name
        Status move_connection( std::string connection_from , std::string connection_to );

        // Get table with connection information
        au::tables::Table * getConnectionsTable();

        // Get pending packets table
        au::tables::Table * getPendingPacketsTable();
        
        // Pending packets
        void push_pending_packet( std::string name , Packet * packet )
        {
            au::TokenTaker tt(&token_packet_queues);
            packet_queues.findOrCreate(name)->push(packet);                
        }

        void push_pending_packet( std::string name , PacketQueue * packet_queue )
        {
            au::TokenTaker tt(&token_packet_queues);
            PacketQueue * target_paquet_queue = packet_queues.findOrCreate(name);                

            while( true )
            {
                Packet * packet = packet_queue->extract();
                if( !packet )
                    return;
                
                target_paquet_queue->push(packet);
                
            }
        }

        void pop_pending_packet( std::string name , PacketQueue * packet_queue )
        {
            //LM_W(("Popping pending packets for connection %s" , name.c_str()));
            
            au::TokenTaker tt(&token_packet_queues);
            PacketQueue * source_paquet_queue = packet_queues.extractFromMap(name);                
            
            if( !source_paquet_queue )
                return; // No pending packets
            
            while( true )
            {
                Packet * packet = source_paquet_queue->extract();
                if( !packet )
                    break;
                
                packet_queue->push(packet);
            }
            
            // remove the original paquet queue
            delete source_paquet_queue;
            
        }        
        
    };
    
    
    
}

#endif