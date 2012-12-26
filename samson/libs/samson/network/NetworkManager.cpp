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

#include "samson/network/NetworkConnection.h"
#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"
#include "NetworkManager.h" // Own interface

namespace samson {
    


    Status NetworkManager::move_connection( std::string connection_from , std::string connection_to )
    {
        au::TokenTaker tt(&token_connections_, "token_connections_.move_connection");

        if( connections.findInMap(connection_to) != NULL )
            return Error;
        
        NetworkConnection * network_connection = connections.extractFromMap( connection_from );
        
        if( !network_connection )
            return Error;
        
        network_connection->name = connection_to;
        connections.insertInMap(connection_to, network_connection);

        // recover pending packets if any...
        multi_packet_queue.pop_pending_packet( connection_to,  &network_connection->packet_queue );
        
        return OK;
    }
 
    Status NetworkManager::add( NetworkConnection * network_connection )
    {
        std::string name = network_connection->getName();
        LM_T(LmtNetworkConnection, ("Adding network_connection:%s", name.c_str()));

        au::TokenTaker tt(&token_connections_, "token_connections_.add");

        if( connections.findInMap( name ) != NULL )
        {
            LM_W(("network_connection:%s already connected", name.c_str()));
            return Error;
        }
        
        // Add to the map of connections
        connections.insertInMap( name , network_connection );
        LM_T(LmtNetworkConnection, ("Inserted in map network_connection:%s", name.c_str()));

        // recover pending packets if any...
        multi_packet_queue.pop_pending_packet( name,  &network_connection->packet_queue );
        
        // Init threads once included in the map
        network_connection->initReadWriteThreads();
        
        return OK;
    }

    size_t NetworkManager::getNumConnections()
    {
        au::TokenTaker tt(&token_connections_, "token_connections_.getNumConnections");
        return connections.size();
    }
    
    bool NetworkManager::isConnected( std::string connection_name )
    {
        au::TokenTaker tt(&token_connections_, "token_connections_.isConnected");

        LM_T(LmtNetworkConnection, ("Asked for connections for network_connection:%s", connection_name.c_str()));

        return (connections.findInMap(connection_name) != NULL);
    }
    
    au::tables::Table * NetworkManager::getConnectionsTable()
    {
        au::TokenTaker tt(&token_connections_, "token_connections_.getConnectionsTable");
        
        au::tables::Table* table = new au::tables::Table( au::StringVector( "Name" , "Host" , "In" , "Out" ) );
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            au::StringVector values;
            
            values.push_back( it_connections->first ); // Name of the connection

            NetworkConnection* connection = it_connections->second;
            au::SocketConnection* socket_connection = connection->socket_connection;
            values.push_back( socket_connection->getHostAndPort() );
            values.push_back( au::str( connection->get_rate_in() , "B/s" ) );
            values.push_back( au::str( connection->get_rate_out() , "B/s" ) );
            
            table->addRow( values );
            
        }
        
        table->setTitle("Connections");
        
