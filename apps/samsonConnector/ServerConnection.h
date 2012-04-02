#ifndef _H_SAMSON_CONNECTOR_SERVER_CONNECTION
#define _H_SAMSON_CONNECTOR_SERVER_CONNECTION

#include <string>

#include "au/Token.h"
#include "au/TokenTaker.h"

#include "common.h" 

namespace samson 
{
    
    class SamsonConnectorConnection;
    class SamsonConnector;
    
    class ServerConnection : public SamsonConnectorItem
    {
        std::string host;
        int port;
        
        SamsonConnectorConnection* connection; // Established connection when possible
        au::Token token;
        
        au::Cronometer connection_cronometer;
        int connection_trials;
        
    public:
                
        // Constructor & Destructor
        ServerConnection( SamsonConnector* samson_connector ,  ConnectionType type , std::string _host , int _port  );
        ~ServerConnection();
        
        std::string getName();
        std::string getStatus();
        void review();

        // Can be removed ( no background threads and so... )
        bool canBeRemoved()
        {
            return false;
        }
        
        bool isConnected();
        void push( Block* block );
        size_t getOuputBufferSize();

        
    };
    
    
}


#endif