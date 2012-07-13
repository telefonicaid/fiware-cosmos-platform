


#ifndef _H_SAMSON_NETWORK_MANAGER
#define _H_SAMSON_NETWORK_MANAGER

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/containers/map.h"
#include "au/tables/Table.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/Object.h"

#include "samson/network/ClusterInformation.h"
#include "samson/network/Packet.h"
#include "samson/network/PacketQueue.h"
#include "au/network/NetworkListener.h"


namespace samson {
    

    class NetworkConnection;
    class SocketConnection;
    class NetworkListener;
    
        
    
    class NetworkManager
    {
        // Multi queue for all unconnected connections
        MultiPacketQueue multi_packet_queue;

        // All managed connection
        au::map<std::string , NetworkConnection> connections;

        // Token to block add and move operations on connections
        au::Token token_connections_;

    public:
        
        NetworkManager():token_connections_("token_connections_")
        {
        }
                
        ~NetworkManager()
        {
            multi_packet_queue.clear();
        }
        
        // Interface to inform about a received packet from a network connection
        virtual void receive( NetworkConnection* connection, Packet* packet )
        {
            LM_W(("NetworkManager::receive not implemented"));
        }
        
        // Push a packet to a connection
        Status send( Packet* packet );

        // Add a network connection
        Status add( NetworkConnection * network_connection );
        
        // Mode a connection to another name
        Status move_connection( std::string connection_from , std::string connection_to );

        // Check connection
        bool isConnected( std::string connection_name );
        
        // Extract next unconnected network connection ... to be deleted
        NetworkConnection* extractNextDisconnectedConnection(  );
        
        // simply remove all unconnected connections
        void remove_disconnected_connections();
        
        // Get table with connection information
        au::tables::Table * getConnectionsTable();

        // Get pending packets table
        au::tables::Table * getPendingPacketsTable();
        
        // API to access internal multi packet queue
        void check();
        void push_pending_packet( std::string name , PacketQueue * packet_queue );

        // Get delilah ids
        std::vector<size_t> getDelilahIds();

        // Debug str
        std::string str();
        
        // Get a collection for all connections
        network::Collection* getConnectionsCollection( Visualization* visualization );
       
        // Reset all connections
        void reset();
        
        // Get number of connections
        size_t getNumConnections();
        
        size_t get_rate_in();
        size_t get_rate_out();
        std::string getStatusForConnection( std::string connection_name );
    };
    
    
    
}

#endif
