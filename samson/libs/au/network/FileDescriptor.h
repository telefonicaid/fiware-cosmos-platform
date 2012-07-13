

#ifndef _H_AU_FILE_DESCRIPTOR
#define _H_AU_FILE_DESCRIPTOR

#include "au/Rate.h"
#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/string.h"
#include "au/ThreadManager.h"

namespace au 
{
    // Class to manage a socket connection
    
    class FileDescriptor
    {
        // Andreu note: Use only LM_LM or LM_LW in this class since it is used in hoock function
        
        int fd;               // If id is -1 it means it was closed for some reason
        std::string name;     // name for debuging
        au::Token token;      // Token to protect multiple closes agains the same fd
        
    public:
        
        au::rate::Rate rate_in;    // Statistics about input output rate
        au::rate::Rate rate_out;
        
    public:
        
        FileDescriptor( std::string _name , int _fd ) : token( "FileDescriptor" )
        {
            name = _name;
            fd = _fd;
        }
        
        ~FileDescriptor()
        {
            close(); // Close if still open
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
        
        // Connection management
        void close();                 // Disconnect
        bool isDisconnected();        // Check if we are still connected
        
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
    
        // Invalidate: Put fd = -1 without closing to reuse this connection in other place
        int getFdAndinvalidate();
    };
    
    
}

#endif
