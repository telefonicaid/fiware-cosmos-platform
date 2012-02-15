

#include "samson/network/misc.h"

#include "NetworkManager.h"

#include "NetworkConnection.h" // Own interface


namespace samson {

   void* NetworkConnection_readerThread(void*p)
    {
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());
        
        NetworkConnection* network_connection = ( NetworkConnection* ) p;
        network_connection->readerThread();
        network_connection->running_t_read = false;
        return NULL;
    }
    
    void* NetworkConnection_writerThread(void*p)
    {
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());

        NetworkConnection* network_connection = ( NetworkConnection* ) p;
        network_connection->writerThread();
        network_connection->running_t_write = false;
        return NULL;
    }
    
    NetworkConnection::NetworkConnection( std::string _name , SocketConnection* _socket_connection , NetworkManager * _network_manager ) : token("NetworkConnection")
    {
        // Name in NetworkManager
        name = _name;
        
        // Connection we are managing
        socket_connection = _socket_connection;
        
        // Non identified
        node_identifier = NodeIdentifier( UnknownNode , 0 );
        
        // Manager to report received messages
        network_manager = _network_manager;
        
        // Init flags about threads
        running_t_read = false;
        running_t_write = false;

    }

    NetworkConnection::~NetworkConnection()
    {
        if( running_t_read )
            LM_X(1, ("Deleting Network connection with a running read thread"));
        if( running_t_write )
            LM_X(1, ("Deleting Network connection with a running write thread"));
        
        delete socket_connection;
    }
    
    void NetworkConnection::initReadWriteThreads()
    {
        
        // Create both threads for writing and reading
        if( !running_t_read )
        {
            pthread_create(&t_read, NULL, NetworkConnection_readerThread, this);
            running_t_read = true;
        }
        
        if( !running_t_write )
        {
            pthread_create(&t_write, NULL, NetworkConnection_writerThread, this);
            running_t_write = true;
        }
    }
    
    void NetworkConnection::readerThread()
    {
        while( 1 )
        {
            if( socket_connection->isDisconnected() )
            {
                // Wake up writing thread if necessary
                au::TokenTaker tt(&token);
                tt.wakeUpAll();

                return;
            }
            
            // Read a packet
            Packet * packet = new Packet();
            Status s = socket_connection->readPacket( packet );
            
            if( s == OK )
                network_manager->receive( this , packet );
                
        }
        
    }
    void NetworkConnection::writerThread()
    {
        while (1)
        {
                        
            // Quit if this connection is closed
            if( socket_connection->isDisconnected() )
                return;
            
            // Get the next packet to be sent
            Packet* packetP = packet_queue.next();
            
            if ( packetP != NULL )
            {
                Status s = socket_connection->writePacket( packetP );

                if( s == OK )
                    packet_queue.pop();
            }
            else
            {
                // Block this thread until new packets are pushed or connection is restablish...
                au::TokenTaker tt( &token );
                tt.stop(); // block
            }
            
        }
        
        LM_X(1,("Internal error")); // No possible to get this line
    }
    
    
}