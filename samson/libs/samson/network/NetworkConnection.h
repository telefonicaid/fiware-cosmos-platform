

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
        // Identifier of the node ( if available )
        NodeIdentifier node_identifier;

		// User and password for this connection
		std::string user;
		std::string password;
		std::string connection_type;
        
        // Socket Connection
        au::SocketConnection* socket_connection;
        
        // Token to block write thread when more packets have to be sent
        au::Token token;
        
        // Pointer to the manager to report received messages
        NetworkManager * network_manager;

        // Queue with pending packets for this element
        PacketQueue packet_queue;

        // Threads for reading and writing packets to this socket
        pthread_t t_read , t_write;
        bool running_t_read;      // Flag to indicate that there is a thread using this endpoint writing data
        bool running_t_write;     // Flag to indicate that there is a thread using this endpoint reading data

        bool quitting_t_reader;    // Flag to signal the reader thread to stop itself
        bool quitting_t_writer;    // Flag to signal the writer thread to stop itself

        friend class NetworkManager;
        friend class CommonNetwork;
        std::string name;         //Name in NetworkManager

        // Information about rate
        au::rate::Rate rate_in;
        au::rate::Rate rate_out;
        
    public:

        // Constructor & Destructor
        NetworkConnection( std::string _name , au::SocketConnection* socket_connection , NetworkManager * _network_manager );
        ~NetworkConnection();

        // Init io threads
        void initReadWriteThreads();
        
        // to stop the reader and writer threads
        void stopReadWriteThreads();

        // Push a packet to this node
        void push( Packet* p )
        {
            // Protect agains NULL Packets...
            if ( !p )
                return;
            
            // Push to the queue
            packet_queue.push(p);
            
            // Wake up writing thread if necessary
            au::TokenTaker tt(&token);
            tt.wakeUpAll();
        }
        
        void close()
        {
            // Set the flag to make sure all threads finish correctly
            socket_connection->close();
            
            // Wake up writing thread if necessary
            au::TokenTaker tt(&token);
            tt.wakeUpAll();
        }
        
        bool isDisconnected()
        {
            return socket_connection->isDisconnected();
        }
        
        bool noThreadsRunning()
        {
            if ( running_t_write )
                return false;
            if ( running_t_read )
                return false;
            
            return true;
        }
        
        // Read & Write threads operations
        void readerThread();
        void writerThread();

        friend void* NetworkConnection_writerThread(void*p);
        friend void* NetworkConnection_readerThread(void*p);
        
        
        std::string getName()
        {
            return name;
        }
        
        std::string getHost()
        {
            return socket_connection->getHost();
        }
        
        int getPort()
        {
            return socket_connection->getPort();
        }
        
        void setNodeIdentifier( NodeIdentifier _node_identifier )
        {
            node_identifier = _node_identifier;
        }

        void setUserAndPassword( std::string _user , std::string _password )
        {
            user = _user;
            password = _password;
        }
        
        void setConnectionType( std::string _connection_type )
        {
            connection_type = _connection_type;
        }
        
        NodeIdentifier getNodeIdentifier()
        {
            return node_identifier;
        }
        
        std::string getHostAndPort()
        {
            return socket_connection->getHostAndPort();
        }
        
        std::string str()
        {
            std::ostringstream output;
            
            output << "[" << (running_t_read?"R":" ") << (running_t_write?"W":" ") << "]";
            
            if ( socket_connection->isDisconnected() )
                output << " Disconnected ";
            else
                output << " Connected    ";
                
            output << "[ " << node_identifier.str() << " ] ";
            return output.str();
        }
        
        void fill( network::CollectionRecord * record, Visualization* visualization);
      
        size_t get_rate_in()
        {
            return rate_in.getRate();
        }

        size_t get_rate_out()
        {
            return rate_out.getRate();
        }
        
        
    };
    
}

#endif
