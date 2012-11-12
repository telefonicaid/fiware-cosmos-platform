/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef PACKET_H
#define PACKET_H

/* ****************************************************************************
*
* FILE                      packet.h
*
* DESCRIPTION				Definition of the packet to be exchanged in the samson-ecosystem
*
*/

#include "logMsg/logMsg.h"               // LM_TODO()

#include "au/utils.h"
#include "au/network/FileDescriptor.h"
#include "au/Status.h"
#include "au/Object.h"

#include "samson/common/samson.pb.h"     // google protocol buffers
#include "samson/common/EnvironmentOperations.h"        // str( network::Message* )

#include "engine/MemoryManager.h"        // MemoryManager
#include "engine/Engine.h"               // engine::Engine
#include "engine/Buffer.h"               // engine::Buffer
#include "engine/BufferContainer.h"

#include "Message.h"                     // samson::MessageType 

#include "samson/network/ClusterNodeType.h"
#include "samson/network/NodeIdentifier.h"

namespace samson
{
	/** 
	 Unique packet type sent over the network between workers and delilahs
	 */
    
    
	class Packet : public au::Object
	{

        engine::BufferContainer buffer_container;
        
	public:

        NodeIdentifier from;              // Identifier of the sender
        NodeIdentifier to;                // Identifier of the receiver

		Message::MessageCode  msgCode;    // Message code (sent in the header of the network interface)
        
		network::Message*     message;    // Message with necessary fields (codified using Google Protocol Buffers)
        
		bool                  disposable; // Message to be disposed if connection not OK
        
		Packet();
		Packet(Message::MessageCode _msgCode);
		Packet( Packet* p );

		~Packet();

        // Set and get buffer to be transmitted ( it is retained internally )
        void setBuffer( engine::Buffer * _buffer );
        engine::Buffer* getBuffer();
        
        // Debug string
        std::string str();        

        // Get size of this packet ( for statistical information )
        size_t getSize();
      
        // Handly function to create a packet with a certain type
        static Packet* messagePacket( std::string message );
        
        // Read and write functions
        au::Status read( au::FileDescriptor *fd , size_t *size );
        au::Status write( au::FileDescriptor *fd , size_t *size );

        
	}; 
}

#endif
