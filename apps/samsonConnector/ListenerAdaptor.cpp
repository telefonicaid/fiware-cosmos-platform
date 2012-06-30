
#include "StreamConnector.h"
#include "FileDescriptorConnection.h"
#include "ListenerAdaptor.h" // Own interfave

namespace samson {
    namespace connector {
        
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
}