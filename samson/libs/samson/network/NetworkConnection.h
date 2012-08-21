

#ifndef _H_SAMSON_NETWORK_CONNECTION
#define _H_SAMSON_NETWORK_CONNECTION

#include <string>

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/Rate.h"

#include "au/network/SocketConnection.h"

#include "samson/common/status.h"
#include "samson/common/Visualitzation.h"
#include "samson/network/PacketQueue.h"
#include "samson/network/Packet.h"

namespace samson {
  
  class NetworkManager;
  
  class NetworkConnection
  {
    
  public:
    
    // Constructor & Destructor
    NetworkConnection( NodeIdentifier node_identifier 
                      , au::SocketConnection* socket_connection 
                      , NetworkManager * network_manager );
    
    ~NetworkConnection();
    
    // Close connection ( if still open ) and wait until threads are gone
    void CloseAndStopBackgroundThreads();

    // Wake up the writer thrad
    void WakeUpWriter();

    // Close socket ( no waiting for backgroud threads )
    void Close();

    // Check if the socket is closed
    bool isDisconnectd();
    
    // Debug string
    std::string str();
    
    // Get some information
    std::string getName();
    std::string getHost();
    int getPort();
    size_t get_rate_in();
    size_t get_rate_out();
    NodeIdentifier node_identifier();
    std::string host_and_port();


    // Function to generate lists of items in delilah console
    void fill( gpb::CollectionRecord * record, const Visualization& visualization);
    
  private:
    
    // Friend functions to run both read and write threads
    friend void* NetworkConnection_writerThread(void*p);
    friend void* NetworkConnection_readerThread(void*p);

    // Read & Write threads main functions
    void readerThread();
    void writerThread();
    
    // Identifier of the node ( if available )
    NodeIdentifier node_identifier_;
    
		// User and password for this connection
		std::string user;
		std::string password;
		std::string connection_type;
    
    // Socket Connection
    au::SocketConnection* socket_connection_;
    
    // Token to block write thread when more packets have to be sent
    au::Token token_;
    
    // Pointer to the manager to report received messages 
    // and extrace message to send
    NetworkManager * network_manager_;
    
    // Threads for reading and writing packets to this socket
    pthread_t t_read , t_write;
    bool running_t_read;      // Flag to indicate that there is a thread using this endpoint writing data
    bool running_t_write;     // Flag to indicate that there is a thread using this endpoint reading data
    
    friend class NetworkManager;
    friend class CommonNetwork;
    
    // Information about rate
    au::rate::Rate rate_in;
    au::rate::Rate rate_out;
    
  };
  
}

#endif
