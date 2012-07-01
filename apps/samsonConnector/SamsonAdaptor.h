#ifndef _H_SAMSON_CONNECTOR_SAMSON_CONNECTION
#define _H_SAMSON_CONNECTOR_SAMSON_CONNECTION

#include "au/mutex/Token.h"

#include "Adaptor.h"
#include "Connection.h"
#include "common.h"



namespace stream_connector {
    
    class Block;
    
    class SamsonConnection : public Connection , public samson::DelilahLiveDataReceiverInterface
    {
        
        samson::SamsonClient * client;
        std::string queue;
        
    public:
        
        
        SamsonConnection( Item  * _item 
                         , ConnectionType _type 
                         , std::string name 
                         , samson::SamsonClient * _client 
                         , std::string _queue );
        
        
        ~SamsonConnection();
        
        virtual void start_connection();
        virtual void stop_connection();
        virtual void review_connection();
        
        size_t getSize();
        
        void push( engine::Buffer* buffer );
        
        // Overwriteen method of SamsonClient
        void receive_buffer_from_queue(std::string queue , engine::Buffer* buffer);
        
        std::string getStatus();
        
    };
    
    class SamsonAdaptor : public Item
    {
        // Information to stablish the connection with the SAMSON system
        std::string host;
        int port;
        std::string queue;
        
        // Last connection trial
        au::Cronometer cronometer;
        
    public:
        
        SamsonAdaptor( Channel * _channel , ConnectionType _type , 
                      std::string _host 
                      , int _port 
                      , std::string _queue );
        
        
        // Get status of this element
        std::string getStatus();
        
        void review_item();

        
        
    };
    
}

#endif

