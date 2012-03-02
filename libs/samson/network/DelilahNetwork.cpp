
#include "au/StringVector.h"
#include "au/utils.h"
#include "tables/Table.h"

#include "samson/common/ports.h"
#include "samson/network/NetworkConnection.h"

#include "DelilahNetwork.h" // Own interface


#define MAIN_DELILAH_CONNECTION_NAME       "delilah_main_connection"
#define SECONDARY_DELILAH_CONNECTION_NAME  "delilah_secondary_connection"

namespace samson {

    
    bool is_only_digits( std::string txt )
    {
        for ( size_t i = 0 ; i < txt.length() ; i++ )
        {
            char c = txt[i];
            
            if ( !( ( c >= 48) && ( c <= 57 ) ) )
                return false;
        }
        
        return true;
    }
    
    DelilahNetwork::DelilahNetwork( std::string _connection_type , size_t delilah_random_code )
    {
        // I am a delilah ( random id auto-asigned )
        node_identifier = NodeIdentifier( DelilahNode , delilah_random_code );
        
        // Save connection type string  to be send in all hello messages
        connection_type = _connection_type;
    }
    
    // Add new connection agains any worker in the cluster
    Status DelilahNetwork::addMainDelilahConnection( 
                                                    std::string _host 
                                                    , int _port 
                                                    , std::string _user 
                                                    , std::string _password
                                                    )
    {
        // Check previous connection....
        std::string connection_name = MAIN_DELILAH_CONNECTION_NAME;
        if( connections.findInMap(connection_name) != NULL )
            return Error;

        // Check not already connected to a cluster
        if( cluster_information.getId() != 0 )
            return Error;
        
        host = _host;
        port = _port;
        user = _user;
        password = _password;
        
        SocketConnection* socket_connection;
        Status s = SocketConnection::newSocketConnection(host , port , &socket_connection );        
        
        if( s != OK )
        {
            std::cerr << au::str("Error: Not possible to open connection with %s:%d (%s)\n" , host.c_str() , port , status(s));
            return s;
        }
        
        
        // Create network connection with this socket
        NetworkConnection * network_connection = new NetworkConnection( connection_name , socket_connection , this );
        
        // Insert in the map of connections
        connections.insertInMap( connection_name , network_connection );
        
        // Start the read and write threads for this connection
        network_connection->initReadWriteThreads();
     
        return OK;
    }

