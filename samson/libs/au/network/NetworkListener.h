

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
  

  void* NetworkListener_run(void*p);
  
  class NetworkListener
  {
    
  public:
    
    // Constructor
    NetworkListener( NetworkListenerInterface * _network_listener_interface );
    ~NetworkListener();
    
    // Init and close functions
    Status InitNetworkListener( int port );
    
    // Function to cancel banground thread accepting connections
    void StopNetworkListener( );
    
    // Get port
    int port();

    // Debug information
    bool IsNetworkListenerRunning();
    std::string getStatus();
    
  private:
    
    // Main function to run in background
    void runNetworkListener();

    SocketConnection* acceptNewNetworkConnection(void);
    
    // Network manager to be notified
    NetworkListenerInterface * network_listener_interface;
    
    // Port where we are listening
    int port_;
    
    // Internal file descriptor
    int rFd;
    
    // Background thread
    bool background_thread_running;     // Flag indicating if the background thread is running ( to joint at destructor )
    pthread_t t;

    // Background thread function
    friend void* NetworkListener_run(void*p);

    
  };
  
}

#endif
