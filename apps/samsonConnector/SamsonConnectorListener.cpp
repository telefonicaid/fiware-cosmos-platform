

#include "SamsonConnectorConnection.h"
#include "SamsonConnector.h"
#include "SamsonConnectorListener.h" // Own interfave


namespace samson {

    void SamsonConnectorListener::newSocketConnection( au::NetworkListener* listener 
                                              , au::SocketConnection * socket_connetion )
    {
        
        std::string name  = au::str("%s ( connected to localhost:%d )"  , socket_connetion->getHostAndPort().c_str() , port );
        
        SamsonConnectorConnection* new_item = new SamsonConnectorConnection( samson_connector 
                                                                            , type 
                                                                            , name 
                                                                            , socket_connetion );

        // Add this item as my children item
        samson_connector->show_message( au::str("Connection starts: %s" , new_item->getDescription().c_str() ) );
        samson_connector->add( new_item , getSamsonconnectorId() ); 
    }
                              
}