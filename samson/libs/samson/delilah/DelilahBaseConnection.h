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
#ifndef _H_DELILAH_BASE_CONNECTION
#define _H_DELILAH_BASE_CONNECTION


#include "au/mutex/Token.h"
#include "au/ErrorManager.h"
#include "samson/network/NetworkInterface.h"

namespace samson 
{
    
	class DelilahBaseConnection : public NetworkInterfaceReceiver
	{
        // Network interface
		NetworkInterface* network;								
        
        // Identifier of the next worker to send data
		int next_worker;
        
        // Token  to protect multiple access
        au::Token token;
        
    public:
        
		DelilahBaseConnection( ) : token("DelilahBaseConnection")
        {
            // By default no connection with network
            network = NULL;
            
            // No next worker decided
            next_worker = -1;

        }
        
		~DelilahBaseConnection()
        {
            if( network )
            {
                delete network;
                network = NULL;
            }
        }
        
        void delilah_connect( std::string connection_type 
                                                    , std::string host 
                                                    , int port
                                                    , std::string user
                                                    , std::string password
                                                    , au::ErrorManager* error);

        void stop();
        
        void delilah_disconnect( au::ErrorManager * error );

        // Check if we are connected
        bool isConnected();

        // Check if we are connected to all nodes
        bool isConnectionReady();
        
        // Network related operations ( only possible when connected )
        void send( Packet* packet , au::ErrorManager *error );
        
        std::vector<size_t> getConnectedWorkerIds( au::ErrorManager * error );
        std::vector<size_t> getWorkerIds( au::ErrorManager * error );
        
        // Run an internal command cluster
        std::string runClusterCommand( std::string command , au::ErrorManager* error );
        
        // Get information from cluster I am connected to
        std::string getConnectionInformation();
        
        
        // Set network interface for this delilah
        void setNetwork( NetworkInterface* _network );

        // Get next worker_id to send data...
        size_t getNextWorkerId();
        
        
    };
}

#endif
