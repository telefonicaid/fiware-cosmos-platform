

#ifndef _H_AU_NETWORK_LISTENER
#define _H_AU_NETWORK_LISTENER

#include "au/Status.h"

namespace au 
{
    
    class SocketConnection;
    class NetworkListener;
    
    class NetworkListenerInterface
    {
        public:
        
        virtual void newSocketConnection( NetworkListener* listener , SocketConnection * socket_connetion )=0;
    };
    
    
    class NetworkListener
    {
        // Network manager to be notified
        NetworkListenerInterface * network_listener_interface;
        
        // Port where we are listening
        int port;
        
        // Internal file descriptor
        int rFd;
        
        // Flag to inform that background thread should finish
        bool quit_flag;
        
        // Thread listening connections...
        pthread_t t;
        
    public: 
        
        // Flag indicating if the background thread is running
        bool background_thread_running;
        
    public:

        // Constructor
        NetworkListener( NetworkListenerInterface * _network_listener_interface );
        ~NetworkListener();

        // Get port
        int getPort()
        {
            return port;
        }
        
        // Init and close functions
        Status initNetworkListener( int port );        

        // Function to cancel banground thread accepting connections
        void stop( bool wait );
        
        // Non blocking and blocking calls to accept connections
        void runNetworkListenerInBackground();
        void runNetworkListener();
        
        
        // Check running status
        bool isNetworkListenerRunning()
        {
            return background_thread_running;
        }
        
        std::string getStatus()
        {
            if( background_thread_running )
                return "listening";
            else
                return "not listening";
                
        }
        
    private:
        
        SocketConnection* acceptNewNetworkConnection(void);

        
    };
    
}

#endif
