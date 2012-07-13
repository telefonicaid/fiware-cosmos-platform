
#include "Channel.h"

#include "StreamConnector.h"
#include "SamsonAdaptor.h" // Own interface

namespace stream_connector {
    
    SamsonConnection::SamsonConnection( Adaptor  * _item 
                                       , ConnectionType _type 
                                       , std::string host 
                                       , int port
                                       , std::string queue 
                                       )
    : Connection( _item , _type , au::str("SAMSON(%s:%d:%s)" , host.c_str() , port , queue.c_str() ) )
    {
        // No init client by default here
        client_ = NULL;
        
        // Keep information for connection
        host_ = host;
        port_ = port;
        queue_ = queue;
     
        // Init values
        num_connection_trials = 0;
    }
    
    
    void SamsonConnection::try_connect()
    {
        if( client_ )
            return;
        
        // Update counter to trials
        num_connection_trials++;
        cronometer_reconnection.reset();
        
        // Try to connect to this SAMSON
        client_ = new samson::SamsonClient("connector");
        
        au::ErrorManager error;
        client_->initConnection( &error , host_ , port_ );
        
        if( error.isActivated() )
        {
            // Not possible to establish connection
            delete client_;
            client_ = NULL;
            return;
        }
        
        // At the moment, it is not possible to specify flags new of clear here
        if( getType() == connection_input )
            client_->connect_to_queue(queue_, false, false); 
        
        // Set me as the receiver of live data from SAMSON
        client_->set_receiver_interface(this);

    }
    
    SamsonConnection::~SamsonConnection()
    {
        if( client_ )
            delete client_;
    }
    
    void SamsonConnection::start_connection()
    {
        
        // Try to connect by the first time
        try_connect();
    }
    
    void SamsonConnection::stop_connection()
    {
        // TODO: Stop all theads since it will be removed
    }
    
    void SamsonConnection::review_connection()
    {
        // Nothing to do here
        if( !client_ )
        {
            set_as_connected(false);
            if( cronometer_reconnection.diffTime() < 3 )
                return; // Do not try to connect again...
            
            // Try to reconnect here...
            try_connect();
            return;
        }
        
        set_as_connected( client_->connection_ready() );
        
    }
    
    size_t SamsonConnection::getSize()
    {
        if( getType() == connection_output )
            return client_->getNumPendingOperations(); // It is not the size in bytes but at least is >0 if not all data is emitted
        else
            return 0;
    }
    
    
    // Overload method to push blocks using samsonClient
    void SamsonConnection::push( engine::Buffer* buffer )
    {
        if( getType() == connection_input )
            return; // Nothing to do if we are input
        
        // Report manually size ( not we are overloading Connection class )
        report_output_size( buffer->getSize() );
        
        // Push this block directly to the SAMSON client
        //client->push( queue , new BlockDataSource( buffer ) );
        client_->push( queue_ , buffer );
    }
    
    // Overwriteen method of SamsonClient
    void SamsonConnection::receive_buffer_from_queue(std::string queue , engine::Buffer* buffer)
    {
        // Transformation of buffer
        samson::KVHeader *header = (samson::KVHeader*) buffer->getData();
        
        if( header->isTxt() )
        {
            // Push the new buffer
            pushInputBuffer( buffer );
            
        }
        else
        {
            LM_W(("Received a binary buffer %s from %s. Still not implemented how to process this" 
                  , au::str( buffer->getSize() , "B" ).c_str() , getFullName().c_str() ));
        }
    }
    
    std::string SamsonConnection::getStatus()
    {
        if( !client_ )
            return "Not connected";
        
        if( client_->connection_ready() )
            return client_->getStatisticsString();
        else
            return "Trying to connect...";
            
    }
    
    
    
    SamsonAdaptor::SamsonAdaptor( 
                                 Channel * _channel 
                                 , ConnectionType _type 
                                 , 
                                 std::string _host 
                                 , int _port 
                                 , std::string _queue )
    : 
    Adaptor( _channel 
         , _type 
         , au::str("SAMSON(%s:%d)" , _host.c_str() , _port) ) 
    {
        // Information for connection
        host = _host;
        port = _port;
        queue = _queue;
                
    }
    
    // Get status of this element
    std::string SamsonAdaptor::getStatus()
    {
        return "OK";
    }
    
    
    void SamsonAdaptor::start_item()
    {
        // Add connection
        add( new SamsonConnection( this , getType() , host , port , queue ) );
        
    }

    void SamsonAdaptor::review_item()
    {
        
    }

    
}

