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


#include "au/ThreadManager.h"

#include "Service.h" // Own interface



namespace au
{
    namespace network
    {

        void* run_service_item( void*p )
        {
            // Recover the item from the provided pointer
            ServiceItem* item = (ServiceItem*)p;
            
            // Main function in Service to define what to do with new connections
            item->service->run( item->socket_connection , &item->quit );
            
            // Notify finish of this thread
            item->service->finish(item);

            // Remove item since it is not contained anywhere
            delete item;

            return NULL;
        }
        
        ServiceItem::ServiceItem( Service * _service , SocketConnection * _socket_connection )
        {
            // Keep a pointer to the service to nofity when finished
            service = _service;
            
            // Keep a pointer to the socket connection
            socket_connection = _socket_connection;
            
            // By default, do not quit
            quit = false;
        }
        
        ServiceItem::~ServiceItem()
        {
            // Make sure this connection is closed
            socket_connection->close();
            delete socket_connection;
        }
        
        
        void ServiceItem::stop()
        {
            socket_connection->close(); // Close connection, so the thread will come back
            quit = true;                // Also inform with this bool variable ( accessible in connection )
        }
        
        void ServiceItem::runInBackground()
        {
            // Run a separate thread for this connection
            pthread_t t;
            ThreadManager::shared()->addThread("ServiceItem", &t, NULL, run_service_item, this);
        }
        
        
        Service::Service( int _port ) : listener( this ) , token("Service")
        {
            port = _port;
            init = false;
        }
        
        Status Service::initService( )
        {
            Status s = listener.initNetworkListener( port );
            
            if( s == OK )
                listener.runNetworkListenerInBackground();
            
            return s;
        }
        
        std::string Service::str()
        {
            return au::str("Server on port %d" , port);
        }
        
        void Service::stop( bool wait )
        {
            // Stop all connections ( close socket and inform about quit )
            {
                au::TokenTaker tt(&token);
                au::set< ServiceItem >::iterator it_items;
                for (it_items = items.begin() ; it_items != items.end() ; it_items ++ )
                    (*it_items)->stop();
            }
            
            // Stop the main listener
            listener.stop( wait );
            
            // Wait until all connections are gone...
            {
                au::Cronometer c;
                while( true )
                {
                    // Check if all connections are gone
                    size_t num_items = 0;
                    {
                        au::TokenTaker tt(&token);
                        num_items = items.size();
                        if( num_items == 0 )
                            return;
                    }
                    
                    if( c.diffTime() > 1 )
                    {
                        c.reset();
                        LM_W(("Still %lu ServiceItems do not finish after closing its associated sockets in Server %s" , num_items , str().c_str() ));
                    }
                    
                    usleep(10000);
                }
            }
        }
        
        void Service::newSocketConnection( NetworkListener* _listener , SocketConnection * socket_connetion )
        {
            au::TokenTaker tt(&token);
            
            if( _listener != &listener )
            {
                LM_E(("Unexpected listner in au::Service"));
                return ;
            }
            
            // Create the item
            ServiceItem *item = new ServiceItem( this , socket_connetion );
            
            // Insert the item
            items.insert( item );
            
            // Run in background
            item->runInBackground();
            
        }
        
        au::tables::Table* Service::getConnectionsTable()
        {
            
            std::string format = "Host|time,f=time|In (B),f=uint64|Out (B),f=uint64|In (B/s),f=uint64|Out (B/s),f=uint64";
            au::tables::Table* table = new au::tables::Table( format );
            
            au::set< ServiceItem >::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                ServiceItem * item = *it_items;
                
                au::StringVector values;
                
                values.push_back( item->socket_connection->getHostAndPort() );
                values.push_back( au::str("%lu", item->socket_connection->getTime() ) );
                
                values.push_back( au::str("%lu", (size_t)item->socket_connection->rate_in.getTotalSize() ) );
                values.push_back( au::str("%lu", (size_t)item->socket_connection->rate_out.getTotalSize() ) );
                values.push_back( au::str("%lu", (size_t)item->socket_connection->rate_in.getRate() ) );
                values.push_back( au::str("%lu", (size_t)item->socket_connection->rate_out.getRate() ) );
                
                table->addRow( values );
            }
            
            
            return table;
        }
        
        int Service::getPort()
        {
            return  port;
        }
        
        
        void Service::finish( ServiceItem * item )
        {
            // Mutex proctection
            au::TokenTaker tt(&token);
            
            // Notify this has finished
            items.erase( item );
        }

        
    }
}