        return table;
    }
    
    void NetworkManager::remove_disconnected_connections()
    {
        while (true) 
        {
            NetworkConnection* connection = extractNextDisconnectedConnection();
            if( connection )
            {
                delete connection;
            }
            else
                return;
        }
        
    }

    NetworkConnection* NetworkManager::extractNextDisconnectedConnection( )
    {
        au::TokenTaker tt(&token_connections_, "token_connections_.extractNextDisconnectedConnection");

        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            NetworkConnection * network_connection = it_connections->second;
            
            if( network_connection->isDisconnected() )
                if( network_connection->noThreadsRunning() )
                {
                    LM_T(LmtNetworkConnection, ("Removing connection '%s' because disconnected and no threads running", it_connections->first.c_str()));

                    connections.erase( it_connections );
                    return network_connection;
                }
        }
        return NULL; // No next unconnected
    }
    
    std::vector<size_t> NetworkManager::getDelilahIds()
    {
        // Return all connections with pattern delilah_X
        std::vector<size_t> ids;
        
        au::TokenTaker tt(&token_connections_);

        au::map<std::string , NetworkConnection>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            NodeIdentifier _node_identifier = it_connections->second->getNodeIdentifier();
            
            if( _node_identifier.node_type  == DelilahNode )
            {
                size_t id = _node_identifier.id;
                
                if( it_connections->first == _node_identifier.getCodeName() )
                {
                    // Add this id to the list
                    ids.push_back(id);
                }
                else
                    LM_W(("Delilah %lu (%s) connected using wrong connection name %s",
                          _node_identifier.id,
                          _node_identifier.getCodeName().c_str(),
                          it_connections->first.c_str()
                          ));
            }
        }
        
        return ids;
    }
    
    std::string NetworkManager::str()
    {
        au::TokenTaker tt(&token_connections_);

        std::ostringstream output;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++)
            output << it_connections->first << " : " << it_connections->second->str() << "\n";
        
        return output.str();
    }
    
    Status NetworkManager::send( Packet* packet )
    {
        au::TokenTaker tt(&token_connections_);

        // Recover connection name
        std::string name = packet->to.getCodeName();
        
        NetworkConnection* connection = connections.findInMap( name );
        
        if( !connection )
        {
            // Only messages to workers are saved to be sended when reconnecting
            if(  !packet->disposable && (packet->to.node_type == WorkerNode) )
            {
                // Save this messages appart
                multi_packet_queue.push_pending_packet( name , packet );
                return OK;
            }
            else
            {
                // Delilah messages, just discard them
                LM_W(("Packet %s destroyed since connection %s is not available" , packet->str().c_str(), name.c_str() ));
                return Error;
            }
        }
        
        connection->push( packet );   
        return OK;
    }

    network::Collection* NetworkManager::getConnectionsCollection( Visualization* visualization )
    {
        network::Collection* collection = new network::Collection();
        collection->set_name("connections");
        
        au::TokenTaker tt(&token_connections_);

        au::map<std::string , NetworkConnection>::iterator it_connections;
        
        for ( it_connections =connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            network::CollectionRecord* record = collection->add_record();            
            it_connections->second->fill( record , visualization );
            
        }
        
        return collection;
    }
    
    void NetworkManager::reset()
    {
        au::TokenTaker tt(&token_connections_);

        au::map<std::string , NetworkConnection>::iterator it_connections;
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            std::string name = it_connections->first;
            
            NetworkConnection * connection = it_connections->second;

            connection->close();

            // Setting the quitting flags to stop reader and writer threads
            LM_W(("Setting the quitting flags to stop reader and writer threads"));

            connection->quitting_t_reader = true;
            connection->quitting_t_writer = true;
            connection->setNodeIdentifier( NodeIdentifier(UnknownNode,-1) );
        }
        
        // We cannot wait for all connections to be disconnected because reset command is originated in a delilah connection
        
        
    }
    
    size_t NetworkManager::get_rate_in()
    {
        au::TokenTaker tt(&token_connections_);

        size_t total = 0;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += it_connections->second->get_rate_in();
        
        return total;
    }
    
    size_t NetworkManager::get_rate_out()
    {
        au::TokenTaker tt(&token_connections_);

        size_t total = 0;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += it_connections->second->get_rate_out();
        
        return total;
    }

    std::string NetworkManager::getStatusForConnection( std::string connection_name )
    {
        au::TokenTaker tt(&token_connections_);

        // Find this connection...
        NetworkConnection* connection = connections.findInMap( connection_name );
        
        if (!connection )
            return "Non connected";
        else if ( connection->isDisconnected() )
            return "Disconnecting";
        else
            return au::str( "Connected In: %s Out: %s " , au::str( connection->get_rate_in() , "B/s" ).c_str() , au::str( connection->get_rate_out() , "B/s" ).c_str() );
    }
    
    au::tables::Table * NetworkManager::getPendingPacketsTable()
    {
        return multi_packet_queue.getPendingPacketsTable();
    }

    void NetworkManager::check()
    {
        multi_packet_queue.check();
    }
    
    void NetworkManager::push_pending_packet( std::string name , PacketQueue * packet_queue )
    {
        multi_packet_queue.push_pending_packet(name, packet_queue );
    }

}
