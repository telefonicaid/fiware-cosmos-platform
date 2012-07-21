
#include "StreamConnector.h"
#include "FileDescriptorConnection.h"

#include "ChannelAdaptor.h" // Own interfave

namespace stream_connector {
    
    // ----------------------------------------------------------------
    //
    // InputInterChannelConnection
    //
    // ----------------------------------------------------------------

    
    InputInterChannelConnection::InputInterChannelConnection( StreamConnector * stream_connector,
                                std::string host_name 
                                , au::SocketConnection * socket_connection )
    : Connection( NULL , connection_output , getName( host_name , "???") ) // No item until we identify target channel
    {
        if( !socket_connection )
            LM_X(1, ("Internal error"));
        
        // Keep a pointer to StreamConnector
        stream_connector_ = stream_connector;
        
        // Keep a pointer to the socket connection
        socket_connection_ = socket_connection;
        
        // Keep the host name for updating description when channel name is available
        host_name_ = host_name;
        
        // No link until start_connection is called
        link_ = NULL;
    }
    
    std::string InputInterChannelConnection::getName( std::string host , std::string channel )
    {
        return "from channel " + host + ":" + channel; 
    }
    
    
    std::string InputInterChannelConnection::getStatus()
    {
        if( link_->isConnected() )
            return "Connected";
        else
            return "Non Connected";
    }
    
    void InputInterChannelConnection::review_connection()
    {
        if( link_ )
            set_as_connected( link_->isConnected() );
    }
    
    void InputInterChannelConnection::start_connection()
    {
        if( link_ )
            return;
        
        // Create link
        link_ = new InterChannelLink( getFullName() , socket_connection_ , this );
    }
    
    void InputInterChannelConnection::stop_connection()
    {
        if( link_ )
            link_->close_and_stop();
    }
    
    // More information for this connection ( during first step )
    std::string InputInterChannelConnection::getHostAndPort()
    {
        return link_->getHostAndPort();
    }
    
    void InputInterChannelConnection::process_packet( InterChannelPacket* packet )
    {
        Message * message = packet->getMessage();
        
        
        // Informas about source data
        if( message->has_source_channel() )
        {
            source_channel_name_ = message->source_channel();
            setDescription( getName( host_name_ , source_channel_name_ ) );
        }
        
        // Select target channel...
        if( message->has_target_channel() )
        {
            // New channel
            std::string target_channel = message->target_channel();
            
            if( ( target_channel_ != "" ) && ( target_channel_ != target_channel ) )
                return close_connection("Not possible to select another channel");
            
            // Switch to this channel
            target_channel_ = target_channel;
            au::ErrorManager error;
            stream_connector_->select_channel( this , target_channel_ , &error );
            
            if( error.isActivated() )
                return close_connection( error.getMessage() );
            
            // Send ACK
            InterChannelPacket* packet = new InterChannelPacket();
            Message* message = packet->getMessage();
            message->set_ack(true);
            link_->push(packet);
            return;
        }
        
        // If buffer, push it ( no transformation at the moment )
        engine::Buffer* buffer = packet->getBuffer();
        if( buffer )
            pushInputBuffer(buffer);
        
    }
    
    void InputInterChannelConnection::close_connection( std::string error_message )
    {
        // Send error message to the input connection for debugging
        InterChannelPacket* packet = new InterChannelPacket();
        Message* message = packet->getMessage();
        message->set_error( error_message );
        link_->push(packet);
        
        // Sleep a little bit to make sure this pakcet is received?
        
        // Close socket connection phisically
        link_->close_socket_connection();
    }
    
    // ----------------------------------------------------------------
    //
    // OutputInterChannelConnection
    //
    // ----------------------------------------------------------------

    OutputInterChannelConnection::OutputInterChannelConnection( Adaptor  * item 
                                                               , std::string host 
                                                               , std::string channel_name 
                                                               )
    : Connection( item , connection_output , au::str("to channel %s:%s" , host.c_str() , channel_name.c_str() ))
    {
        // At the moment not connected to anyone
        link_ = NULL;
        
        // Keep information for this connection
        host_ = host;
        channel_name_ = channel_name;
        
        // Init cronometer and trials counter
        connection_cronometer.reset();
        connection_trials = 0;
        
        // Flag to indicate that we can start sending data
        hand_shake_finished = false;
        
    }
    
