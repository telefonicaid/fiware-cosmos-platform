
#ifndef _H_SAMSON_CONNECTOR_LISTENER
#define _H_SAMSON_CONNECTOR_LISTENER

#include "samson/network/NetworkListener.h"
#include "common.h"

namespace samson 
{
    class SamsonConnectorListener : public NetworkListener , public samson::NetworkListenerInterface , public SamsonConnectorItem 
    {
        
        int port;
        Status status_init;
        
        friend class SamsonConnector;
        
    public:
      
        SamsonConnectorListener( SamsonConnector * samson_connector , ConnectionType type , int _port ) : NetworkListener( this ) , SamsonConnectorItem( samson_connector , type )
        {
            // Keep the port
            port = _port;
            
            // Init listener and run in background
            status_init = initNetworkListener( port );    
            
            if ( status_init == OK )
                runNetworkListenerInBackground();
        }

        // samson::NetworkListenerInterface
        void newSocketConnection( samson::NetworkListener* listener 
                                 , samson::SocketConnection * socket_connetion );

        
        // Get a name of this element
        std::string getName()
        {
            if ( status_init == OK )
                return au::str("Listen %d" , port);
            else
                return au::str("Error opening port %d ", port );
        }
        
        // Get status of this element
        std::string getStatus()
        {
            if ( status_init == OK )
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