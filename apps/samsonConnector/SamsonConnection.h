#ifndef _H_SAMSON_CONNECTOR_SAMSON_CONNECTION
#define _H_SAMSON_CONNECTOR_SAMSON_CONNECTION

#include "au/Token.h"

#include "common.h"

namespace samson {
    
    class Block;
    
    class SamsonConnection
    {
        std::string queue;
        std::string host;
        int port;
        
        ConnectionType type;
        
        SamsonClient *samson_client;

        au::Token token;
        
    public:
        
        SamsonConnection( std::string _host , int _port , ConnectionType _type , std::string _queue );
        
        std::string getName();
        std::string str();
        void push( Block* block );
        
        ConnectionType getType()
        {
            return type;
        }
        
        
    };
    
}

#endif

