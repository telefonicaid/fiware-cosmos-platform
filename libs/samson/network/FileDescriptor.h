

#ifndef _H_SAMSON_FILE_DESCRIPTOR
#define _H_SAMSON_FILE_DESCRIPTOR

#include "au/Token.h"
#include "au/string.h"
#include "samson/common/status.h"
#include "samson/network/Packet.h"

namespace samson 
{
    // Class to manage a socket connection
    
    class FileDescriptor
    {
        
        int fd;               // If id is -1 it means it was closed for some reason
        std::string name;     // name for debuging
        au::Token token;      // Token to protect multiple closes agains the same fd
        
    public:
        
        FileDescriptor( std::string _name , int _fd ) : token( "FileDescriptor" )
        {
            name = _name;
            fd = _fd;
        }
        
        int getFd()
        {
            return fd;
        }
        
        std::string getName()
        {
            return name;
        }

        void setName( std::string _name )
        {
            name = _name;
        }

        
        // Connection mnagement
        void close();                 // Disconnect
        bool isDisconnected();        // Check if we are still connected
        
        // Read and write packet routines
        Status readPacket( Packet * packetP , size_t *size);
        Status writePacket( Packet * packetP , size_t *size );
        
        // Read & Write line 
        Status readLine( char* line, size_t max_size , int max_seconds );
        Status writeLine( const char* line, int retries, int tv_sec, int tv_usec);
        
        // Read buffer
        Status readBuffer(char* line, size_t max_size, int tmoSecs);
        
        // Debug string
        std::string str();
                
        // Auxiliar function to read and write to the socket
        Status okToSend(int tries, int tv_sec, int tv_usec);
        Status msgAwait( int secs, int usecs, const char* what );
        
        Status partWrite( const void* dataP, int dataLen, const char* what, int retries = 300, int tv_sec = 1, int tv_usec = 0);
        Status partRead( void* vbuf, size_t bufLen, const char* what , int max_seconds , size_t * readed_size = NULL );
        
    };
    
    
    
}

#endif