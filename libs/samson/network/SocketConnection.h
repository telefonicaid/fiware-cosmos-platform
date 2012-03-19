

#ifndef _H_SAMSON_SOCKET_CONNECTION
#define _H_SAMSON_SOCKET_CONNECTION

#include "au/Token.h"
#include "au/string.h"
#include "samson/common/status.h"
#include "samson/network/Packet.h"
#include "samson/network/FileDescriptor.h"

namespace samson {
    // Class to manage a socket connection
    
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
