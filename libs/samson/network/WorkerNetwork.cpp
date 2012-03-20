
#include "au/utils.h"

#include "samson/network/NetworkConnection.h"

#include "WorkerNetwork.h" // Own interface

namespace samson {

    WorkerNetwork::WorkerNetwork( int port , int web_port )
    {
        // Workers are allways connected as user samson
        // ----------------------------------------------------------------------------
        user = "samson"; 
        connection_type = "worker";
        
        // Init counter for temporal network connection names
        // ----------------------------------------------------------------------------
        tmp_counter = 0;
        
        // Load cluster information from file
        // ----------------------------------------------------------------------------
        au::ErrorManager error;
        cluster_information.load(&error);
        
        // Node identifier
        // ----------------------------------------------------------------------------
        if( cluster_information.getId() != 0 )
            node_identifier = NodeIdentifier( WorkerNode , cluster_information.getAssignedId() );
        else
            node_identifier = NodeIdentifier( WorkerNode , -1 );
        
        
        // Add listsner for incomming connections
        // ----------------------------------------------------------------------------
        {
            worker_listener = new NetworkListener( this );
            Status s = worker_listener->initNetworkListener( port );
            
            if( s != OK )
            {
                // Not allow to continue without incomming connections...
                LM_X(1, ("Not possible to open port %d (%s)" , port , status(s) ));
            }
            // Init background thread to receive connections 
            worker_listener->runNetworkListenerInBackground();
            
        }
        
        // Add listsner for incomming web/rest connections
        // ----------------------------------------------------------------------------
        {
            web_listener = new NetworkListener( this );
            Status s = web_listener->initNetworkListener( web_port );
            
            if( s != OK )
            {
                // Not allow to continue without incomming connections...
                LM_W(("Not possible to open web interface at port %d (%s)" , port , status(s) ));
            }
            else
            {
                // Init background thread to receive connections 
                web_listener->runNetworkListenerInBackground();
            }
        }
        // ----------------------------------------------------------------------------
            
    }
    
    void WorkerNetwork::newSocketConnection( NetworkListener* listener , SocketConnection * socket_connection )
    {
        if( listener == worker_listener )
        {
            
            //Add to the unknown connection vector
            std::string name = au::str("tmp_%lu" , tmp_counter++ );
                
            // Create network connection
            NetworkConnection* network_connection = new NetworkConnection( name , socket_connection , this );
            
            // Add this connection in the NetworkManager
            NetworkManager::add( network_connection );
            
            // Send Hello packet
            network_connection->push( helloMessage( network_connection ) );
        }
        else if ( listener == web_listener )
        {
            char line[1024];

            Status s = socket_connection->readLine( line , sizeof(line) , 10 );

            if (s == OK)
            {
                //
                // Read the rest of the REST Request, but without parsing it ...
                //
                char flags[1024];
                s = socket_connection->readBuffer(flags, sizeof(flags), 1);
                if (s == OK)
                    LM_T(LmtRest, ("Rest of GET:\n%s", flags));
                else
                    LM_W(("error reading rest of REST request"));
                au::CommandLine cmdLine;
                cmdLine.parse( line );
                
                if ((cmdLine.get_num_arguments() >= 2) && (cmdLine.get_argument(0) == "GET"))
                {
                    std::string content = network_interface_receiver->getRESTInformation(cmdLine.get_argument(1));
                    socket_connection->writeLine(content.c_str(), 1, 0, 100); // Try just once, timeout 0.0001 seconds
                }
                else
                    socket_connection->writeLine("Error: GET message expected\n", 1, 0, 100);
            }
            else
                socket_connection->writeLine(au::str("Error: unable to read incoming command (%s)\n", status(s)).c_str(), 1, 0, 100);

            
            // Close socket connection in all cases
            socket_connection->close();
        }
        else
        {
            LM_X(1, ("New connections from an unknown listener"));
            socket_connection->close();
            delete socket_connection;
        }
        
    }    
    
    void WorkerNetwork::processHello( NetworkConnection* connection, Packet* packet )
    {
        
        // -----------------------------------------------------------------------------------------------
        // Check correct format for hello message ( cluster_information & node_identifier )
        // -----------------------------------------------------------------------------------------------
        
        if( !packet->message->has_hello() )
        {
            LM_W(("Error in SAMSON Protocol: Non Hello information in a hello packet"));
            connection->close();
            return;
        }
        
        // Recover information included in the hello message
        ClusterInformation new_cluster_information( packet->message->hello().cluster_information() );
        NodeIdentifier new_node_identifier( packet->message->hello().node_identifier() );

        connection->setUserAndPassword( packet->message->hello().user(), packet->message->hello().password() );
        connection->setConnectionType( packet->message->hello().connection_type() );
        
        // -----------------------------------------------------------------------------------------------
        // General update of cluster information
        // -----------------------------------------------------------------------------------------------
        
        if( cluster_information.getId() == 0 )
        {
            if( new_node_identifier.node_type == DelilahNode )
            {
                // New cluster form a delilah ( should provide information about my position in this cluster )
                size_t assigned_id = packet->message->hello().cluster_information().assigned_id();
                node_identifier = NodeIdentifier( WorkerNode , assigned_id );
                update_cluster_information( &new_cluster_information  , assigned_id );
                
                // Notify worker to reset everyting 
                // ------------------------------------------------------------
                Packet *packet = new Packet( Message::WorkerCommand );
                network::WorkerCommand * worker_command = packet->message->mutable_worker_command();
                worker_command->set_command("remove_all_stream");
                packet->message->set_delilah_component_id(-1);
                packet->from = NodeIdentifier( DelilahNode , 0 );
                packet->to = node_identifier;
                network_interface_receiver->receive(  packet );
                // ------------------------------------------------------------
                
            }
            else
            {
                LM_W(("[%s] Rejecting an incomming hello message from a worker since a cluster is not defined"
                      , connection->str().c_str() 
                      ));
                connection->close();
                return;
                
            }
        }
        else
        {
            if( cluster_information.getId() != new_cluster_information.getId()  )
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
                    update_cluster_information(&new_cluster_information);
            }
        }            

