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

#ifndef _H_AU_NETWORK_SERVICE
#define _H_AU_NETWORK_SERVICE

#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/set.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/gpb.h"

#include "au/string.h"
#include "au/utils.h"

#include "au/console/Console.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"

namespace au
{
    namespace network
    {
        
        class Service;
        
        class ServiceItem
        {
            Service * service;
            SocketConnection * socket_connection;

            bool quit; // Falg to inform that it is necessary to quit
            
            friend void* run_service_item( void*p );
            friend class Service;
            
        public:
            
            ServiceItem( Service * _service , SocketConnection * _socket_connection );
            virtual ~ServiceItem();
            
            // Stop this connection
            void stop();
            
            void runInBackground();
            
        };
        
        // Service opening a paricular port and accepting connections
        
        class Service : public NetworkListenerInterface
        {
            
            int port;                      // Port to offer this service
            bool init;                     // Flag to avoid multiple inits 
            NetworkListener listener;      // Listener to receive connections
            au::set< ServiceItem > items;  // Connected items 

            au::Token token;               // Mutex protection ( list of items )
            
            friend class ServiceItem;
            friend void* run_service_item( void*p );
            
        public:
            
            // Constructor with the port to open
            Service( int _port );            
            
            // DEbug informatation
            std::string getStatus()
            {
                if( listener.isNetworkListenerRunning() )
                    return "listening";
                else
                    return "not listening";
            }
            
            // Init the service to recover the error code if not possible to open port
            Status initService( );

            // NetworkListenerInterface
            void newSocketConnection( NetworkListener* _listener , SocketConnection * socket_connetion );            

            // Debug information
            std::string str();            
            au::tables::Table* getConnectionsTable();            
            int getPort();

            // Stop all threads for connections and thread for listener
            void stop( bool wait );            
            
            // Overload this method to define service action
            virtual void run( SocketConnection * soket_connection , bool *quit )=0;
            
        private:
            
            // Method called by thread running item in background
            void finish( ServiceItem * item );
            
        };
                
    }
}

#endif
