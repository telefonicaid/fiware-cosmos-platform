
#ifndef _H_SAMSON_CONNECTOR_LISTENER
#define _H_SAMSON_CONNECTOR_LISTENER

#include "au/network/NetworkListener.h"
#include "common.h"

namespace samson 
{
    class SamsonConnectorListener : public au::NetworkListener 
                                  , public au::NetworkListenerInterface 
                                  , public SamsonConnectorItem 
    {
        
        int port;
        au::Status status_init;
        
        friend class SamsonConnector;
        
    public:
      
        SamsonConnectorListener( SamsonConnector * samson_connector , ConnectionType type , int _port ) : 
                  au::NetworkListener( this ) 
                  , SamsonConnectorItem( samson_connector , type )
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

        
        // Get a name of this element
        std::string getName()
        {
            if ( status_init == au::OK )
                return au::str("Listen %d" , port);
            else
                return au::str("Error opening port %d ", port );
        }
        
        // Get status of this element
        std::string getStatus()
        {
            if ( status_init == au::OK )
                return "Listening";
            else
                return "NOT Listening";
        }
        
        // Can be removed ( no background threads and so... )
        bool canBeRemoved()
        {
            return false; // never removed
        }
        
        // Check if this item is finished ( if so, no data is push here )
        bool isConnected()
        {
            return false;  // This one is not really connected
        }
        
        // Method called every 5 seconds to re-connect or whatever is necessary here...
        void review()
        {
            return; // Nothing to review here
        }
        

    };
}


#endif