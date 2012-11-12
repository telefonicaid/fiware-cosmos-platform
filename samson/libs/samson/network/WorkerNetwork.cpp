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

#include "au/utils.h"

#include "samson/network/NetworkConnection.h"

#include "WorkerNetwork.h" // Own interface

namespace samson {

    WorkerNetwork::WorkerNetwork( int port , int web_port )
    {
        // Workers are always connected as user samson
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
        
        
        // Add listener for incoming connections
        // ----------------------------------------------------------------------------
        {
            worker_listener = new au::NetworkListener( this );
            au::Status s = worker_listener->initNetworkListener( port );
            
            if( s != au::OK )
            {
                // Not allow to continue without incoming connections...
                LM_X(1, ("Not possible to open main samson port %d (%s). Probably another worker is running..." , port , status(s) ));
            }
            // Init background thread to receive connections 
            worker_listener->runNetworkListenerInBackground();
            
        }
  
    }
    
    
    WorkerNetwork::~WorkerNetwork()
    {
        LM_T(LmtCleanup, ("In cleanup"));
        if (worker_listener != NULL)
        {
            delete worker_listener;
            worker_listener = NULL;
        }
        
    }
    
    void WorkerNetwork::stop()
    {
        // Stop listeners
        worker_listener->stop( true );
        
        // Close all connections
        NetworkManager::reset();
    }

    void WorkerNetwork::newSocketConnection( au::NetworkListener* listener , au::SocketConnection * socket_connection )
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
            Packet* hello_packet =helloMessage( network_connection );
            network_connection->push( hello_packet );
            hello_packet->release();
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
        // Reset flag
        // -----------------------------------------------------------------------------------------------
        
        if ( new_node_identifier.node_type == DelilahNode )
            if( packet->message->hello().has_reset_cluster_information() )
                if( packet->message->hello().reset_cluster_information() )
                {
                    LM_W(("Received a reset cluster command"));
                    
                    // Reset cluster information
                    cluster_information.clearClusterInformation();
                    cluster_information.remove_file();
                    
                    // Close all connections
                    NetworkManager::reset();
                    return;
                }

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
                
                // Notify worker to reset everything
                // ------------------------------------------------------------
                Packet *packet = new Packet( Message::WorkerCommand );
                network::WorkerCommand * worker_command = packet->message->mutable_worker_command();
                worker_command->set_command("remove_all_stream");
                packet->message->set_delilah_component_id(-1);
                packet->from = NodeIdentifier( DelilahNode , 0 );
                packet->to = node_identifier;
                network_interface_receiver->receive(  packet );
                packet->release();
                // ------------------------------------------------------------
                
            }
            else
            {
                LM_W(("[%s] Rejecting incoming hello message from a worker as no cluster is defined"
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
                LM_W(("[%s] Rejecting incoming hello message since it is meant for another cluster (%lu vs %lu)"
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
                    LM_W(("[%s] Rejecting incoming hello as this worker is already connected"
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
                LM_W(("[%s] Rejecting incoming hello message"
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
        {
            Packet * hello_packet = helloMessage( connection );
            connection->push( hello_packet );
            hello_packet->release();
        }
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
