
#ifndef _H_SAMSON_CONNECTOR_LISTENER
#define _H_SAMSON_CONNECTOR_LISTENER

#include "au/network/NetworkListener.h"
#include "common.h"
#include "Item.h"

namespace samson 
{
    namespace connector
    {
        
        class ListenerItem : public Item, public au::NetworkListener , public au::NetworkListenerInterface
        {

            // Port to open to accept connections
            int port;
            
            friend class SamsonConnector;
            
            au::Status status_init;
            
        public:
            
            ListenerItem( Channel * channel , ConnectionType type , int _port ) : 
            Item( channel , type , au::str("LISTEN(%d)" , _port ) ) ,
            au::NetworkListener( this )
            {
                // Keep the port
                port = _port;
                
                // Init listener and run in background
                status_init = initNetworkListener( port );    
                
                if ( status_init == au::OK )
                    runNetworkListenerInBackground();
            }
            
            // samson::NetworkListenerInterface
            void newSocketConnection( au::NetworkListener* listener 
                                     , au::SocketConnection * socket_connetion );
            
            // Get status of this element
            std::string getStatus()
            {
                if ( isNetworkListenerRunning() )
                    return "Listening";
                else
                    return au::str( "NOT Listening (%s)" , au::status( status_init ) );
            }
            
            // Review item: open port if it was not possible in the past...
            void review_item()
            {
                
                if( !isNetworkListenerRunning() )
                {
                    // Init again
                    status_init = initNetworkListener( port );    
                    
                    if ( status_init == au::OK )
                        runNetworkListenerInBackground();
                }
            }
            
            void stop_item()
            {
                // Stop listener
                if( isNetworkListenerRunning() )
                    stop(true);
            }
            
        };
    }
}


#endif