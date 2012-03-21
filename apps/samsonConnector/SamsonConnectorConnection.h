#ifndef _H_SAMSON_CONNECTOR_CONNECTION
#define _H_SAMSON_CONNECTOR_CONNECTION

#include "au/TokenTaker.h"

#include "samson/network/NetworkListener.h"
#include "samson/network/SocketConnection.h"
#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"

#include "BufferProcessor.h"
#include "Block.h"

#include "common.h"

extern size_t buffer_size;

namespace samson{
    
    // Function to run the background thread
    void* run_SamsonConnectorConnection( void* p);
    
    
    class SamsonConnectorConnection
    {
        
    private:
        
        SamsonConnector* samson_connector;   // General smason connector class
        FileDescriptor * file_descriptor;    // File descritor to read or write
        bool thread_running;                 // Flag to indicate if thread is still running in background
        
        ConnectionType type;                 // Type ( input / output )

        BufferProcessor *block_processor;    // Processor for input buffers ( only in input )
        
        au::Token token;
        au::list<Block> pending_blocks;      // List of pending packets ( only in output )
        Block* current_block;                // Block currently being sent
        
    public:

        // Rate statistics
        au::rate::Rate input_rate;
        au::rate::Rate output_rate;
        
    public:
    
        SamsonConnectorConnection( SamsonConnector* _samson_connector, FileDescriptor * _file_descriptor , ConnectionType _type );
        ~SamsonConnectorConnection();
        
        // Main function of the dedicated thread
        void run();
        
        void run_as_input();
        void run_as_output();
        
        // Function to check if the thread is running from the manager
        bool isFinished()
        {
            return !thread_running;
        }
        
        ConnectionType getType()
        {
            return type;
        }

        std::string getName()
        {
            return file_descriptor->getName();
        }
        
        // Debug string
        std::string str()
        {
            const char* type_name = (type==connection_input)?"Input ":"Output";
            
            return au::str("%s [ %s %s %s %s ] : %s "  
                           , type_name 
                           , au::str(input_rate.getTotalSize() ,"B").c_str()
                           , au::str(input_rate.getRate(),"B/s").c_str()
                           , au::str(output_rate.getTotalSize(),"B").c_str()
                           , au::str(output_rate.getRate(),"B/s").c_str()
                           , file_descriptor->getName().c_str() 
                           );
        }

        // Push a block to be emitted ( only output )
        void push( Block* block );
        size_t getOutputBufferSize();
        
        
    };
    
}


#endif