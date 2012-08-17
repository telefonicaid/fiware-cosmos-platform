

#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <sys/socket.h>
#include <netdb.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/ThreadManager.h"
#include "au/network/SocketConnection.h"
#include "au/network/misc.h"
#include "NetworkListener.h" // Own interface

namespace au
{
  
  NetworkListener::NetworkListener( NetworkListenerInterface * _network_listener_interface  )
  {
    network_listener_interface = _network_listener_interface;
    
    rFd = -1; // Init value
    port_ = -1;

    // Thread to indicate if we are really listening connections
    background_thread_running = false;
  }
  
  NetworkListener::~NetworkListener()
  {
    // make sure the background thread is not working
    StopNetworkListener();
  }
  
  void NetworkListener::StopNetworkListener( )
  {
    if( !background_thread_running )
      return; // Nothing to do

    // Close the open file descriptor
    ::close(rFd);
    rFd = -1;
    
    // Joint the background thread
    void* r;
    LM_LW(( "Joining background thread of listener on port %d to finish\n" , port_ ));
    pthread_join(t, &r);
  }
  
  Status NetworkListener::InitNetworkListener( int port )
  {
    // If we are running something in background, we do not accept bind to another port
    if( background_thread_running )
    {
      if( port_ != port )
        LM_W(("NetworkListener already listening on port %s. Ignoring request to open port %d" , port_ , port ));
      return au::Error;
    }
    
    // Keep port information
    port_ = port;
    
    int                 reuse = 1;
    struct sockaddr_in  sock;
    struct sockaddr_in  peer;
    
    if (rFd != -1)
    {
      LM_W(("This listener already seems to be prepared, so not init again"));
      return Error;
    }
    
    if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      LM_RP(SocketError, ("socket"));
    
    fcntl(rFd, F_SETFD, 1);
    
    memset((char*) &sock, 0, sizeof(sock));
    memset((char*) &peer, 0, sizeof(peer));
    
    sock.sin_family      = AF_INET;
    sock.sin_addr.s_addr = INADDR_ANY;
    sock.sin_port        = htons(port);
    
    setsockopt(rFd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
    
    if (bind(rFd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
    {
      ::close(rFd);
      rFd = -1;
      //LM_RP(BindError, ("bind to port %d: %s", port, strerror(errno)));
      return BindError;
    }
    
    if (listen(rFd, 10) == -1)
    {
      ::close(rFd);
      rFd = -1;
      //LM_RP(ListenError, ("listen to port %d", port));
      return ListenError;
    }
    
    
    // Create thread
    LM_T(LmtCleanup, ("Creating a thread"));
    au::ThreadManager::shared()->addNonDetachedThread( au::str("NetworkListener on port %d" , port )
                                                      ,&t
                                                      , NULL
                                                      , NetworkListener_run
                                                      , this );

    background_thread_running = true;
    
    return OK;
  }
  
  void* NetworkListener_run(void*p)
  {
    NetworkListener * network_listener = (NetworkListener*) p;
    network_listener->runNetworkListener();
    network_listener->background_thread_running = false;
    return NULL;
  }
  
  
  void NetworkListener::runNetworkListener()
  {
    
    if( rFd == -1 )
      LM_X(1, ("NetworkListener not initialized"));
    
    
    fd_set           rFds;
    int              max;
    struct timeval   tv;
    
    int              fds;
    int              eps;
    
    while ( true )
    {
      // this means that stop has been called
      if ( rFd == -1 )
        return;
      
      // One fd to read connections
      FD_ZERO(&rFds);
      eps = 1;
      max = rFd;
      FD_SET(rFd, &rFds);
      
      // Timeout
      tv.tv_sec  = 1;
      tv.tv_usec = 0;
      
      // Main select to wait new connections
      fds = select(max + 1,  &rFds, NULL, NULL, &tv);
        
      if ((fds == -1) && (errno == EINTR))
        continue;
      
      if (fds == -1)
        return; // Finish thread since this is an error

      if (fds == 0)
      {
        //timeout();
        continue;
      }

      if (!FD_ISSET(rFd, &rFds))
        LM_X(1, ("Error in main loop to accept connections"));
        
      // Accept a new connection
      SocketConnection* socket_connection = acceptNewNetworkConnection();
        
      // Notify this new connection
      network_listener_interface->newSocketConnection(this, socket_connection );
      
    }
  }
  
  SocketConnection* NetworkListener::acceptNewNetworkConnection(void)
  {
    int                 fd;
    struct sockaddr_in  sin;
    char                hostName[64];
    unsigned int        len         = sizeof(sin);
    int                 hostNameLen = sizeof(hostName);
    
    memset((char*) &sin, 0, len);
    
    LM_T(LmtNetworkListener, ("Accepting incoming connection"));
    if ((fd = ::accept(rFd, (struct sockaddr*) &sin, &len)) == -1)
      LM_RP(NULL, ("accept"));
    
    ip2string(sin.sin_addr.s_addr, hostName, hostNameLen);
    
    // Create the connection
    SocketConnection* socket_connection = new SocketConnection( fd , hostName , -1 );
    return socket_connection;
  }
  
  // Check running status
  bool NetworkListener::IsNetworkListenerRunning()
  {
    return background_thread_running;
  }
  
  std::string NetworkListener::getStatus()
  {
    if( background_thread_running )
      return "listening";
    else
      return "not listening";
    
  }
  
  int NetworkListener::port()
  {
    return port_;
  }


}
