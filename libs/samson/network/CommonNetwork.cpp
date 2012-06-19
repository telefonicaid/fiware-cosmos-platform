
#include "samson/common/MessagesOperations.h"

#include "samson/network/NetworkConnection.h"
#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/network/NetworkConnection.h"

#include "CommonNetwork.h" // Own interface

namespace samson {

    
    void CommonNetwork::notify( engine::Notification* notification )
    {
        
        LM_T( LmtNetworkConnection , ("Review common network..." ));
        
        if (notification == NULL)
            LM_D(("notification == NULL"));

        // Check pennding packets to be removed after 1 minute disconnected
        // -------------------------------------------------------------------------------------
        NetworkManager::check();
        
        // Check disconnected elements to be removed ( if possible no threads or pending packets )
        // ----------------------------------------------------------------------------------------
        NetworkConnection * connection = NetworkManager::extractNextDisconnectedConnection();
        while (connection) 
        {

            // Some notification about this disconnection
            NodeIdentifier node_identifier = connection->getNodeIdentifier();

            if( node_identifier.node_type == WorkerNode )
            {
                LM_T( LmtNetworkConnection , ("report_worker_disconnected WorkerNode:%lu", node_identifier.id ));
                report_worker_disconnected( node_identifier.id );
            }
            
            if( node_identifier.node_type == DelilahNode )
            {
                LM_T( LmtNetworkConnection , ("report_worker_disconnected DelilahNode:%lu", node_identifier.id ));
                report_delilah_disconnected( node_identifier.id );
            }
            
            
            // Remove connection
            delete connection;
            
            // Scan the next connection
            connection = NetworkManager::extractNextDisconnectedConnection();
        }
        
        
        // Check workers ( review connections , create new connections if possible , .... ) 
        // -------------------------------------------------------------------------------------
        
        if( cluster_information.getId() != 0 )
        {
            
            // Get a duplicate of current nodes involved in this cluster to connect to all of them
            au::vector<ClusterNode> nodes = cluster_information.getNodesToConnect( node_identifier );

            for ( size_t i = 0 ; i < nodes.size() ; i++ )
            {
                std::string name = NodeIdentifier( WorkerNode , nodes[i]->id ).getCodeName();
                
                if ( !NetworkManager::isConnected(name) )
                {
                    
                    // Connect with this worker....
                    std::string host = nodes[i]->host;
                    int port = nodes[i]->port;
                    size_t worker_id = nodes[i]->id;

                    LM_T( LmtNetworkConnection , ("Worker %lu ( %s ) not connected. Trying to connect to %s:%d..."
                                                  , worker_id
                                                  , name.c_str()
                                                  , host.c_str()
                                                  , port
                                                  ));
                    
                    
                    addWorkerConnection(worker_id, host, port);
                }
                
            }
            
            // Remove nodes
            nodes.clearVector();
            
        }
        
        // -------------------------------------------------------------------------------------
        
        
        
        
        
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
            if( NetworkManager::isConnected(connection_name ) )
                connected_workers.push_back( workers[i] );
        }
        
