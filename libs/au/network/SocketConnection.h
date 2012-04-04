

#ifndef _H_AU_SOCKET_CONNECTION
#define _H_AU_SOCKET_CONNECTION

#include "au/Token.h"
#include "au/string.h"
#include "au/Status.h"
#include "au/network/FileDescriptor.h"

namespace au 
{
    
    class SocketConnection : public FileDescriptor
    {
        
        std::string host;     // Name of this element for traces
        int port;             // Port in outputgoing connections ( -1 in receiving connections )
        
    public:
        
        SocketConnection( int _fd , std::string _host , int _port );        
        
        // Debug information information
        std::string str();

        // Get host name
        std::string getHost();

        // Get used port
        int getPort();
        
        // Get host and port name
        std::string getHostAndPort();
        
        // Connect to another element in the SAMSON network
        static Status newSocketConnection( std::string host , int port , SocketConnection** socket_connection );
        
    };
    
}

#endif
