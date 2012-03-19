


#include "Block.h"


#include "SamsonConnection.h" // Own interface


namespace samson {

    SamsonConnection::SamsonConnection( std::string _host , int _port , ConnectionType _type , std::string _queue ) 
    : token( "SamsonConnection" )
    {
        host = _host;
        port = _port;
        type = _type;
        queue = _queue;
        
        samson_client = new SamsonClient( "connector" );
        samson_client->init( host , port );
        
        if( type == connection_input )
            samson_client->connect_to_queue(queue, false, false);
    }
    
    std::string SamsonConnection::getName()
    {
        if( type == connection_input )
            return au::str("input_%s:%d_%s" , host.c_str() , port , queue.c_str());
        else
            return au::str("output_%s:%d_%s" , host.c_str() , port , queue.c_str() );
    }
    
    std::string SamsonConnection::str()
    {
        return au::str("%s:%d" , host.c_str() , port );
    }
    
    void SamsonConnection::push( Block* block )
    {
        if( type == connection_input )
            return; // Nothing to do if we are input
        
        
        samson_client->push(queue, new BlockDataSource(block) );
        
        
    }


}