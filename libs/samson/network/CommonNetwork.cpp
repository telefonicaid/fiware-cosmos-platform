
#include "samson/common/MessagesOperations.h"

#include "samson/network/NetworkConnection.h"
#include "samson/network/NetworkListener.h"
#include "samson/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"

#include "CommonNetwork.h" // Own interface

namespace samson {

    
    void CommonNetwork::notify( engine::Notification* notification )
    {
        if (notification == NULL)
            LM_D(("notification == NULL"));

        // Check pennding packets to be removed after 1 minute disconnected
        // -------------------------------------------------------------------------------------
        
        {
            au::TokenTaker tt(&token_packet_queues);
            au::map<std::string , PacketQueue>::iterator it_packet_queues;
            for( it_packet_queues = packet_queues.begin() ; it_packet_queues != packet_queues.end() ;  )
            {
                if( it_packet_queues->second->getSeconds() > 60 )
                {
                    std::string name = it_packet_queues->first;
                    LM_W(("Removing  pending packerts for %s since it has been disconnected mote thatn 60 secs",name.c_str()));
                    it_packet_queues->second->clear();   
                    packet_queues.erase( it_packet_queues++ );
                }
                else
                    ++it_packet_queues;
            }
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
    
    std::vector<size_t> CommonNetwork::getConnectedWorkerIds()
    {
        std::vector<size_t> workers = getWorkerIds();
        std::vector<size_t> connected_workers;

        for ( size_t i = 0 ; i < workers.size() ; i++ )
        {
            std::string connection_name = NodeIdentifier( WorkerNode , workers[i]).getCodeName();
            if( connections.findInMap(connection_name ) != NULL )
                connected_workers.push_back( workers[i] );
        }
        
        return connected_workers;
    }
    
    std::vector<size_t> CommonNetwork::getDelilahIds()
    {
        // Return all connections with pattern delilah_X
        std::vector<size_t> ids;
        
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
        
        // Set identifier since we know this worker id
        network_connection->setNodeIdentifier( node_identifier );
        
        // Insert in the map of connections
        connections.insertInMap( name , network_connection );
        

        // Sent hello packages rigth now to make sure if identity us
        network_connection->push( helloMessage( network_connection ) );
        
        // Recover pending packets
        pop_pending_packet(name, &network_connection->packet_queue);
        
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

        // Copy user and password
        pb_hello->set_user( user );
        pb_hello->set_password( password );
        
        // Copy connection type
        pb_hello->set_connection_type(connection_type);
        
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
        if( packet->buffer )
        {
            packet->buffer->addToName( au::str(" [in packet to %s]" , packet->to.str().c_str() ));
        }
        
        //LM_W(("Sending packet %s to %s" ,  packet->str().c_str() , packet->to.str().c_str() ));
        
        if ( packet->to == node_identifier )
        {
            // Local loop
            packet->from = node_identifier;
            network_interface_receiver->schedule_receive( packet );
            return OK;
        }
        
        // Get code name for the connection
        std::string name = packet->to.getCodeName();
        
        NetworkConnection* connection = connections.findInMap( name );
        
        if( !connection )
        {
            // Only messages to workers are saved to be sended when reconnecting
            std::string prefix = "worker_";
            if( ( name.substr( 0 , prefix.length() ) == prefix ) &&  !packet->disposable && (packet->to.node_type == WorkerNode) )
            {
                // Save this messages appart
                push_pending_packet( name , packet );
                return OK;
            }
            else
            {
                // Delilah messages, just discard them
                LM_W(("Packet %s destroyed since connection %s is not available" , packet->str().c_str(), name.c_str() ));
                if( packet->buffer )
                    engine::MemoryManager::shared()->destroyBuffer(packet->buffer);
                delete packet;
                return Error;
            }
        }
        
        connection->push( packet );
        return OK;
    }   
    
    // Receive a packet from a connection
    void CommonNetwork::receive( NetworkConnection* connection, Packet* packet )
    {
        
        if( packet->msgCode == Message::Hello )
        {
            processHello(connection, packet);
            return;
        }
        
        if( connection->getNodeIdentifier().node_type == UnknownNode )
            LM_X(1, ("Packet %s received from a non-identified node %s %s"
                     , packet->str().c_str()
                     , connection->getNodeIdentifier().str().c_str()
                     , connection->getName().c_str()
                     ));
        
        
        // Common interface to receive packets
        packet->from = connection->getNodeIdentifier();
        

        // Flush previous packets for me ( if any )
        if( network_interface_receiver )
        {
            while( pending_packets_for_me.size() > 0 )
            {
                Packet * previous_packet = pending_packets_for_me.front();
                pending_packets_for_me.pop_front();
                network_interface_receiver->schedule_receive( previous_packet );
            }

            // Schedule the new packet
            network_interface_receiver->schedule_receive( packet );
        }
        else
            pending_packets_for_me.push_back( packet );
        
    }

    network::Collection* CommonNetwork::getConnectionsCollection( Visualization* visualization )
    {
        network::Collection* collection = new network::Collection();
        collection->set_name("connections");
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        
        for ( it_connections =connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            network::CollectionRecord* record = collection->add_record();            
            it_connections->second->fill( record , visualization );
            
        }

        
        
        return collection;
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
        packet->message->mutable_network_notification()->set_connected_delilah_id(id);
        network_interface_receiver->receive( packet );
    }
    
    void CommonNetwork::report_delilah_disconnected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_disconnected_delilah_id(id);
        network_interface_receiver->receive( packet );
    }
    
    size_t CommonNetwork::get_rate_in()
    {
        size_t total = 0;

        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += it_connections->second->get_rate_in();
        
        return total;
    }
    
    size_t CommonNetwork::get_rate_out()
    {
        size_t total = 0;
        
        au::map<std::string , NetworkConnection>::iterator it_connections;
        for (it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += it_connections->second->get_rate_out();
        
        return total;
    }
    
    NodeIdentifier CommonNetwork::getMynodeIdentifier()
    {
        return node_identifier;
    }
    
    std::string CommonNetwork::getHostForWorker(size_t worker_id)
    {
        return cluster_information.hostForWorker( worker_id );
    }
    
    au::tables::Table* CommonNetwork::getClusterConnectionsTable()
    {
        au::tables::Table* table = new au::tables::Table( au::StringVector( "Worker" , "Host" , "Status" , "In" , "Out" ) );
        
        if ( cluster_information.getId() == 0 )
            table->setDefaultTitle("Not connected to any cluster");
        else
        {
            
            au::vector<ClusterNode> nodes = cluster_information.getNodes();
            
            for ( size_t i = 0 ; i < nodes.size() ; i++ )
            {
                au::StringVector values;
                values.push_back( au::str("%lu" , nodes[i]->id ) );
                values.push_back( au::str("%s:%d" , nodes[i]->host.c_str() , nodes[i]->port ) );
                
                NodeIdentifier ni = nodes[i]->getNodeIdentifier();
                std::string connection_name = ni.getCodeName();
                
                if( ni == node_identifier )
                {
                    values.push_back("me");
                    values.push_back("-");
                    values.push_back("-");
                }
                else
                {
                    // Find this connection...
                    NetworkConnection* connection = connections.findInMap( connection_name );
                    
                    if (!connection )
                    {
                        values.push_back("Not connected");
                        values.push_back("-");
                        values.push_back("-");
                    }
                    else
                    {
                        if ( connection->isDisconnected() )
                        {
                            values.push_back("Disconnected");
                            values.push_back("-");
                            values.push_back("-");
                        }
                        else
                        {
                            values.push_back("Connected");
                            values.push_back( au::str( connection->get_rate_in() , "B/s" ) );
                            values.push_back( au::str( connection->get_rate_out() , "B/s" ) );
                        }
                    }
                    
                }
                
                table->addRow( values );
                
            }
            
            std::string title = au::str("Cluster %lu ( version %lu )" 
                                        , cluster_information.getId()
                                        , cluster_information.getVersion()
                                        );
            table->setDefaultTitle( title );
            nodes.clearVector();
        }
        
        return table;
        
    }

    
}
