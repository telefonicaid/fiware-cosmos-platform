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
#ifndef _H_CLUSTER_INFORMATION
#define _H_CLUSTER_INFORMATION

/* ****************************************************************************
 *
 * FILE                     CliusterInformation.h - information about the cluster
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2012
 *
 */

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // LmtNetworkInterface, ...

#include "au/string.h"
#include "au/xml.h"
#include "au/mutex/Token.h"     
#include "au/containers/vector.h"     
#include "au/mutex/TokenTaker.h"
#include "au/ErrorManager.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/samson.pb.h"

#include "samson/network/Message.h"    // samson::Message::MessageCode
#include "samson/network/Packet.h"

namespace samson {
    
    
    // Information for every node in the cluster
    class ClusterNode
    {
        
    public:

        std::string host; // Ip of the cluster
        int port;       // only used at the moment for testing
        size_t id;      // Identifier of node
        
        // Constructor
        ClusterNode( std::string _host , int _port , size_t _id );
        ClusterNode( ClusterNode* other );

        // Fukk Google Protool Buffer information
        void fill( network::ClusterNode* pb_cluster_node );
        
        // Get xml information for monitoring
        void getInfo( ::std::ostringstream& output , ::std::string format = "xml");

        // String monitorization
        std::string str();

        // Get some information
        NodeIdentifier getNodeIdentifier();
        
    };
    
    class ClusterInformation
    {
        size_t id;                         // Cluster information
        size_t version;                    // Version of this cluster information
        
        std::vector<ClusterNode*> nodes;   // Information for all nodes
        
        size_t assigned_id;                // Node id assigned to this worker
        
        au::Token token;                   // Mutex protection agains
        
    public:
        
        ClusterInformation();
        ClusterInformation ( network::ClusterInformation pb_cluster_information );
        ~ClusterInformation();

        
        // ----------------------------------------------------
        // Update cluster information...
        // ----------------------------------------------------
        
        void clearClusterInformation();
        void init_new_cluster( std::string host , int port );
        size_t add_node( std::string host , int port );
        bool remove_host( size_t worker_id );
        void update( ClusterInformation* new_cluster_information );

        
        // ----------------------------------------------------
        // Monitorization
        // ----------------------------------------------------
        
        void fill( network::ClusterInformation* pb_cluster_information );
        void parse( network::ClusterInformation* pb_cluster_information );
        void getInfo( ::std::ostringstream& output , ::std::string format = "xml" );
        std::string str();
        
        // ----------------------------------------------------
        // Getting and setting information
        // ----------------------------------------------------
        
        
        size_t getId();
        size_t getVersion();
        size_t getNumNodes();
        size_t getAssignedId();
        void setAssignedId( size_t _assigned_id );

        std::string str_identifier();

        std::string     hostForWorker( size_t worker_id );
        unsigned short  portForWorker( size_t worker_id );
        size_t          getIdForWorker( std::string host ); // host or host:port are accepted
        
        // ----------------------------------------------------
        // Saving and loading to file
        // ----------------------------------------------------
        
        
        void remove_file();
        void save(  au::ErrorManager * error );
        void load( au::ErrorManager * error );
        au::vector<ClusterNode> getNodesToConnect( NodeIdentifier node_identifier );
        au::vector<ClusterNode> getNodes( );
        std::vector<size_t> getWorkerIds();
        
        
    };

}
    
#endif
    
