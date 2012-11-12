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
/* *******************************************************************************
*
* FILE                     Packet.cpp
*
* DESCRIPTION              Definition of the packet to be exchange in the samson-ecosystem
*
*/
#include "Packet.h"             // Own interface

namespace samson
{
    
    
    Packet::Packet()
    {
        msgCode    = Message::Unknown;
        message    = new network::Message();
        disposable = false;
    };        
    
    Packet::Packet(Message::MessageCode _msgCode)
    {
        msgCode    = _msgCode;
        message    = new network::Message();
        disposable = false;
    };
    
    Packet::Packet( Packet* p )
    {
        // Copy the message type
        msgCode    = p->msgCode;
        disposable = p->disposable;
        
        // Point to the same buffer
        //buffer_container.setBuffer( p->buffer_container.getBuffer() );
        buffer_container = p->buffer_container;
        
        // Google protocol buffer message
        message = new network::Message();
        message->CopyFrom(*p->message);
    };
    
    Packet::~Packet()
    {
        // Buffer contained in buffer_container is auto-released
        
        delete message;
    }


    void Packet::setBuffer( engine::Buffer * _buffer )
    {
        // Handy function to get the buffer
        buffer_container.setBuffer(_buffer);
    }
    
    engine::Buffer* Packet::getBuffer()
    {
        // Handy function to get the buffer
        return  buffer_container.getBuffer();
    }
    
    au::Status Packet::write( au::FileDescriptor *fd , size_t *size )
    {
        au::Status           s;
        Message::Header  header;
        
        *size =0;
        
        LM_T(LmtSocketConnection, ("Sending Packet '%s' to %s " , str().c_str() , fd->getName().c_str() ));
        
        // Get a pointer to the buffer ( if any )
        engine::Buffer * buffer = buffer_container.getBuffer();
        
        //
        // Preparing header
        //
        
        memset(&header, 0, sizeof(header));
        
        header.code        = msgCode;
        header.gbufLen     = message->ByteSize();        
        header.kvDataLen   = buffer?buffer->getSize():0;
        
        // Set magic number
        header.setMagicNumber();
        
        //
        // Sending header
        //
        s = fd->partWrite( &header, sizeof(header), "header");
        *size += sizeof(header);
        
        if (s != au::OK)
        {
            //LM_RE(s, ("partWrite:header(%s): %s", name.c_str(), au::status(s)));
            return s;
        }
        //
        // Sending Google Protocol Buffer
        //
        if( header.gbufLen != 0 )
        {
            char* outputVec;
            
            outputVec = (char*) malloc(header.gbufLen + 2);
            if (outputVec == NULL)
                LM_XP( 1 , ("malloc(%d)", header.gbufLen));
            
            if (message->SerializeToArray(outputVec, header.gbufLen) == false)
                LM_X( 1, ("SerializeToArray failed"));
            
            s = fd->partWrite( outputVec, header.gbufLen, "Google Protocol Buffer" );
            free(outputVec);
            if (s != au::OK)
                LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s)));
            *size += header.gbufLen;
        }
        
        if ( buffer != 0 )
        {
            s = fd->partWrite( buffer->getData(), buffer->getSize(), "KV data" );
            if (s != au::OK )
                LM_RE(s, ("partWrite returned %d and not the expected %d", s, buffer->getSize()));
            *size += buffer->getSize();
        }
        
        return au::OK;
    }    

    au::Status Packet::read( au::FileDescriptor *fd , size_t *size )
    {
        au::Status s;
        Message::Header  header;
        
        LM_T(LmtSocketConnection, ("SocketConnection %s: Reading packet" , str().c_str() ));
        *size = 0;
        
        s = fd->partRead(&header, sizeof(Message::Header), "Header" , 300 );  // Timeout 300 secs for next packet
        if (s != au::OK)
        {
            return s;
        }
        *size += sizeof(Message::Header);
        
        // Check header
        if ( !header.check() )
        {
            fd->close(); // Close connection ( We close here since it is not a io error, is a protocol error )
            LM_E(("Error checking received header from %s" , fd->getName().c_str() ));
            return au::Error; // Generic error
        }
        
        // Get the message code from header
        msgCode = header.code;
        
        if (header.gbufLen != 0)
        {
            char* dataP = (char*) calloc(1, header.gbufLen + 1);
            
            s = fd->partRead(dataP, header.gbufLen, "Google Protocol Buffer" , 300);
            if ( s != au::OK )
            {
                free(dataP);
                return s;
            }
            *size += header.gbufLen;
            
            LM_T(LmtSocketConnection, ("Read %d bytes of GOOGLE DATA from '%s'", header.gbufLen, fd->getName().c_str() ));
            
            // Decode the google protocol buffer message
            message->ParseFromArray(dataP, header.gbufLen);
            
            if ( message->IsInitialized() == false )
            {
                
                LM_E(("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!",
                      header.gbufLen, samson::Message::messageCode(header.code)));
                // Close connection ( We close here since it is not a io error, is a protocol error )
                free(dataP);
                fd->close();
                return au::Error; // Generic error
            }
            
            free(dataP);
        }
        
        if (header.kvDataLen != 0)
        {
            // Alloc a buffer to read buffer of data
            std::string buffer_name = au::str("Network Buffer from %s" , fd->getName().c_str() );
            engine::Buffer *buffer = engine::MemoryManager::shared()->createBuffer( buffer_name , "network" , header.kvDataLen , 0.9 );
            
            char*  kvBuf  = buffer->getData();
            s = fd->partRead(kvBuf, header.kvDataLen , "Key-Value Data" , 300);
            
            if (s != au::OK)
                return s;

            LM_T(LmtSocketConnection, ("Read %d bytes of KV DATA from '%s'", header.kvDataLen, fd->getName().c_str()));
            *size += header.kvDataLen;
            
            buffer->setSize( header.kvDataLen );
            
            // Put inside the buffer_container and release the created buffer
            // From now on, it released (at least ) internally in buffer_container 
            buffer_container.setBuffer(buffer);
            buffer->release();
            
        }
        
        return au::OK;
    }
    
    Packet* Packet::messagePacket( std::string message )
    {
        Packet * packet = new Packet( Message::Message );
        packet->message->set_message(message);
        return packet;
    }

    std::string Packet::str()
    {
        std::ostringstream output;
        output << "Packet " << messageCode( msgCode );
        
        // Extra information for worker command
        if( msgCode == Message::WorkerCommand )
            output << "(W-Command: " << message->worker_command().command() << ")";
        
        if( msgCode == Message::WorkerCommandResponse )
            output << "(W-CommandResponse: " << message->worker_command_response().worker_command().command() << ")";
        
        engine::Buffer* buffer = buffer_container.getBuffer();
        if ( buffer )
            output << " [ Buffer " << au::str(  buffer->getSize() ) << "/" << au::str(  buffer->getMaxSize() ) << " ]" ;
        return output.str();
    }
    
    
    size_t Packet::getSize()
    {
        size_t total = 0;
        
        engine::Buffer* buffer = buffer_container.getBuffer();
        if ( buffer )
            total += buffer->getSize();
        
        total += message->ByteSize();
        
        return total;
    }
    

}
