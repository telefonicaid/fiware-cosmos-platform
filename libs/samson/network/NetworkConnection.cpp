
#include "au/ThreadManager.h"
#include "au/network/misc.h"

#include "samson/common/MessagesOperations.h"
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
        network_connection->quitting_t_reader = false;
        return NULL;
    }
    
    void* NetworkConnection_writerThread(void*p)
    {
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());

        NetworkConnection* network_connection = ( NetworkConnection* ) p;
        network_connection->writerThread();
        network_connection->running_t_write = false;
        network_connection->quitting_t_writer = false;
        return NULL;
    }
    
    NetworkConnection::NetworkConnection( std::string _name 
                                         , au::SocketConnection* _socket_connection 
                                         , NetworkManager * _network_manager ) : token("NetworkConnection")
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
        // and stopping
        quitting_t_reader = false;
        quitting_t_writer = false;

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
            std::string name = au::str("NetworkConnection::initReadWriteThreads::read (%s)"
                                       , socket_connection->getHostAndPort().c_str() );
            
            running_t_read = true;
            au::ThreadManager::shared()->addThread(name, &t_read, NULL, NetworkConnection_readerThread, this);
        }
        
        if( !running_t_write )
        {
            std::string name = au::str("NetworkConnection::initReadWriteThreads::write (%s)"
                                       , socket_connection->getHostAndPort().c_str() );
            
            running_t_write = true;
            au::ThreadManager::shared()->addThread(name, &t_write, NULL, NetworkConnection_writerThread, this);
        }
    }
    
    void NetworkConnection::stopReadWriteThreads()
    {

        // Set the quit flag for stopping threads
        if( running_t_read )
        {
            quitting_t_reader = true;
        }

        if( running_t_write )
         {
            quitting_t_writer = true;
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

            if (quitting_t_reader == true)
            {
                LM_W(("Quit readerThread because quitting_t_reader == true"));
                au::TokenTaker tt(&token);
                 tt.wakeUpAll();
                 return;
            }

            // Read a packet
            Packet * packet = new Packet();

            size_t total_read = 0;
            
            // Read packet
            au::Status s = packet->read( socket_connection , &total_read );

            // Monitor rate
            rate_in.push(total_read);
            
            if( s == au::OK )
                network_manager->receive( this , packet );
            else
                socket_connection->close(); // Close connection since a packet has not been received correctly
            
            // Release the created packet
            packet->release();            
        }
        
    }
    void NetworkConnection::writerThread()
    {
        while (1)
        {
                        
            // Quit if this connection is closed
            if( socket_connection->isDisconnected() )
            {
                // Save pending packets...
                network_manager->push_pending_packet(name, &packet_queue);
                
                return;
            }
            
            if (quitting_t_writer == true)
            {
                LM_W(("Quit writerThread because quitting_t_writer == true"));
                // Save pending packets...
                network_manager->push_pending_packet(name, &packet_queue);

                return;
            }

            // Get the next packet to be sent
            Packet* packet = packet_queue.next();
            
            if ( packet != NULL )
            {
                size_t total_write = 0;

                // Write the packet over this socket
                au::Status s = packet->write( socket_connection , &total_write );
                
                // Monitor rate
                rate_out.push(total_write);

                if( s == au::OK )
                    packet_queue.pop(); // If not ok, packet is still in the queue
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
    
    void NetworkConnection::fill( network::CollectionRecord * record, Visualization* visualization )
    {
        if (visualization == NULL)
            LM_D(("visualization == NULL"));

        ::samson::add( record , "name"    , name , "different,left" );
        ::samson::add( record , "user"    , user , "different,left" );
        ::samson::add( record , "connection"    , connection_type , "different,left" );

        if( socket_connection->isDisconnected() )
            ::samson::add( record , "status" , "disconnected" , "different,left" );
        else
            ::samson::add( record , "status" , "connected" , "different,left" );
        
        ::samson::add( record , "host"    , socket_connection->getHostAndPort() , "different" );
        
        ::samson::add( record , "In (B)"    , rate_in.getTotalSize() , "f=uint64,sum" );
        ::samson::add( record , "Out (B)"    , rate_out.getTotalSize() , "f=uint64,sum" );

        ::samson::add( record , "In (B/s)"   , (size_t)rate_in.getRate() , "f=uint64,sum" );
        ::samson::add( record , "Out (B/s)"   , (size_t)rate_out.getRate() , "f=uint64,sum" );
        
    }

    
    
}