        // -----------------------------------------------------------------------------------------------
        // Reset flag
        // -----------------------------------------------------------------------------------------------
        
        if ( new_node_identifier.node_type == DelilahNode )
            if( packet->message->hello().has_reset_cluster_information() )
                if( packet->message->hello().reset_cluster_information() )
                {
                    // Reset cluster information
                    cluster_information.clearClusterInformation();
                    cluster_information.remove_file();
                    
                    // Close all connections
                    NetworkManager::reset();
                }
        
        // -----------------------------------------------------------------------------------------------
        // Set NodeIdentifier and move connection to the rigth place ( name )
        // -----------------------------------------------------------------------------------------------
        
        std::string prefix = "tmp_";
        std::string connection_name = connection->getName();
        if( connection_name.substr( 0 , prefix.size() ) == prefix )
        {
            if( new_node_identifier.node_type == DelilahNode )
            {
                // New delilah connection
                connection->setNodeIdentifier( new_node_identifier );

                // Check it is a valid id
                size_t new_delilah_id = new_node_identifier.id;
                if( !au::code64_is_valid(new_delilah_id) )
                {
                    connection->close();
                    return;
                }
                
                // Check there is not another delilah with the same id... 
                std::string future_conection_name = connection->getNodeIdentifier().getCodeName();
                if( NetworkManager::isConnected(future_conection_name) )
                {
                    LM_W(("It seems there is another delilah with the same id (%s) already connected [ Moving from %s to %s ]" 
                          , future_conection_name.c_str()
                          , connection_name.c_str() 
                          , future_conection_name.c_str() ));
                    connection->close();
                    return;
                }
                
                
                // Move the connection to the rigth place
                NetworkManager::move_connection( connection_name ,  future_conection_name );
            }
            else if ( new_node_identifier.node_type == WorkerNode )
            {
                std::string future_connection_name = new_node_identifier.getCodeName();
                if ( NetworkManager::isConnected(future_connection_name) )
                {
                    LM_W(("[%s] Rejecting an incomming hello since we are already connected to this worker"
                          , connection->str().c_str() 
                          , cluster_information.getId()
                          , new_cluster_information.getId()
                          ));
                    connection->close();
                    return;
                }
                connection->setNodeIdentifier( new_node_identifier );
                NetworkManager::move_connection( connection_name , future_connection_name );
            }
            else
            {
                LM_W(("[%s] Rejecting an incomming hello message"
                      , connection->str().c_str() 
                      ));
                connection->close();
                return;
                
            }
        }
        
        // -----------------------------------------------------------------------------------------------
        // Send Hello back if necessary
        // -----------------------------------------------------------------------------------------------

        if( packet->message->hello().answer_hello_required() )
            connection->push( helloMessage( connection ) );
    }
    
    void WorkerNetwork::update_cluster_information( ClusterInformation * new_cluster_information )
    {
        cluster_information.update( new_cluster_information );
        
        // Save cluster information
        au::ErrorManager error;
        cluster_information.save(&error);
        if( error.isActivated() ) 
            LM_W(("Error saving cluster information: %s" , error.getMessage().c_str()));
        
    }

    void WorkerNetwork::update_cluster_information( ClusterInformation * new_cluster_information , size_t assigned_id )
    {
        cluster_information.update( new_cluster_information );
        cluster_information.setAssignedId( assigned_id );

        // Save cluster information
        au::ErrorManager error;
        cluster_information.save(&error);
        if( error.isActivated() ) 
            LM_W(("Error saving cluster information: %s" , error.getMessage().c_str()));
        
    }
    
    std::string WorkerNetwork::cluster_command( std::string command )
    {
        au::CommandLine cmdLine;
        cmdLine.parse(command);
        
        if ( cmdLine.get_num_arguments() == 0 )
            return "";
        
        if ( cmdLine.get_num_arguments() == 1 )
        {
            return "Type cluster info connections";
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
            au::tables::Table * table = NetworkManager::getConnectionsTable();
            std::string res = table->str();
            delete table;
            return res;
        }

        if ( main_command == "pending" )
        {
            au::tables::Table * table = NetworkManager::getPendingPacketsTable();
            std::string res = table->str();
            delete table;
            return res;
        }
        
        return au::str("Unknown cluster command %s", main_command.c_str() );
    }
 
    

}
