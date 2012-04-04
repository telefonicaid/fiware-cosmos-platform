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
    
    const char* ClusterNodeType2str( ClusterNodeType type )
    {
        switch ( type ) 
        {
            case DelilahNode: return "delilah";
            case WorkerNode:  return "worker";
            case UnknownNode: return "unknown";
        }
        
        LM_X(1, ("Impossible to got here"));
        return "Error";
    }

    
    au::Status Packet::write( au::FileDescriptor *fd , size_t *size )
    {
        au::Status           s;
        Message::Header  header;
        
        *size =0;
        
        LM_T(LmtSocketConnection, ("Sending Packet '%s' to %s " , str().c_str() , fd->getName().c_str() ));
        
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
                fd->close();
                return au::Error; // Generic error
            }
            
            free(dataP);
        }
        
        if (header.kvDataLen != 0)
        {
            // Alloc a buffer to read buffer of data
            std::string buffer_name = au::str("Network Buffer from %s" , fd->getName().c_str() );
            buffer = engine::MemoryManager::shared()->newBuffer( buffer_name , "network" , header.kvDataLen , 0.9 );
            
            char*  kvBuf  = buffer->getData();
            s = fd->partRead(kvBuf, header.kvDataLen , "Key-Value Data" , 300);
            
            if (s != au::OK)
                return s;

            LM_T(LmtSocketConnection, ("Read %d bytes of KV DATA from '%s'", header.kvDataLen, fd->getName().c_str()));
            *size += header.kvDataLen;
            
            buffer->setSize( header.kvDataLen );
        }
        
        return au::OK;
    }
}
