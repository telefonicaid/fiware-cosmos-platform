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

#include "samson/common/samson.pb.h"     // google protocol buffers
#include "samson/common/EnvironmentOperations.h"        // str( network::Message* )

#include "engine/MemoryManager.h"        // MemoryManager
#include "engine/Object.h"
#include "engine/Engine.h"               // engine::Engine
#include "engine/Buffer.h"               // engine::Buffer
#include "Message.h"                     // samson::MessageType 

namespace samson
{
	/** 
	 Unique packet type sent over the network between workers and delilahs
	 */
    
    // Type of elements in the cluster
    typedef enum 
    {
        DelilahNode,
        WorkerNode,
        UnknownNode
    } ClusterNodeType;
    
    const char* ClusterNodeType2str( ClusterNodeType type );
    
    
    class NodeIdentifier
    {
        public:
        
        ClusterNodeType node_type;
        size_t id;
        
        NodeIdentifier()
        {
            node_type = UnknownNode;
            id = (size_t)-1;
        }
                
        NodeIdentifier( network::NodeIdentifier pb_node_identifier  )
        {
           
            switch ( pb_node_identifier.node_type() ) 
            {
                case network::NodeIdentifier_NodeType_Delilah:
                    node_type = DelilahNode;
                    break;
                case network::NodeIdentifier_NodeType_Worker:
                    node_type = WorkerNode;
                    break;
                case network::NodeIdentifier_NodeType_Unknown:
                    node_type = UnknownNode;
                    break;
                    
                default:
                    node_type = UnknownNode;
                    break;

            }
            
            id = pb_node_identifier.id();
        }

        void fill( network::NodeIdentifier* pb_node_identifier )
        {
            switch ( node_type ) 
            {
                case DelilahNode:
                    pb_node_identifier->set_node_type( network::NodeIdentifier_NodeType_Delilah );
                    break;
                case WorkerNode:
                    pb_node_identifier->set_node_type( network::NodeIdentifier_NodeType_Worker );
                    break;
                case UnknownNode:
                    pb_node_identifier->set_node_type( network::NodeIdentifier_NodeType_Unknown );
                    break;
            }            
            pb_node_identifier->set_id(id);
        }
        
        NodeIdentifier ( ClusterNodeType _node_type , size_t _id )
        {
            node_type = _node_type;
            id = _id;
        }

        bool operator==(const NodeIdentifier&  other)
        {
            if ( node_type != other.node_type )
                return false;
            if ( id != other.id )
                return false;
            
            return true;
        }
        
        std::string str()
        {
            if( id == (size_t) -1 )
                return au::str("%s:Unknown" , ClusterNodeType2str( node_type ) );
                
            return au::str("%s:%lu" , ClusterNodeType2str( node_type ) , id );
        }

        std::string getCodeName()
        {
            if( node_type == DelilahNode )
            {
                std::string code_id = au::code64_str(id);
                return au::str("%s_%s" , ClusterNodeType2str( node_type ) ,  code_id.c_str() );
            }
            else
                return au::str("%s_%lu" , ClusterNodeType2str( node_type ) , id );
                
        }
        
        bool isDelilahOrUnknown()
        {
            switch (node_type) 
            {
                case DelilahNode: 
                case UnknownNode: 
                    return true;
                case WorkerNode: 
                    return false;
            }
            
            LM_X(1, ("Unknown error"));
            return false;
        }
        
        
        
        
    };
    
	class Packet : public engine::Object
	{
        
	public:

        NodeIdentifier from;      // Identifier of the sender
        NodeIdentifier to;        // Identifier of the receiver

		Message::MessageCode  msgCode;    // Message code (sent in the header of the network interface)
        
		network::Message*     message;    // Message with necessary fields (codified using Google Protocol Buffers)
        
		engine::Buffer*       buffer;     // Data for key-values
        
		bool                  disposable; // Message to be disposed if connection not OK


		Packet()
		{
			msgCode    = Message::Unknown;
			buffer     = NULL;
			message    = new network::Message();
			disposable = false;
		};        
        
		Packet(Message::MessageCode _msgCode)
		{
			msgCode    = _msgCode;
			buffer     = NULL;
			message    = new network::Message();
			disposable = false;
		};

		Packet(Packet* p)
		{
			// Copy the message type
			msgCode    = p->msgCode;
			disposable = p->disposable;

			// Copy the buffer (if any)
			if (p->buffer)
			{
                std::string name = au::str("Copy of '%s'" , p->buffer->getName().c_str() );
				buffer = engine::MemoryManager::shared()->newBuffer( name , p->buffer->getType() , p->buffer->getSize() );
				memcpy(buffer->getData(), p->buffer->getData(), p->buffer->getSize() );
				buffer->setSize( p->buffer->getSize() );
			}
			else
				buffer = NULL;

			// Google protocol buffer message
			message = new network::Message();
			message->CopyFrom(*p->message);
		};


		~Packet()
		{
			// Note: Not remove buffer since it can be used outside the scope of this packet
			delete message;
		}
        
        
        // Debug string
        std::string str()
        {
            std::ostringstream output;
            output << "Packet " << messageCode( msgCode );

            // Extra information for worker command
            if( msgCode == Message::WorkerCommand )
                output << "(W-Command: " << message->worker_command().command() << ")";

            if( msgCode == Message::WorkerCommandResponse )
                output << "(W-CommandResponse: " << message->worker_command_response().worker_command().command() << ")";
            
            
            if ( buffer )
                output << " [ Buffer " << au::str(  buffer->getSize() ) << "/" << au::str(  buffer->getMaxSize() ) << " ]" ;
            return output.str();
        }
        
        
        size_t getSize()
        {
            size_t total = 0;
            if ( buffer )
                total += buffer->getSize();
            
            total += message->ByteSize();
            
            return total;
        }
      
        static Packet* messagePacket( std::string message )
        {
            Packet * packet = new Packet( Message::Message );
            packet->message->set_message(message);
            return packet;
        }
        
        void freeBuffer()
        {
            if( buffer )
            {
                engine::MemoryManager::shared()->destroyBuffer(buffer);
                buffer = NULL;
            }
        }
        
        
        // Read and write functions
        au::Status read( au::FileDescriptor *fd , size_t *size );
        au::Status write( au::FileDescriptor *fd , size_t *size );

        
	};
}

#endif
