
#include "samson/network/NetworkConnection.h"
#include "samson/network/NetworkListener.h"
#include "samson/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"
#include "NetworkManager.h" // Own interface

namespace samson {
    


    Status NetworkManager::move_connection( std::string connection_from , std::string connection_to )
    {
        if( connections.findInMap(connection_to) != NULL )
            return Error;
        
        NetworkConnection * network_connection = connections.extractFromMap( connection_from );
        
        if( !network_connection )
            return Error;
        
        network_connection->name = connection_to;
        connections.insertInMap(connection_to, network_connection);
        
        return OK;
    }
 
    Status NetworkManager::add( NetworkConnection * network_connection )
    {
        std::string name = network_connection->getName();

        if( connections.findInMap( name ) != NULL )
            return Error;
        
        // Add to the map of connections
        connections.insertInMap( name , network_connection );
        
        // Init threads once included in the map
        network_connection->initReadWriteThreads();
        
        return OK;
    }
    
    au::tables::Table * NetworkManager::getConnectionsTable()
    {
        
        au::tables::Table * table = new au::tables::Table( au::StringVector( "Name" , "Status" ) );
        
        au::map<std::string , NetworkConnection>::iterator it_connections;

        
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            au::StringVector values;
            values.push_back( it_connections->first );
            
            NetworkConnection* connection = it_connections->second;
            values.push_back( connection->str() );
            
            table->addRow( values );
            
        }
        
        return table;
    }
    
}