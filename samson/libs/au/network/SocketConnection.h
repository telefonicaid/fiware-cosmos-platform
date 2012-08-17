

#ifndef _H_AU_SOCKET_CONNECTION
#define _H_AU_SOCKET_CONNECTION

#include "au/mutex/Token.h"
#include "au/string.h"
#include "au/Status.h"
#include "au/Cronometer.h"
#include "au/network/FileDescriptor.h"

namespace au 
{
    
    class SocketConnection : public FileDescriptor
    {
        // Andreu note: Use only LM_LM or LM_LW in this class since it is used in hoock function
        
        au::Cronometer cronometer; // Creation time cronometer
        std::string host;          // Name of this element for traces
        int port;                  // Port in outputgoing connections ( -1 in receiving connections )
        
        
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

        // Get connection time;
        size_t getTime()
        {
            return cronometer.seconds();
        }
        
        // Duplicate this socket connection invalidating this instance
        // It is a mechanism to extrat this connection and avoid this one to close the socket
        SocketConnection* duplicateAndInvalidate()
        {
            int fd = getFdAndinvalidate();
            if( fd == -1 )
                return NULL;
            
            return new SocketConnection( fd , host , port );
        }
        
    };
    
}

#endif