        return connected_workers;
    }
    

    
    std::string CommonNetwork::str()
    {
        std::ostringstream output;
        
        output << "-----------------------------------------------\n";
        output << "NetworkManager (" << node_identifier.str() <<  ") \n";
        output << "-----------------------------------------------\n";

        output << NetworkManager::str();
        
        output << "-----------------------------------------------\n";
        
        return output.str();
    }
    
    // Add a new connection agains an additional worker to be added to the cluster
    Status CommonNetwork::addWorkerConnection( size_t worker_id , std::string host , int port )
    {
        NodeIdentifier node_identifier = NodeIdentifier( WorkerNode , worker_id );
        std::string name = node_identifier.getCodeName();

        // Check if we already have this connection
        if( NetworkManager::isConnected(name) )
            return Error;
        
        // Init connection
        au::SocketConnection* socket_connection;
        au::Status s = au::SocketConnection::newSocketConnection(host , port , &socket_connection );        

        // If there is an error, just return the error
        if( s != au::OK )
            return Error; // Generic error
        
        // Create network connection with this socket
        NetworkConnection* network_connection = new NetworkConnection( name , socket_connection , this );
        
        // Set identifier since we know this worker id
        network_connection->setNodeIdentifier( node_identifier );

        // Sent hello packages rigth now to make sure if identity us
        Packet *hello_packet = helloMessage( network_connection );
        network_connection->push( hello_packet );
        hello_packet->release();

        // Add this new connection
        Status ss = NetworkManager::add( network_connection );
        if( ss != OK )
        {
            LM_W(("Error adding new connection for worker %s" , name.c_str() ));
            delete network_connection;
            return ss;
        }

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
        // Add more info to buffer name for debuggin
        engine::Buffer * buffer = packet->getBuffer();
        if( buffer )
            buffer->addToName( au::str(" [in packet to %s]" , packet->to.str().c_str() ));
        
        //LM_W(("Sending packet %s to %s" ,  packet->str().c_str() , packet->to.str().c_str() ));
        
        if ( packet->to == node_identifier )
        {
            // Local loop
            packet->from = node_identifier;
            schedule_receive( packet );
            return OK;
        }
        
        // Push a packet to a connection or eventually keep in queue to see if it connects back soon ;)
        return NetworkManager::send( packet );
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

        if( network_interface_receiver )
        {
            // Flush previous packets for me ( if any )
            while( pending_packets_for_me.getNumPackets() > 0 )
            {
                // Process next packet
                Packet * previous_packet = pending_packets_for_me.next();
                schedule_receive( previous_packet );
                pending_packets_for_me.pop();
            }

            // Schedule the new packet
            schedule_receive( packet );
        }
        else
            pending_packets_for_me.push( packet );
        
    }
    
    void CommonNetwork::getInfo( ::std::ostringstream& output , std::string command , std::string format)
    {
        if( command == "main" )
        {
            // Do something
        }        
        if( command == "cluster" )
           cluster_information.getInfo( output , format);
        else
        {
            if (format == "xml")
                output << "<error>Unknown network element " << command << "</error>\n";
            else
                output << "  \"error\" : \"unknown network element: " << command << "\"\r\n";
        }
    }
    
    void CommonNetwork::report_worker_connected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_connected_worker_id(id);
        schedule_receive( packet );
        packet->release();
    }
    
    void CommonNetwork::report_worker_disconnected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        packet->message->mutable_network_notification()->set_disconnected_worker_id(id);
        schedule_receive( packet );
        packet->release();
    }
    
    void CommonNetwork::report_delilah_connected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        LM_M(("report_delilah_connected() id:%lu", id));
        packet->message->mutable_network_notification()->set_connected_delilah_id(id);
        schedule_receive( packet );
        packet->release();
    }
    
    void CommonNetwork::report_delilah_disconnected( size_t id )
    {
        Packet * packet = new Packet( Message::NetworkNotification );
        LM_M(("report_delilah_disconnected() id:%lu", id));
        packet->message->mutable_network_notification()->set_disconnected_delilah_id(id);
        schedule_receive( packet );
        packet->release();
    }
    
    NodeIdentifier CommonNetwork::getMynodeIdentifier()
    {
        return node_identifier;
    }
    
    std::string CommonNetwork::getHostForWorker(size_t worker_id)
    {
        return cluster_information.hostForWorker( worker_id );
    }
    
    unsigned short CommonNetwork::getPortForWorker(size_t worker_id)
    {
        return cluster_information.portForWorker( worker_id );
    }
    
    au::tables::Table* CommonNetwork::getClusterConnectionsTable()
    {
        au::tables::Table* table = new au::tables::Table( au::StringVector( "Worker" , "Host" , "Status"  ) );
        
        if ( cluster_information.getId() == 0 )
            table->setTitle("Not connected to any cluster");
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
                    values.push_back("me");
                else
                    values.push_back( NetworkManager::getStatusForConnection(connection_name) );
                
                table->addRow( values );
                
            }
            
            std::string title = au::str("Cluster %lu ( version %lu )" 
                                        , cluster_information.getId()
                                        , cluster_information.getVersion()
                                        );
            table->setTitle( title );
            nodes.clearVector();
        }
        
        return table;
        
    }

    std::vector<size_t> CommonNetwork::getDelilahIds()
    {
        return NetworkManager::getDelilahIds();
    }
    network::Collection* CommonNetwork::getConnectionsCollection( Visualization* visualization )
    {
        return NetworkManager::getConnectionsCollection(visualization);
    }
    size_t CommonNetwork::get_rate_in()
    {
        return NetworkManager::get_rate_in();
    }
    size_t CommonNetwork::get_rate_out()
    {
        return NetworkManager::get_rate_out();
    }
    

    
}
