#ifndef _H_SAMSON_CONNECTOR_COMMON
#define _H_SAMSON_CONNECTOR_COMMON

#include "samson/client/SamsonClient.h"

namespace samson 
{
    
    typedef enum
    {
        connection_input,
        connection_output
    } ConnectionType;

    
    struct ServerConnection
    {
        std::string host;
        int port;
        
        ConnectionType type;
        
        ServerConnection( std::string _host , int _port , ConnectionType _type )
        {
            host = _host;
            port = _port;
            type = _type;
        }
        
        std::string getName()
        {
            if( type == connection_input )
                return au::str("input_%s:%d" , host.c_str() , port );
            else
                return au::str("output_%s:%d" , host.c_str() , port );
        }
        
        std::string str()
        {
            return au::str("%s:%d" , host.c_str() , port );
        }
        
    };
    
    
}


#endif