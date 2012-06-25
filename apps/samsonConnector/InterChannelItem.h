
#ifndef _H_SAMSON_CONNECTOR_INTERCHANNEL_ITEM
#define _H_SAMSON_CONNECTOR_INTERCHANNEL_ITEM

#include "au/network/PacketReaderWriter.h"
#include "au/network/SocketConnection.h"
#include "common.h"
#include "Item.h"
#include "message.pb.h"
#include "Connection.h"
#include "common.h"

#define InterChannelPacketHeader_magic_number 876875

namespace samson 
{
    namespace connector
    {
        
        
        // ------------------------------------------------------------------
        //
        // InterChannelPacket
        //
        // Unit of information exchanged in a InterChannel link
        //
        // ------------------------------------------------------------------
        
        typedef struct
        {
            int magic_number;    // Magic number to check sync
            size_t message_size; // Size of the companion GPB message
            size_t buffer_size;  // Optional buffer size
            
        } InterChannelPacketHeader;
        
        class InterChannelPacket : public au::Object
        {
            InterChannelPacketHeader header_;          // Header
            Message *message;                          // GPB Message with additional information
            engine::BufferContainer buffer_container_; // Optional buffer with data
            
        public:
            
            InterChannelPacket()
            {
                header_.magic_number = InterChannelPacketHeader_magic_number;
                message = new Message();
                header_.buffer_size = 0;
            }
            
            ~InterChannelPacket()
            {
                delete message;
            }

            // Get a pointer to the message to read or write
            Message* getMessage()
            {
                return message;
            }
            
            engine::Buffer* getBuffer()
            {
                return buffer_container_.getBuffer();
            }
            
            void setBuffer( engine::Buffer * buffer )
            {
                buffer_container_.setBuffer( buffer );
                if( buffer )
                    header_.buffer_size = buffer->getSize();
                else
                    header_.buffer_size = 0;
            }
            
            au::Status read( au::FileDescriptor * fd )
            {
                // Read a packet from file descriptor
                
                au::Status s = fd->partRead(&header_, sizeof(header_), "header", 0 );
                if( s != au::OK )
                    return s;

                // Reject excesive large packets
                if( header_.buffer_size > ( 256 * 1024 * 1024 ) )
                    return au::Error;
                if( header_.message_size > ( 256 * 1024 * 1024 ) )
                    return au::Error;

                
                // Read the GPB message
                if ( header_.message_size != 0 )
                {
                    char* dataP = (char*) calloc(1, header_.message_size + 1 );
                    
                    s = fd->partRead(dataP, header_.message_size, "Google Protocol Buffer" , 300);
                    if ( s != au::OK )
                    {
                        free(dataP);
                        return s;
                    }
                    
                    // Decode the google protocol buffer message
                    message->ParseFromArray( dataP , header_.message_size );
                    
                    if ( message->IsInitialized() == false )
                    {
                        
                        LM_E(("Error parsing Google Protocol Buffer of %lu bytes because is not initialized!",
                              header_.message_size ));
                        // Close connection ( We close here since it is not a io error, is a protocol error )
                        free(dataP);
                        fd->close();
                        return au::Error; // Generic error
                    }
                    
                    free(dataP);
                }
                
                // Create buffer and read it
                if( header_.buffer_size > 0 )
                {
                    
                    buffer_container_.create("InterChannelPacket", "streamConnector", header_.buffer_size );
                    s = fd->partRead( buffer_container_.getBuffer()->getData() 
                                     , header_.buffer_size 
                                     , "data"
                                     , 0 );

                    // Set the correct size for this buffer
                    engine::Buffer * buffer = buffer_container_.getBuffer();
                    buffer->setSize( header_.buffer_size );

                }
                if( s != au::OK )
                    return s;

                // Return OK
                return au::OK;
            }
            
            void recompute_sizes_in_header()
            {
                header_.message_size = message->ByteSize();
                engine::Buffer * buffer = buffer_container_.getBuffer();
                if( buffer )
                    header_.buffer_size = buffer->getSize();
                else
                    header_.buffer_size = 0;
            }
            
            au::Status write( au::FileDescriptor * fd )
            {
                // Write a packet to a file descriptor
                
                if( !is_valid_packet() )
                    return au::Error; 
                
                // Recompute size of the mssage in the header
                recompute_sizes_in_header();

                au::Status s = fd->partWrite(&header_, sizeof(header_), "header" );
                if( s != au::OK )
                    return s;

                //
                // Sending Google Protocol Buffer
                //
                if( header_.message_size > 0 )
                {
                    char* outputVec = (char*) malloc( header_.message_size + 2 );
                    if (outputVec == NULL)
                        LM_XP( 1 , ("malloc(%d)", header_.message_size ));
                    
                    if (message->SerializeToArray(outputVec, header_.message_size) == false)
                        LM_X( 1, ("SerializeToArray failed"));
                    
                    s = fd->partWrite( outputVec, header_.message_size, "Google Protocol Buffer" );
                    free(outputVec);
                    
                    if (s != au::OK)
                        LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s)));
                }
                
