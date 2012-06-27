

#include "SamsonConnector.h"
#include "Channel.h"
#include "FileDescriptorConnection.h"
#include "ServerConnection.h" // Own interface

namespace samson {
    namespace connector
    {
        
        // ---------------------------------
        // ConnectionItem
        // ---------------------------------
        
        ConnectionItem::ConnectionItem( Channel* _channel , ConnectionType _type , std::string _host , int _port  )
        : Item( _channel 
               , _type
               , au::str("CONNECTION(%s:%d)" , _host.c_str() , _port) 
               ) 
        {
            host = _host;
            port = _port;

            // Init cronometer and trials counter
            connection_cronometer.reset();
            connection_trials = 0;
            
            
        }
        
        
        void ConnectionItem::try_connect()
        {
            au::SocketConnection* socket_connection;
            au::Status s = au::SocketConnection::newSocketConnection( host
                                                                     , port
                                                                     , &socket_connection);                                  
            if( s == au::OK )
            {
                std::string name = au::str( "socket %s" ,socket_connection->getHostAndPort().c_str() );
                add( new FileDescriptorConnection( this , getType() , name , socket_connection ) );
                
                connection_cronometer.reset();
                connection_trials = 0;
            }
            else
                connection_trials++;
            
        }
        
        void ConnectionItem::start_item()
        {
            // Review item to establish connection
            try_connect();
        }
        
        void ConnectionItem::review_item()
        {
            
            if( getNumConnections() > 0 )
                return; // Connection is established
            if( connection_cronometer.diffTimeInSeconds() < 3 )
                return; // No retray
            try_connect();
        }

        std::string ConnectionItem::getStatus()
        {
            if( getNumConnections() > 0 )
                return "connected";
            return au::str("connecting... [ %d trials ] )" , connection_trials );
        }
        
        void ConnectionItem::stop_item()
        {
            // Nothing to do here
        }
        
        // ---------------------------------
        // StdinItem
        // ---------------------------------

        
        // Constructor & Destructor
        StdinItem::StdinItem( Channel* _channel ) : Item( _channel , connection_input , "STDIN" ) 
        {
            
        }
        
        // Information about status
        std::string StdinItem::getStatus()
        {
            if( getNumConnections() > 0 )
                return "connected";
            return "closed";
        }
        
        void StdinItem::review_item()
        {
            // Nothing to do here
        }
        void StdinItem::start_item()
        {
            // Add connection
            add( new FileDescriptorConnection( this 
                                              , connection_input 
                                              , "stdin" 
                                              , new au::FileDescriptor( "stdin" , 0 ) ) );
            
            // No more connections will be stablished, so consider it finished
            set_as_finished();
        }
        
        void StdinItem::stop_item()
        {
            // Nothing to do here
        }
        
        
        // ---------------------------------
        // StdoutItem
        // ---------------------------------
        
        // Constructor & Destructor
        StdoutItem::StdoutItem( Channel* _channel ) : Item( _channel , connection_output , "STDOUT" ) 
        {
        }
        
        // Information about status
        std::string StdoutItem::getStatus()
        {
            if( getNumConnections() > 0 )
                return "connected";
            return "closed";
        }
        
        void StdoutItem::start_item()
        {
            // Add connection
            add( new FileDescriptorConnection( this , connection_output , "stdout" , new au::FileDescriptor( "stdout" , 1 ) ) );
        }
        
        void StdoutItem::review_item()
        {
            // Nothing to do here
        }
        
        void StdoutItem::stop_item()
        {
            // nothing to do here
        }

        
        
        
    }
    
    
}