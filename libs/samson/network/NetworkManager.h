


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

namespace samson {
    

    class NetworkConnection;
    class SocketConnection;
    class NetworkListener;
    
    class NetworkManager 
    {

    protected:
        
        // All managed connection
        au::map<std::string , NetworkConnection> connections;
        
    public:
        
        NetworkManager()
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

        
    };
    
    
    
}

#endif