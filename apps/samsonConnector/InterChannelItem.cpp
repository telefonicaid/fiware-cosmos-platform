
#include "SamsonConnector.h"
#include "FileDescriptorConnection.h"

#include "InterChannelItem.h" // Own interfave

namespace samson {
    namespace connector {
 
        
        
        // ----------------------------------------------------------------
        //
        // InputInterChannelConnection
        //
        // ----------------------------------------------------------------
        
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
                samson_connector_->select_channel( this , target_channel_ , &error );
                
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

    }
}