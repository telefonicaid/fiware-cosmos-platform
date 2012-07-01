
#include "StreamConnector.h"
#include "FileDescriptorConnection.h"
#include "ListenerAdaptor.h" // Own interfave

namespace stream_connector {
    
    ListenerAdaptor::ListenerAdaptor( Channel * channel , ConnectionType type , int _port ) : 
    Adaptor( channel , type , au::str("LISTEN(%d)" , _port ) ) ,
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
    std::string ListenerAdaptor::getStatus()
    {
        if ( isNetworkListenerRunning() )
            return "Listening";
        else
            return au::str( "NOT Listening (%s)" , au::status( status_init ) );
    }
    
    // Review item: open port if it was not possible in the past...
    void ListenerAdaptor::review_item()
    {
        
        if( !isNetworkListenerRunning() )
        {
            // Init again
            status_init = initNetworkListener( port );    
            
            if ( status_init == au::OK )
                runNetworkListenerInBackground();
        }
    }
    
    void ListenerAdaptor::stop_item()
    {
        // Stop listener
        if( isNetworkListenerRunning() )
            stop(true);
    }
    
    
    void ListenerAdaptor::newSocketConnection( au::NetworkListener* listener 
                                              , au::SocketConnection * socket_connetion )
    {
        
        std::string name  = au::str("Socket %s" , socket_connetion->getHostAndPort().c_str() );
        
        FileDescriptorConnection* new_connection = new SimpleFileDescriptorConnection( this 
                                                                                      , getType() 
                                                                                      , name 
                                                                                      , socket_connetion );
        // Add this item as my children item
        add( new_connection ); 
    }
    
}
