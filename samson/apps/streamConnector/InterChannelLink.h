#ifndef _H_SAMSON_CONNECTOR_INTERCHANNEL_LINK
#define _H_SAMSON_CONNECTOR_INTERCHANNEL_LINK

#include "au/network/PacketReaderWriter.h"
#include "au/network/SocketConnection.h"

#include "engine/ProcessItem.h"

#include "common.h"
#include "Adaptor.h"
#include "message.pb.h"
#include "Connection.h"
#include "common.h"
#include "InterChannelPacket.h"

namespace stream_connector {
    
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
                         );
        ~InterChannelLink();
        
        // Push a packet
        void push( InterChannelPacket* packet );
        void push( au::ObjectList<InterChannelPacket>* packets );

        void close_socket_connection();
        void close_and_stop();

        // Return If this can be removed looking at threads
        bool isRunning();
        bool isConnected();

        std::string getHostAndPort();
        size_t bufferedSize();

        void extract_pending_packets( au::ObjectList<InterChannelPacket>* packets );
        
    };
}

#endif
    