    // Add a new connection agains an additional worker to be added to the cluster
    std::string DelilahNetwork::addSecondaryDelilahConnection( std::string host , int port )
    {
        std::string name = SECONDARY_DELILAH_CONNECTION_NAME;

        NetworkConnection* previous_network_connection = connections.findInMap(name);
        if( previous_network_connection != NULL )
        {
            return au::str( "Not adding node %s:%d since still processing node %s"
                            , host.c_str()
                            , port 
                            , previous_network_connection->str().c_str()
                           );
        }
        
        // Init connection
        SocketConnection* socket_connection;
        Status s = SocketConnection::newSocketConnection(host , port , &socket_connection );        
        
        if( s != OK )
        {
            return 
                    au::str("Error: Not possible to open connection with %s:%d (%s)\n" 
                            , host.c_str() 
                            , port 
                            , status(s)
                            );
        }
        
        // Create network connection with this socket
        NetworkConnection * network_connection = new NetworkConnection( name , socket_connection , this );
        
        // Insert in the map of connections
        connections.insertInMap( name , network_connection );
        
        // Start the read and write threads for this connection
        network_connection->initReadWriteThreads();
        
        return  au::str("Sent message to %s:%d to add this node to the cluster\n" 
                        , host.c_str() 
                        , port 
                        );
    }    
    


    
    void DelilahNetwork::processHello( NetworkConnection* connection, Packet* packet )
    {
        
        // -----------------------------------------------------------------------------------------------
        // Check correct format for hello message ( cluster_information & node_identifier )
        // -----------------------------------------------------------------------------------------------
        
        if( !packet->message->has_hello() )
            LM_X(1, ("Error in SAMSON Protocol: Non Hello information in a hello packet"));
        
        // Information included in the hello message
        ClusterInformation new_cluster_information( packet->message->hello().cluster_information() );
        NodeIdentifier new_node_identifier( packet->message->hello().node_identifier() );
        
        std::string connection_name = connection->getName();
        std::string host = connection->getHost();
        int port = connection->getPort();
        
        // -----------------------------------------------------------------------------------------------
        // Main init connection
        // -----------------------------------------------------------------------------------------------
        
        if( connection_name == MAIN_DELILAH_CONNECTION_NAME )
        {
            if ( new_node_identifier.node_type != WorkerNode )
                LM_X(1, ("Error in SAMSON Protocol"));
            
            if( new_cluster_information.getId() == 0 )
            {
                // Create a new "1" node cluster with this worker
                cluster_information.init_new_cluster(host, port);
                
                
                // Send special Hello Packet
                // ---------------------------------------------------------------
                Packet * packet = helloMessage( connection );
                
                // Asigned id to this worker "0"
                packet->message->mutable_hello()->mutable_cluster_information()->set_assigned_id( 0 );
                
                connection->push(packet);
                // ---------------------------------------------------------------
                
                message(
                        au::str("Creating new cluster at %s:%d ( id %lu )"
                                , host.c_str()
                                , port
                                , cluster_information.getId()
                                )
                        );

                // Recolocation of connection ( it is alwasy worker_0 )
                connection->setNodeIdentifier( NodeIdentifier(WorkerNode,0) );
                move_connection( MAIN_DELILAH_CONNECTION_NAME , "worker_0" );
                
                // Notify delilah about the new connection
                report_worker_connected(0);
                
                return;
                
            }
            else
            {
                cluster_information.update( &new_cluster_information );

                
                message(
                        au::str("Connecting to cluster %lu with %lu nodes at %s:%d"
                                , cluster_information.getId()
                                , cluster_information.getNumNodes()
                                , host.c_str()
                                , port
                                )
                        );
                

                // Send hello back if necessary
                if( packet->message->hello().answer_hello_required() )
                    connection->push( helloMessage( connection ) );

                // Relocation of this connection to the rigth place
                connection->setNodeIdentifier( new_node_identifier );
                move_connection( MAIN_DELILAH_CONNECTION_NAME , connection->getNodeIdentifier().getCodeName() );
                
                report_worker_connected( new_node_identifier.id );
                
                return;
                
            }
            
        }

        // -----------------------------------------------------------------------------------------------
        // Secondary connection ( add node )
        // -----------------------------------------------------------------------------------------------
        
        if( connection_name == SECONDARY_DELILAH_CONNECTION_NAME )
        {
            if( cluster_information.getId() == 0 )
                LM_X(1, ("SAMSON Protocol error"));

            if( new_cluster_information.getId() != 0 )
            {
                // Not possible to add this connections
                connection->close();
                message( 
                        au::str("Not possible to add node %s:%d since it belongs to another cluster"
                                , host.c_str()
                                , port
                                 ) 
                        );
                return;
            }
            
            // Add this node to the cluster
            size_t assigned_id = cluster_information.add_node(host, port);
            
            
            // Send Hello Packet "Identifier"
            // ---------------------------------------------------------------
            Packet * packet = helloMessage( connection );
            packet->message->mutable_hello()->mutable_cluster_information()->set_assigned_id( assigned_id );
            connection->push(packet);
            // ------------------------------------------------------------     
            
            // Send Hello to the rest of nodes
            std::vector<size_t> ids = getWorkerIds();
            for ( size_t i = 0 ; i < ids.size() ; i++ )
                if( ids[i] != assigned_id )
                {
                    Packet *packet = helloMessage(NULL);
                    packet->to = NodeIdentifier( WorkerNode , ids[i] );
                    send(packet);
                }

            // Recolocation of connection 
            connection->setNodeIdentifier(  NodeIdentifier(WorkerNode,assigned_id) );
            move_connection( SECONDARY_DELILAH_CONNECTION_NAME , connection->getNodeIdentifier().getCodeName() );
            
            report_worker_connected( assigned_id );
            return;
        }        
        

         
        // -----------------------------------------------------------------------------------------------
        // General update of cluster information
        // -----------------------------------------------------------------------------------------------
        
        if( cluster_information.getId() == 0 )
            LM_X(1, ("SAMSON Protocol error"));

        if( cluster_information.getId() != new_cluster_information.getId() )
        {
            LM_W(("[%s] Rejecting an incomming hello message since it is involved in another cluster (%lu vs %lu)"
                  , connection->str().c_str() 
                  , cluster_information.getId()
                  , new_cluster_information.getId()
                  ));
            connection->close();
            return;
        }
        else
        {
            if( cluster_information.getVersion() < new_cluster_information.getVersion() )
                cluster_information.update(&new_cluster_information);
        }
        
        // -----------------------------------------------------------------------------------------------
        // Send Hello back if necessary
        // -----------------------------------------------------------------------------------------------
        
        if( packet->message->hello().answer_hello_required() )
            connection->push( helloMessage( connection ) );
    }
    
