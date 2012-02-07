

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
        Status readPacket( Packet * packetP );
        Status writePacket( Packet * packetP );
        
        // Read line 
        Status readLine( char* line, size_t max_size , int max_seconds );
        
        // Write line
        Status writeLine( const char* line );
        
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
        
        
        // Connect to another element in the SAMSON network
        static Status newSocketConnection( std::string host , int port , SocketConnection** socket_connection );
        
        
    private:
        
        // Auxiliar function to read and write to the socket
        
        Status okToSend( );
        Status msgAwait( int secs, int usecs, const char* what );
        
        Status partWrite( const void* dataP, int dataLen, const char* what );
        Status partRead( void* vbuf, long bufLen, const char* what , int max_seconds );
        
    };
    
    
    
}

#endif