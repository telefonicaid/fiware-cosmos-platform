
#ifndef _H_SAMSON_CONNECTOR_INTERCHANNEL_ITEM
#define _H_SAMSON_CONNECTOR_INTERCHANNEL_ITEM

#include "au/network/PacketReaderWriter.h"
#include "au/network/SocketConnection.h"

#include "engine/ProcessItem.h"

#include "common.h"
#include "Adaptor.h"
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
            
            size_t getSize()
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
                output << "InterChannelPacket ( " << au::str(getSize() , "B") <<  " )";
                
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

            void push( au::ObjectList<InterChannelPacket>* packets )
            {
                while( true )
                {
                    au::ObjectContainer<InterChannelPacket> packet_container;
                    packets->extract_front( packet_container );
                    
                    InterChannelPacket* packet = packet_container.getObject();
                    if( packet )
                        push( packet );
                    else
                        return; // No more packets to be push
                }
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
            
            // Stop all threads
            void close_and_stop()
            {
                socket_connection_->close();
                packet_reader_writer->stop_threads();
            }
            
            size_t getBufferedSize()
            {
                return packet_reader_writer->getOutputBufferedSize();
            }
            
            void extract_pending_packets( au::ObjectList<InterChannelPacket>* packets )
            {
                packet_reader_writer->extract_pending_packets(packets);
                
            }


            
        };
        
        
        
        class InputInterChannelConnection 
        : public Connection 
        , public au::network::PacketReaderInteface<InterChannelPacket>
        {
            // Real element to exchange data using "InterChannelPacket"
            InterChannelLink * link_;
          
            // Keep a poiter to the socket connection
            au::SocketConnection * socket_connection_;
            
            // Global pointer ( necessary to select channel )
            StreamConnector * stream_connector_;

            // Keep information about host
            std::string host_name_;
            
            // Selected channel
            std::string target_channel_;
            std::string source_channel_name_; // Information about the source
            
        public:
            
            InputInterChannelConnection( StreamConnector * stream_connector,
                                        std::string host_name 
                                        , au::SocketConnection * socket_connection )
            : Connection( NULL , connection_output , getName( host_name , "???") ) // No item until we identify target channel
            {
                if( !socket_connection )
                    LM_X(1, ("Internal error"));
                
                // Keep a pointer to SamsonConnector
                stream_connector_ = stream_connector;
                
                // Keep a pointer to the socket connection
                socket_connection_ = socket_connection;

                // Keep the host name for updating description when channel name is available
                host_name_ = host_name;
                
                // No link until start_connection is called
                link_ = NULL;
            }
            
            static std::string getName( std::string host , std::string channel )
            {
                return "from channel " + host + ":" + channel; 
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

            virtual void review_connection()
            {
                if( link_ )
                    set_as_connected( link_->isConnected() );
            }
            
            virtual void start_connection()
            {
                if( link_ )
                    return;
                
                // Create link
                link_ = new InterChannelLink( getFullName() , socket_connection_ , this );
            }

            virtual void stop_connection()
            {
                if( link_ )
                    link_->close_and_stop();
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
            
            // Information about connection
            std::string channel_name_;
            std::string host_;
            
            // Information about retrials
            au::Cronometer connection_cronometer;
            int connection_trials;

            // Link ( when established )
            InterChannelLink * link_;

            // Last error while trying to connect
            std::string last_error;
            
            // finish handshare
            bool hand_shake_finished;
            
            // List of pending packets from previous connection
            au::ObjectList<InterChannelPacket> pending_packets;
            
        public:
            
            OutputInterChannelConnection( Item  * item 
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
            
            void init_hand_shake( std::string target_channel )
            {
                // Initial packets to setup link
                InterChannelPacket * packet = new InterChannelPacket();
                
                // Select target channel
                packet->getMessage()->set_target_channel( target_channel );
                packet->getMessage()->set_source_channel( getFullName() );
                link_->push(packet);
            }
            
            virtual void process_packet( InterChannelPacket* packet )
            {
                
                Message* message = packet->getMessage();
                                
                // Hand shake confirmation
                if( message->has_ack() )
                    if( message->ack() == true )
                        hand_shake_finished = true;
                
            }

            virtual std::string getStatus()
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
            virtual size_t getBufferedSize()
            {
                // Base size ( internal list of buffers in class Connection )
                size_t total = Connection::getBufferedSize();
                
                if( link_ )
                    total += link_->getBufferedSize();
                
                return total;
            }

            
            virtual void start_connection()
            {
                if( !link_ )
                {
                    try_connect();
                    return;
                }
            }
            
            virtual void stop_connection()
            {
                // Close all threads related with this connection
                if( link_ )
                    link_->close_and_stop();
            }
            
            virtual void review_connection()
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
                    return;
                }
                
                set_as_connected( link_->isConnected() );
                
                // Only start sending data if hand-shake is finished
                if( hand_shake_finished )
                {
                    
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
            
            void try_connect()
            {
                if( link_ ) 
                    return; // Already connected
                
                au::SocketConnection* socket_connection;
                au::Status s = au::SocketConnection::newSocketConnection( host_
                                                                         , SAMSON_CONNECTOR_INTERCHANNEL_PORT
                                                                         , &socket_connection);                                  
                
                if( s == au::OK )
                {
                    
                    link_ = new InterChannelLink( "link_" + getFullName() , socket_connection , this );

                    // Push pending packets ( if any )
                    link_->push( &pending_packets );
                    pending_packets.clear();
                    
                    // Init all hand shake for this connection
                    init_hand_shake( channel_name_ );
                    
                    // Reset counters of reconnection
                    connection_cronometer.reset();
                    connection_trials = 0;
                }
                else
                {
                    last_error = au::status(s);
                    connection_trials++;
                }
                
            }

            
            
        };
        
        
        class OutputChannelAdaptor : public Item
        {
            
            // Information about connection
            std::string channel_name_;
            std::string host_;
            
            
        public:
            
            OutputChannelAdaptor( Channel * channel 
                                   , const std::string& host
                                   , const std::string& channel_name ) 
            : Item( channel 
                   , connection_output
                   , au::str("CHANNEL(%s:%s)" , host.c_str() , channel_name.c_str() ) )
            {
                
                // Information for connection
                host_ = host;
                channel_name_ = channel_name;
            }
            
            virtual void start_item()
            {
                // Create a single connection for this item
                add( new OutputInterChannelConnection( this , host_ , channel_name_ ) );
            };
            
            // Get status of this element
            std::string getStatus()
            {
                return "Ok";
            }
            
            void review_item()
            {
                // Nothing to do here
            }
            
        };
        
        class InputChannelAdaptor : public Item
        {
            
            // Information about retrials
            au::Cronometer connection_cronometer;
            int connection_trials;
            
        public:
            
            InputChannelAdaptor( Channel * channel ) 
            : Item( channel 
                   , connection_input 
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