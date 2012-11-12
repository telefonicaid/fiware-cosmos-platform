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



#ifndef _H_SAMSON_COMMON_NETWORK
#define _H_SAMSON_COMMON_NETWORK

#include "engine/Object.h"

#include "au/network/NetworkListener.h"

#include "samson/common/status.h"
#include "samson/common/ports.h"

#include "samson/network/NetworkInterface.h"
#include "samson/network/NetworkManager.h"
#include "samson/network/PacketQueue.h"


namespace samson {
    
    
    class CommonNetwork : public NetworkManager ,  public NetworkInterface , public engine::Object
    {
        
    protected:
        
        // My identifier in the cluster
        NodeIdentifier node_identifier;
        
        // Cluster information for this node ( worker or delilah )
        ClusterInformation cluster_information;

        // Information for the hello message
        std::string user;
        std::string password;
        std::string connection_type;

        // List of packets for me ( accumulated when receiver is still not set )
        PacketQueue pending_packets_for_me;
        
    public:
        
        CommonNetwork()
        {
            
            // Listen notifications for network manager
            listen("notification_network_manager_review");
            
            // Create a periodic notification to review connections
            engine::Engine::shared()->notify( new engine::Notification( "notification_network_manager_review" ) , 1  );
        }
        
        ~CommonNetwork()
        {
        }
        
        // Engine notification interface
        // ----------------------------------------------------------------
        void notify( engine::Notification* notification );

        // NetworkManager
        // ----------------------------------------------------------------
        void receive( NetworkConnection* connection, Packet* packet );
        virtual void processHello(NetworkConnection* connection , Packet* packet)
        {
            LM_W(("processHello not implemented"));
            if ((connection == NULL) || (packet == NULL))
                return;
        }

        // NetworkInterface common methods
        // ----------------------------------------------------------------
        Status send( Packet* packet );
        std::vector<size_t> getWorkerIds();
        std::vector<size_t> getDelilahIds();
        std::vector<size_t> getConnectedWorkerIds();
        void getInfo( ::std::ostringstream& output , std::string command ,  std::string format = "xml");
        network::Collection* getConnectionsCollection( Visualization* visualization );
        size_t get_rate_in();
        size_t get_rate_out();
        NodeIdentifier  getMynodeIdentifier();
        std::string     getHostForWorker(size_t worker_id);
        unsigned short  getPortForWorker(size_t worker_id);
        
        // Add output worker connections
        // ----------------------------------------------------------------
        Status addWorkerConnection( size_t worker_id , std::string host , int port );
        
        // Monitorization
        std::string str();
        
        // Get information to show on screen
        au::tables::Table* getClusterConnectionsTable();

    protected:
        
        // Hello message for this connection
        Packet* helloMessage( NetworkConnection * target ); 
      
        void report_worker_connected( size_t id );
        void report_worker_disconnected( size_t id );
        
        void report_delilah_connected( size_t id );
        void report_delilah_disconnected( size_t id );
        
    };
    

}

#endif