    void OutputInterChannelConnection::init_hand_shake( std::string target_channel )
    {
        // Initial packets to setup link
        InterChannelPacket * packet = new InterChannelPacket();
        
        // Select target channel
        packet->getMessage()->set_target_channel( target_channel );
        packet->getMessage()->set_source_channel( getFullName() );
        link_->push(packet);
    }
    
    void OutputInterChannelConnection::process_packet( InterChannelPacket* packet )
    {
        
        Message* message = packet->getMessage();
        
        // Hand shake confirmation
        if( message->has_ack() )
            if( message->ack() == true )
                hand_shake_finished = true;
        
    }
    
    std::string OutputInterChannelConnection::getStatus()
    {
        
        if( !link_ )
        {
            return au::str("Connecting... [ %d trials %s (last error %s) ] )" 
                           , connection_trials 
                           , connection_cronometer.str().c_str()
                           , last_error.c_str() );
        }
        
        if( link_->isConnected() )
            return "Connected";
        else
            return "Non Connected";
    }
    
    // Get currect size accumulated here
    size_t OutputInterChannelConnection::getBufferedSize()
    {
        // Base size ( internal list of buffers in class Connection )
        size_t total = Connection::getBufferedSize();
        
        if( link_ )
            total += link_->getBufferedSize();
        
        return total;
    }
    
    
    void OutputInterChannelConnection::start_connection()
    {
        if( !link_ )
        {
            try_connect();
            return;
        }
    }
    
    void OutputInterChannelConnection::stop_connection()
    {
        // Close all threads related with this connection
        if( link_ )
            link_->close_and_stop();
    }
    
    void OutputInterChannelConnection::review_connection()
    {
        if( !link_ )
            set_as_connected( false );
        
        // If link_ is not valid any more, just remove it...
        if( link_ && !link_->isConnected() )
        {
            // Close connection
            link_->close_and_stop();
            
            // Reset the handshake flag
            hand_shake_finished = false;
            
            // Recover unsent packets
            link_->extract_pending_packets( &pending_packets );
            
            delete link_;
            link_ = NULL;
        }
        
        if( !link_ )
        {
            if( connection_cronometer.diffTimeInSeconds() < 3 )
                return; // No retray at the moment
            
            try_connect();
            
            connection_cronometer.reset();
            
            return;
        }
        
        set_as_connected( link_->isConnected() );
        
        // Only start sending data if hand-shake is finished
        if( hand_shake_finished )
        {
            // Reset counters of reconnection
            connection_trials = 0;

            
            // Encapsulate generated buffers in packets
            while( true )
            {
                // Check generated packed included size in link_ is not too large ( always in memory )
                if( link_->getBufferedSize() > 256*1024*1024 )
                    break;
                
                // Recover next generated buffer
                engine::BufferContainer buffer_container;
                getNextBufferToSent(&buffer_container);
                engine::Buffer * buffer = buffer_container.getBuffer();
                
                if( buffer )
                {
                    // Put buffer in a packet and send
                    InterChannelPacket * packet = new InterChannelPacket();
                    packet->setBuffer(buffer);
                    link_->push(packet);
                }
                else
                    break; // No more generated packetd
            }
        }
    };
    
    // Type to establish this connection with remote server
    
    void OutputInterChannelConnection::try_connect()
    {
        if( link_ ) 
            return; // Already connected
        
        au::SocketConnection* socket_connection;
        au::Status s = au::SocketConnection::newSocketConnection( host_
                                                                 , SAMSON_CONNECTOR_INTERCHANNEL_PORT
                                                                 , &socket_connection);                                  
        
        connection_trials++;
        
        if( s == au::OK )
        {
            
            link_ = new InterChannelLink( "link_" + getFullName() , socket_connection , this );
            
            // Push pending packets ( if any )
            link_->push( &pending_packets );
            pending_packets.clear();
            
            // Init all hand shake for this connection
            init_hand_shake( channel_name_ );
        }
        else
        {
            last_error = au::status(s);
        }
        
    }
    

    
}