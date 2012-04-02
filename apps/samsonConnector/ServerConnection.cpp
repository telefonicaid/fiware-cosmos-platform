

#include "SamsonConnectorConnection.h"
#include "SamsonConnector.h"
#include "ServerConnection.h" // Own interface


namespace samson {


    ServerConnection::ServerConnection( SamsonConnector* samson_connector 
                                       , ConnectionType type 
                                       ,  std::string _host 
                                       , int _port  ) 
        : SamsonConnectorItem( samson_connector , type ) , token("ServerConnection")
    {
        host = _host;
        port = _port;
        
        connection = NULL;
        review();
        
        connection_cronometer.reset();
        connection_trials = 0;
    }
    
    ServerConnection::~ServerConnection()
    {
        if( connection )
            delete connection;
    }
    
    void ServerConnection::review()
    {
        au::TokenTaker tt(&token);
        
        // If already connected, nothing to do
        if( connection )
        {
            if( connection->canBeRemoved() )
            {
                delete connection;
                connection = NULL;

                // Init counter and cronometer
                connection_cronometer.reset();
                connection_trials = 0;
            }
            else
                return;
        }
        
        SocketConnection* socket_connection;
        Status s = SocketConnection::newSocketConnection( host
                                                         , port
                                                         , &socket_connection);                                  
        if( s == OK )
        {
            std::string name = socket_connection->getHostAndPort().c_str();
            connection = new SamsonConnectorConnection( samson_connector , type , name , socket_connection  );
        }
        else
            connection_trials++;
    }
    
    std::string ServerConnection::getName()
    {
        return au::str("%s:%d" , host.c_str() , port );
    }

    std::string ServerConnection::getStatus()
    {
        au::TokenTaker tt(&token);
        if( !connection )
        {
            return au::str("connecting... [ %d trials in %s ] )" 
                           , connection_trials 
                           , connection_cronometer.str().c_str() );
        }
        else
            return connection->getStatus();
    }
    
    
    bool ServerConnection::isConnected()
    {
        if( !connection ) 
            return false;
        
        return connection->isConnected();
    }

    void ServerConnection::push( Block* block )
    {
        // Overwrite this methid to pass block to the connection
        if ( connection )
            connection->push(block);
    }

    size_t ServerConnection::getOuputBufferSize()
    {
        // Overwrite this methid to pass block to the connection
        if ( connection )
            return connection->getOuputBufferSize();
        return 0;
    }


    
}