    std::string DelilahNetwork::cluster_command( std::string command )
    {
        
        au::CommandLine cmdLine;
        cmdLine.set_flag_string("user", "anynimous");
        cmdLine.set_flag_string("password", "anynimous");
        cmdLine.parse(command);
        
        if ( cmdLine.get_num_arguments() == 0 )
            return "";

        if ( cmdLine.get_num_arguments() == 1 )
        {
            return "Help about cluster command comming soon. Type cluster info/add_node";
        }

        std::string main_command = cmdLine.get_argument(1);
        
        if ( main_command == "info" )
        {
            au::tables::Table * table = getClusterConnectionsTable();
            std::string res = table->str();
            delete table;
            return res;
        }

        if ( main_command == "connections" )
        {
            au::tables::Table * table = getConnectionsTable();
            std::string res = table->str();
            delete table;
            return res;
        }

        if ( main_command == "pending" )
        {
            au::tables::Table * table = getPendingPacketsTable();
            std::string res = table->str();
            delete table;
            return res;
        }
        
        if( main_command == "get_my_id" )
        {
            // Inform about this delilah identifier
            return au::str("Delilah id: %s\n", au::code64_str(node_identifier.id).c_str() );
        }
        
        if ( main_command == "connect" )
        {
            if( cmdLine.get_num_arguments() < 3 )
                return "Usage: cluster connect host [port]";
            
            std::string host = cmdLine.get_argument(2);
            int port = SAMSON_WORKER_PORT;
            if( cmdLine.get_num_arguments() > 3 )
                port = atoi( cmdLine.get_argument(3).c_str() );
            
            resetNetworkManager();
            cluster_information.clearClusterInformation();
            
            std::string user = cmdLine.get_flag_string("user");
            std::string password = cmdLine.get_flag_string("password");
            
            Status s = addMainDelilahConnection( host , port , user , password );

            if ( s == OK)
                return au::str("Connecting with  %s:%d" , host.c_str() , port );
                        else
                return au::str("Not possible to open connection with %s:%d (%s)" , host.c_str() , port , status(s));
            
        }
        
        // cluster add host port
        if ( main_command == "add" )
        {
            if ( cmdLine.get_num_arguments() < 3 )
                return "Usage: cluster add host_name [port]";
            
            std::string host = cmdLine.get_argument(2);
            int port = SAMSON_WORKER_PORT;
            if( cmdLine.get_num_arguments() > 3 )
                port = atoi( cmdLine.get_argument(3).c_str() );
            
            return addSecondaryDelilahConnection(host, port);
            
        }

        
        // cluster remove id
        if ( main_command == "remove" )
        {
            if ( cmdLine.get_num_arguments() < 3 )
                return "Usage: cluster remove id";
            

            if( !is_only_digits( cmdLine.get_argument(2) ) )
                return au::str("Please, provide an id as an argument. %s is not a number" , cmdLine.get_argument(2).c_str() );
            
            size_t id = atoll( cmdLine.get_argument(2).c_str() );

            // Remove this node from the cluster information
            bool s = cluster_information.remove_host( id );
           
            if ( !s )
                return au::str("Non valid worker identifier (%lu). Check 'cluster info' to show cluster nodes" , id );
            
            
            std::ostringstream output;
            
            // Close connection with this node informing to reset the cluster information
            std::string connection_name = NodeIdentifier( WorkerNode , id ).getCodeName();
            NetworkConnection* connection = connections.findInMap( connection_name );
            if ( connection )
            {
                Packet* packet = helloMessage( connection );
                packet->message->mutable_hello()->set_reset_cluster_information(true);
                
                connection->push(packet);
            }
            else
                // Do not close here... it will be closed from the other end
                output << au::str("Not possible to notify worker %lu (%s), since we are not connected to it.\n" 
                                  , id 
                                  , connection_name.c_str() 
                                  );
            
            // Update all workers
            std::vector<size_t> ids = getWorkerIds();
            for ( size_t i = 0 ; i < ids.size() ; i++ )
            {
                Packet * packet = helloMessage(NULL);
                packet->to = NodeIdentifier( WorkerNode , ids[i] );
                send( packet );
            }
            
            output << au::str("OK. Worker %lu eliminated from cluster. Now it has %lu nodes\n" , 
                              id , 
                              cluster_information.getNumNodes() 
                              );
            return output.str();
            
        }
        
        
        return au::str("Unknown cluster command %s", main_command.c_str() );
    }
    
    void DelilahNetwork::message( std::string txt )
    {
        network_interface_receiver->receive( Packet::messagePacket( txt + "\n" ) );
    }

    
    
}
