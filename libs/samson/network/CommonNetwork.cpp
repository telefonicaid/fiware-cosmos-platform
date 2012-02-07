
#include "samson/network/NetworkConnection.h"
#include "samson/network/NetworkListener.h"
#include "samson/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"

#include "CommonNetwork.h" // Own interface

namespace samson {

    
    void CommonNetwork::notify( engine::Notification* notification )
    {
        // Print current status
        
        if( node_identifier.node_type == WorkerNode )
        {
            std::cerr << str() << "\n";
            std::cerr << cluster_information.str() << "\n";
        }

        // Check disconnected elements to be removed ( if possible no threads or pending packets )
        // -------------------------------------------------------------------------------------
        std::vector<std::string> to_be_removed;
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++)
        {
            NetworkConnection * network_connection = it_connections->second;
            
            if( network_connection->isDisconnected() )
                if( network_connection->noThreadsRunning() )
                {
                    // Add to be removed
                    to_be_removed.push_back( it_connections->first );

                    if( it_connections->second->getNodeIdentifier().node_type == WorkerNode )  
                    {
                        // Save packages...
                        
                    }
                    
                    NodeIdentifier node_identifier = network_connection->getNodeIdentifier();
                    
                    if( node_identifier.node_type == WorkerNode )
                        report_worker_disconnected( node_identifier.id );
                    
                    if( node_identifier.node_type == DelilahNode )
                        report_delilah_disconnected( node_identifier.id );
                    
                }
        }
        // -------------------------------------------------------------------------------------
        
        
        
        // Check workers ( review connections , create new connections if possible , .... ) 
        // -------------------------------------------------------------------------------------
        
        if( cluster_information.getId() != 0 )
        {
            // Get a duplicate of current nodes involved in this cluster to connect to all of them
            au::vector<ClusterNode> nodes = cluster_information.getNodesToConnect( node_identifier );
            
            for ( size_t i = 0 ; i < nodes.size() ; i++ )
            {
                
                std::string name = NodeIdentifier( WorkerNode , nodes[i]->id ).getCodeName();
                
                if ( connections.findInMap(name) == NULL )
                {
                    // Connect with this worker....
                    std::string host = nodes[i]->host;
                    int port = nodes[i]->port;
                    size_t worker_id = nodes[i]->id;
                    
                    addWorkerConnection(worker_id, host, port);
                }
                
                
            }
            
            // Remove nodes
            nodes.clearVector();
            
        }
        
        // -------------------------------------------------------------------------------------
        
        
        
        // Remove selected network_connections
        // -------------------------------------------------------------------------------------
        for ( size_t i = 0 ; i < to_be_removed.size() ; i++ )
        {
            NetworkConnection * network_connection = connections.extractFromMap(to_be_removed[i]);
            if( network_connection )
                delete network_connection;
        }
        
        
    }      
    
    
    std::vector<size_t> CommonNetwork::getWorkerIds()
    {
        return cluster_information.getWorkerIds();
    }
    
    std::vector<size_t> CommonNetwork::getDelilahIds()
    {
        // Return all connections with pattern delilah_X
        std::string prefix = "delilah_";
        
        std::vector<size_t> ids;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            std::string name = it_connections->first;
            
            if( name.substr(0,prefix.size()) == prefix )
                ids.push_back( atoll( name.substr(  prefix.size() ).c_str() ) );
        }
        
        return ids;
    }
    
    std::string CommonNetwork::str()
    {
        std::ostringstream output;
        
        output << "-----------------------------------------------\n";
        output << "NetworkManager (" << node_identifier.str() <<  ") \n";
        output << "-----------------------------------------------\n";
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++)
            output << it_connections->first << " : " << it_connections->second->str() << "\n";
        
        output << "-----------------------------------------------\n";
        
        return output.str();
    }
    
    // Add a new connection agains an additional worker to be added to the cluster
    Status CommonNetwork::addWorkerConnection( size_t worker_id , std::string host , int port )
    {
        NodeIdentifier node_identifier = NodeIdentifier( WorkerNode , worker_id );
        std::string name = node_identifier.getCodeName();

        // Check if we already have this connection
        NetworkConnection* previous_network_connection = connections.findInMap(name);
        if( previous_network_connection != NULL )
            return Error;
        
        // Init connection
        SocketConnection* socket_connection;
        Status s = SocketConnection::newSocketConnection(host , port , &socket_connection );        

        // If there is an error, just return the error
        if( s != OK )
            return s;
        
        // Create network connection with this socket
        NetworkConnection* network_connection = new NetworkConnection( name , socket_connection , this );
        
        network_connection->setNodeIdentifier( node_identifier );
        
        // Insert in the map of connections
        connections.insertInMap( name , network_connection );
        
        // Start the read and write threads for this connection
        network_connection->initReadWriteThreads();
        
        // Notify about this connection
        report_worker_connected( worker_id );
        
        return OK;
    }      
    
    Packet* CommonNetwork::helloMessage( NetworkConnection * target )
    {
        Packet* hello_packet = new Packet( Message::Hello ); 
        
        network::Hello* pb_hello =  hello_packet->message->mutable_hello();
        
        // Copy cluster information
        cluster_information.fill( pb_hello->mutable_cluster_information() );
        
        // Copy identification information
        node_identifier.fill( pb_hello->mutable_node_identifier() );
        
        if( !target )
            pb_hello->set_answer_hello_required(false);
        else
        {
            // Ask for Hello back if we still do not know who is the other...
            if(  target->getNodeIdentifier().node_type == UnknownNode )
                pb_hello->set_answer_hello_required(true);
            else
                pb_hello->set_answer_hello_required(false);
        }
        
        return hello_packet;
    }
    

    Status CommonNetwork::send( Packet* packet )
    {
        std::string name = packet->to.getCodeName();
        
        NetworkConnection* connection = connections.findInMap( name );
        
        if( !connection )
        {
            LM_W(("Packet destroyed since connection %s is not available" , name.c_str() ));
            if( packet->buffer )
                engine::MemoryManager::shared()->destroyBuffer(packet->buffer);
            delete packet;
            return Error;
        }
        
        connection->push( packet );
        return OK;
    }   

    void CommonNetwork::getInfo( ::std::ostringstream& output , std::string command )
    {
        if( command == "main" )
        {
            // Do something
        }        
        if( command == "cluster" )
            cluster_information.getInfo( output );
        else
            output << "<error>Unkown network element " << command << "</error>\n";
    }
    
    
    void CommonNetwork::resetNetworkManager()
    {
        std::vector<std::string> connection_names;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            connection_names.push_back( it_connections->first );
            NetworkConnection * connection = it_connections->second;
            connection->close();
            connection->setNodeIdentifier( NodeIdentifier(UnknownNode,-1) );
        }
        
        for ( size_t i = 0 ; i < connection_names.size() ; i++ )
            move_connection(connection_names[i], au::str("canceled_%lu", tmp_canceled_counter++ ));
        
        
    }
    
    void CommonNetwork::report_worker_connected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_connected_worker_id(id);
        network_interface_receiver->receive( packet );
    }
    
    void CommonNetwork::report_worker_disconnected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_disconnected_worker_id(id);
        network_interface_receiver->receive( packet );
    }
    
    void CommonNetwork::report_delilah_connected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_connected_worker_id(id);
        network_interface_receiver->receive( packet );
    }
    
    void CommonNetwork::report_delilah_disconnected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_disconnected_worker_id(id);
        network_interface_receiver->receive( packet );
    }
    
    
    
}