#ifndef _H_SAMSON_CONNECTOR_SAMSON_CONNECTION
#define _H_SAMSON_CONNECTOR_SAMSON_CONNECTION

#include "au/mutex/Token.h"

#include "common.h"


namespace samson {
    
    class Block;
    
    class SamsonConnection : public SamsonConnectorItem , SamsonClient
    {
        std::string queue;
        std::string host;
        int port;
        
        // Internal class for connect to a samson cluster
        bool connected;

        au::Token token;
        
    public:
        
        SamsonConnection( SamsonConnector * samson_connector ,  ConnectionType _type , std::string _host , int _port , std::string _queue );
        
        std::string getName();

        
        // Get status of this element
        std::string getStatus()
        {
            if( connected )
            {
                if( connection_ready() )
                    return  "connected";
                else
                    return "connected but not all workers available";
            }
            else
                return  "connecting...";
        }
        
        bool canBeRemoved()
        {
            return false; // Never remove this can of connections
        }
        
        bool isConnected()
        {
            return connected;
        }
        
        // Method called every 5 seconds to re-connect or whatever is necessary here...
        void review()
        {
            if( connected )
                return;
            
            au::ErrorManager error;
            initConnection( &error, host , port );
            connected = !error.isActivated(); // If not error, assumed connected
        }
        
        // Overload method to push blocks using samsonClient
        void push( engine::Buffer* buffer );

        // Overwritten method
        size_t getOuputBufferSize();
        
        
        // Overwriteen method of SamsonClient
        void receive_buffer_from_queue(std::string queue , engine::Buffer* buffer);

        
    };
    
}

#endif

