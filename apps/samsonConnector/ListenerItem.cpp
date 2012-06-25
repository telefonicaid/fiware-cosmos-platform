
#include "SamsonConnector.h"
#include "FileDescriptorConnection.h"
#include "ListenerItem.h" // Own interfave

namespace samson {
    namespace connector {
        
        void ListenerItem::newSocketConnection( au::NetworkListener* listener 
                                               , au::SocketConnection * socket_connetion )
        {

            if( isRemoving() )
            {
                // Do not accept connections if removing...
                delete socket_connetion; 
                return;
            }
            
            std::string name  = au::str("Socket %s" , socket_connetion->getHostAndPort().c_str() );
            
            FileDescriptorConnection* new_connection = new FileDescriptorConnection( this 
                                                                              , getType() 
                                                                              , name 
                                                                              , socket_connetion );
            // Add this item as my children item
            add( new_connection ); 
        }
        
    }
}