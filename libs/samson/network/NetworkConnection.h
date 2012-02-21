

#ifndef _H_SAMSON_NETWORK_CONNECTION
#define _H_SAMSON_NETWORK_CONNECTION

#include <string>

#include "au/Token.h"
#include "au/TokenTaker.h"
#include "au/Rate.h"

#include "samson/common/status.h"
#include "samson/common/Visualitzation.h"

#include "samson/network/SocketConnection.h"
#include "samson/network/Packet.h"

namespace samson {
    
    
    class NetworkManager;
    
    class PacketQueue
    {
        au::Token            token;
        au::list<Packet>     queue;
        
        
    public:
        
        PacketQueue() : token("PacketQueue") 
        {
        }
        
        size_t getNumPackets()
        {
            au::TokenTaker tt(&token);
            return queue.size();
        }
        
        void push( Packet* p)
        {
            au::TokenTaker tt(&token);
            queue.push_back(p);
        }

        Packet* next()
        {
            au::TokenTaker tt(&token);
            Packet* packet = queue.findFront();
            return packet;
        }
        
        void pop()
        {
            au::TokenTaker tt(&token);
            Packet* packet = queue.extractFront();

            if( !packet )
                LM_W(("pop without packet called at PacketQueue"));
        }
        
        std::string str()
        {
            au::TokenTaker tt(&token);
            return au::str("PaquetQueue: %lu packets" , queue.size() );
        }

        
        void clear()
        {
            // Clear queues, removing packages and alloc buffers
            au::TokenTaker tt(&token);
            au::list<Packet>::iterator it_queue;
            for ( it_queue = queue.begin() ; it_queue != queue.end() ; it_queue++ )
            {
                engine::Buffer * buffer = ( *it_queue )->buffer;
                if ( buffer )
                    engine::MemoryManager::shared()->destroyBuffer( buffer );
            }
            
            // Remove elements calling delete to all of them
            queue.clearList();
        }
        
        
    };    
    
    class NetworkConnection
    {
        // Identifier of the node ( if available )
        NodeIdentifier node_identifier;
        
        // Socket Connection
        SocketConnection* socket_connection;
        
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

        friend class NetworkManager;
        std::string name;         //Name in NetworkManager

        // Information about rate
        au::rate::Rate rate_in;
        au::rate::Rate rate_out;
        
    public:

        // Constructor & Destructor
        NetworkConnection( std::string _name , SocketConnection* socket_connection , NetworkManager * _network_manager );
        ~NetworkConnection();

        // Init io threads
        void initReadWriteThreads();
        
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

        NodeIdentifier getNodeIdentifier()
        {
            return node_identifier;
        }
        
        std::string str_node_name()
        {
            return socket_connection->str_node_name();
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
