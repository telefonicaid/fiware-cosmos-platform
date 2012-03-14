

#ifndef _H_SAMSON_SOCKET_CONNECTION
#define _H_SAMSON_SOCKET_CONNECTION

#include "au/Token.h"
#include "au/string.h"
#include "samson/common/status.h"
#include "samson/network/Packet.h"

namespace samson {
    // Class to manage a socket connection
    
    class SocketConnection
    {
        
        int fd;               // If id is -1 it means it was closed for some reason
        std::string host;     // Name of this element for traces
        int port;             // Port in outputgoing connections ( -1 in receiving connections )
        
        au::Token token;      // Token to protect multiple closes agains the same fd
        
    public:
        
        SocketConnection( int _fd , std::string _host , int _port ) : token( "SocketConnection" )
        {
            fd = _fd;
            host = _host;
            port = _port;
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

        // Monitor information
        std::string str()
        {
            return au::str("Socket %s (fd=%d)" , str_node_name().c_str() , fd );
        }

        std::string str_node_name()
        {
            if ( port == -1 )
                return host;
            else
                return au::str( "%s:%d" , host.c_str() , port ); 
        }
        
        std::string getHost()
        {
            return host;
        }

        int getPort()
        {
            return port;
        }
        
        std::string getHostAndPort()
        {
            if( port != -1 )
                return au::str("%s:%d" , host.c_str() , port );
            else
                return au::str("%s" , host.c_str() );

        }
        
        // Connect to another element in the SAMSON network
        static Status newSocketConnection( std::string host , int port , SocketConnection** socket_connection );
        
        
        // Auxiliar function to read and write to the socket
        
        Status okToSend(int tries, int tv_sec, int tv_usec);
        Status msgAwait( int secs, int usecs, const char* what );
        
        Status partWrite( const void* dataP, int dataLen, const char* what, int retries = 300, int tv_sec = 1, int tv_usec = 0);
        Status partRead( void* vbuf, size_t bufLen, const char* what , int max_seconds , size_t * readed_size = NULL );
        
    };
    
    
    
}

#endif