                engine::Buffer * buffer = buffer_container_.getBuffer();
                if( buffer )
                {
                    s = fd->partWrite(buffer->getData(), buffer->getSize(), "buffer" );
                    if( s != au::OK )
                        return s;
                }
                
                return au::OK;
            }
            
            bool is_valid_packet()
            {
                if( header_.magic_number != InterChannelPacketHeader_magic_number )
                    return false;

                return true;
            }
            
            size_t getSerializationSize()
            {
                
                // Recompute 
                recompute_sizes_in_header();
                
                size_t total = 0 ;
                total += sizeof( header_ );
                total += header_.message_size;
                total += header_.buffer_size;
                
                return total;
            }
            
            std::string str()
            {
                std::ostringstream output;
                output << "InterChannelPacket ( " << au::str(getSerializationSize() , "B") <<  " )";
                
                if ( !is_valid_packet() )
                {
                    output << "[ Non  valid ]";
                    return output.str();
                }
                
                // Message information
                output << "[ ";
                
                if( message->has_target_channel() )
                    output << "target_channel=" << message->target_channel() << " ";

                if( message->has_ack() )
                    output << "ack=" << message->ack() << " ";
                
                output << " ]";
                
                // Buffer at the end of the message
                engine::Buffer* buffer = buffer_container_.getBuffer();
                
                if( buffer )
                    output << "[ Buffer " << au::str( buffer->getSize() , "B" ) << " ]";
                else
                    output << "[ No buffer ]";
                    
                return output.str();
            }
            
        };

        
        // ------------------------------------------------------------------
        //
        // InterChannelLink
        //
        // Endpoint to write and receive InterChannelPacket
        //
        // ------------------------------------------------------------------
        
        
        class InterChannelLink
        {
            // Keep a pointer to the socket connection
            au::SocketConnection * socket_connection_; 
            
            // Two threads to read and write over this fd
            au::network::PacketReaderWriter<InterChannelPacket> * packet_reader_writer;
                        
        public:
            
            InterChannelLink( std::string name 
                             , au::SocketConnection * socket_connection 
                             , au::network::PacketReaderInteface<InterChannelPacket>* interface
                            )
            {
                // Keep for connection information
                socket_connection_ = socket_connection;
                
                // Init the reader writer
                packet_reader_writer = new au::network::PacketReaderWriter<InterChannelPacket>( name 
                                                                                  , socket_connection 
                                                                                  , interface );
            }
            
            ~InterChannelLink()
            {
                // Stop all involved threads
                packet_reader_writer->stop_threads();
                
                // Delete reader/writer
                delete packet_reader_writer;
            }

            // Push a packet
            void push( InterChannelPacket* packet )
            {
                packet_reader_writer->push(packet);
            }
            
            // Return If this can be removed looking at threads
            bool isRunning()
            {
                return packet_reader_writer->isRunning();
            }
            
            bool isConnected()
            {
                return packet_reader_writer->isConnected();
            }
            
            std::string getHostAndPort()
            {
                return socket_connection_->getHostAndPort();
            }
        
            // Cancel connection
            void close_socket_connection()
            {
                socket_connection_->close();
            }
            
        };
        
        
        
        class InputInterChannelConnection 
        : public Connection 
        , public au::network::PacketReaderInteface<InterChannelPacket>
        {
            InterChannelLink * link_;
          
            // Global pointer ( necessary to select channel )
            SamsonConnector * samson_connector_;

            // Selected channel
            std::string target_channel_;
            
            
        public:
            
            InputInterChannelConnection( SamsonConnector * samson_connector,
                                        std::string name 
                                        , au::SocketConnection * socket_connection )
            : Connection( NULL , connection_output , name ) // No item until we identify target channel
            {
                // Keep a pointer to SamsonConnector
                samson_connector_ = samson_connector;
                
                // Create link
                link_ = new InterChannelLink( name , socket_connection , this );
            }
            
            // au::network::PacketReaderInteface<InterChannelPacket>
            virtual void process_packet( InterChannelPacket* packet );
            
            // Close connection sending an error message to the other endpoint
            void close_connection( std::string error_message );
            
            
            virtual std::string getStatus()
            {
                if( link_->isConnected() )
                    return "Connected";
                else
                    return "Non Connected";
            }
            
            virtual bool canBeRemoved()
            {
                return !link_->isRunning();
            }
            
            // More information for this connection ( during first step )
            std::string getHostAndPort()
            {
                return link_->getHostAndPort();
            }
            
            
        };
        
        class OutputInterChannelConnection
        : public Connection 
        , public au::network::PacketReaderInteface<InterChannelPacket>
        {
            
            InterChannelLink * link_;

            bool hand_shake_finished;
            
        public:
            
            OutputInterChannelConnection( Item  * item , std::string name , au::SocketConnection * socket_connection )
            : Connection( item , connection_output , name )
            {
                link_ = new InterChannelLink( name , socket_connection , this );

                // Flag to indicate that we can start sending data
                hand_shake_finished = false;
                
            }
            
            void init_hand_shake( std::string target_channel )
            {
                // Initial packets to setup link
                InterChannelPacket * packet = new InterChannelPacket();
                
                // Select target channel
                packet->getMessage()->set_target_channel( target_channel );
                link_->push(packet);
            }
            
            virtual void process_packet( InterChannelPacket* packet )
            {
                Message* message = packet->getMessage();
                
                // Process incomming message from the other endpoint
                printf("Packet received %s\n" , packet->str().c_str() );
                
                // Hand shake confirmation
                if( message->has_ack() )
                    if( message->ack() == true )
                        hand_shake_finished = true;
                
            }

            virtual std::string getStatus()
            {
                if( link_->isConnected() )
                    return "Connected";
                else
                    return "Non Connected";
            }
            
            virtual bool canBeRemoved()
            {
                return !link_->isRunning();
            }
            
            virtual void review()
            {
                // Only start sending data if hand-shake is finished
                if( hand_shake_finished )
                {
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
                }
            };
            
            
        };
        
        
        class OutputInterChannelItem : public Item
        {
            
            // Information about connection
            std::string channel_name_;
            std::string host_;
            
            // Information about retrials
            au::Cronometer connection_cronometer;
            int connection_trials;
            
        public:
            
            OutputInterChannelItem( Channel * channel 
                                   , const std::string& host
                                   , const std::string& channel_name ) 
            : Item( channel 
                   , connection_output
                   , au::str("Connecting to %s:%s" , host.c_str() , channel_name.c_str() )
                   , au::str("CHANNEL(%s:%s)" , host.c_str() , channel_name.c_str() ) )
            {
                
                // Information for connection
                host_ = host;
                channel_name_ = channel_name;
                
                // Init cronometer and trials counter
                connection_cronometer.reset();
                connection_trials = 0;
                
                // Review item to establish connection
                try_connect();

            }
            
            void try_connect()
            {
                au::SocketConnection* socket_connection;
                au::Status s = au::SocketConnection::newSocketConnection( host_
                                                                         , SAMSON_CONNECTOR_INTERCHANNEL_PORT
                                                                         , &socket_connection);                                  
                if( s == au::OK )
                {
                    std::string name = au::str( "channel list to %s:%s" , host_.c_str() , channel_name_.c_str() );
                    
                    // Create a link to interact with this
                    OutputInterChannelConnection* connection = new  OutputInterChannelConnection( this 
                                                                                                 , name 
                                                                                                 , socket_connection );
                    // Add connections to my list
                    add( connection );
                    
                    // Add handshare connection
                    connection->init_hand_shake( channel_name_ );

                    // Reset counters of reconnection
                    connection_cronometer.reset();
                    connection_trials = 0;
                }
                else
                    connection_trials++;
                
            }
            
            
            // Get status of this element
            std::string getStatus()
            {
                if( getNumConnections() > 0 )
                    return "connected";
                
                return au::str("connecting... [ %d trials %s ] )" , connection_trials , au::str_time( connection_cronometer.diffTimeInSeconds() ).c_str() );
            }
            
            void review_item()
            {
                if( getNumConnections() > 0 )
                    return; // Connection is established
                if( connection_cronometer.diffTimeInSeconds() < 3 )
                    return; // No retray
                try_connect();
            }
            
            bool canBeRemoved()
            {
                return true; // No problem to remove this
            }
            
        };
        
        class InputInterChannelItem : public Item
        {
            
            // Information about retrials
            au::Cronometer connection_cronometer;
            int connection_trials;
            
        public:
            
            InputInterChannelItem( Channel * channel ) 
            : Item( channel 
                   , connection_input 
                   , au::str("Listening inter chnanel")
                   , au::str("CHANNELS(*)" ) )
            {
            }
            
            
            // Get status of this element
            std::string getStatus()
            {
                return au::str("%lu connections" , getNumConnections() );
            }
            
            void review_item()
            {
                
            }
            
            bool canBeRemoved()
            {
                return true; // No problem to remove this
            }
            
            // Check if we accept a particular connection
            // We have not implemented any criteria to reject this connection
            virtual bool accept( InputInterChannelConnection* connection )
            {
                return true;
            }

            
        };
      
        
        
        
    }
}


#endif