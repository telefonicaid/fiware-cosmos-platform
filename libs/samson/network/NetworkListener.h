


#ifndef _H_SAMSON_NETWORK_LISTENER
#define _H_SAMSON_NETWORK_LISTENER


#include "samson/common/status.h"

namespace samson {
    
    class SocketConnection;
    
    class NetworkListener
    {
        // Network manager to be notified
        NetworkManager * network_manager;
        
        // Port where we are listening
        int port;
        
        int rFd;
        bool quit_flag;
        
        // Thread listening connections...
        pthread_t t;
        
    public:

        // Constructor
        NetworkListener( NetworkManager * network_manager );
        
        
        // Init and close functions
        Status initNetworkListener( int port );        

        // Function to cancel banground thread accepting connections
        void quit();
        
        // Non blocking and blocking calls to accept connections
        void runNetworkListenerInBackground();
        void runNetworkListener();
        
    private:
        
        SocketConnection* acceptNewNetworkConnection(void);

        
    };
    
}

#endif
