


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
        connected = samson_client->initConnection( host , port );
        
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
        const char* type_name = (type==connection_input)?"Input ":"Output";
        const char* connection_name = (connected)?"[CONNECTED]":"[NOT CONNECTED]";

        return au::str("%s [ %s %s %s %s ] %s SAMSON %s:%d" 
                       , type_name
                       , au::str( samson_client->pop_rate.getTotalSize() ,"B" ).c_str()
                       , au::str( samson_client->pop_rate.getRate() ,"B/s").c_str()
                       , au::str( samson_client->push_rate.getTotalSize() ,"B").c_str()
                       , au::str( samson_client->push_rate.getRate() ,"B/s").c_str()
                       , connection_name
                       , host.c_str() 
                       , port );
    }
    
    void SamsonConnection::push( Block* block )
    {
        if( !connected )
            connected = samson_client->initConnection( host , port );

        if( !connected )
            return;
        
        if( type == connection_input )
            return; // Nothing to do if we are input
        
        
        samson_client->push(queue, new BlockDataSource(block) );
        
        
    }


}