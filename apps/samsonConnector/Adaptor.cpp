

#include "Connection.h"
#include "Channel.h"
#include "Adaptor.h"        // Own interface

namespace samson 
{
    namespace connector
    {
        Item::Item ( Channel * _channel , ConnectionType _type , std::string description ) 
        : token("connector::Item")
        {
            channel = _channel;
            type = _type;
            description_ = description;
            
            next_id = 0;
                        
            canceled = false; // Default value
            finished = false;
            
        }
        
        Item::~Item()
        {
            // Cancel this item to make sure we do not remove with any thread running here
            cancel_item();

            // Remover all connections
            connections.clearMap();
            
        }

        ConnectionType Item::getType()
        {
            return type;
        }
        
        std::string Item::getName()
        {
            return name_;
        }
        
        std::string Item::getFullName()
        {
            return au::str("%s.%s" ,  channel->getName().c_str() , name_.c_str() );
        }
        
        std::string Item::getDescription()
        {
            return description_;
        }
        
        const char* Item::getTypeStr()
        {
            return str_ConnectionType(type);
        }

        
        void Item::add( Connection* connection )
        {
            au::TokenTaker tt(&token);
            
            connections.insertInMap( next_id , connection );
            connection->id = next_id;

            log("Message", au::str("Connection %s (%s) added" 
                                   , connection->getFullName().c_str() 
                                   , connection->getDescription().c_str() ));
            
            // Init the connection properly
            connection->init_connecton();
            
            next_id++;
        }
        
        void Item::push( engine::Buffer * buffer )
        {
            au::TokenTaker tt(&token);

            au::map<int, Connection>::iterator it_connections;
            for( it_connections = connections.begin() 
                ; it_connections != connections.end() 
                ; it_connections++ )
            {
                Connection* connection = it_connections->second;
                if( connection->getType() == connection_output )
                    connection->push(buffer);
            }
        }
        
        void Item::review()
        {            
            if( canceled )
                return; // Not call review

            if( !finished )
                review_item();
            
            // Review all connections
            {
                au::TokenTaker tt(&token);
                au::map<int, Connection>::iterator it_connections;
                
                for( it_connections = connections.begin() 
                    ; it_connections != connections.end() 
                    ; it_connections++ )
                {
                    Connection* connection = it_connections->second;
                    connection->review();
                }

            }
            
        }
        
        void Item::init_item()
        {
            start_item();
        };
        void Item::cancel_item()
        {
            canceled = true; // This will block future calls to review
            
            stop_item();
            {
                // Mutex protectio
                au::TokenTaker tt( &token );
                
                // Cancel all connections
                au::map<int, Connection>::iterator it_connections;
                for( it_connections = connections.begin() 
                    ; it_connections != connections.end() 
                    ; it_connections++ )
                {
                    Connection* connection = it_connections->second;
                    connection->cancel_connecton();
                }
            }
        }
        int Item::getNumConnections()
        {
            au::TokenTaker tt(&token);
            return connections.size();
        }
        
        void Item::set_as_finished()
        {
            if( finished ) 
                return;

            // Log activity
            log("Message", "Set as finished");
            finished = true;
        }
        
        bool Item::is_finished()
        {
            return finished;
        }

        size_t Item::getConnectionsBufferedSize()
        {
            au::TokenTaker tt(&token);
            size_t total = 0;
            
            au::map<int, Connection>::iterator it_connections;
            for( it_connections = connections.begin() 
                ; it_connections != connections.end() 
                ; it_connections++ )
            {
                Connection* connection = it_connections->second;
                total += connection->getBufferedSize();
            }
            
            return total;
        }
        
        void Item::remove_finished_connections(au::ErrorManager* error)
        {
            // Mutex protectio
            au::TokenTaker tt( &token );
            
            // Cancel all connections
            au::map<int, Connection>::iterator it_connections;
            for( it_connections = connections.begin() 
                ; it_connections != connections.end() 
                ; it_connections++ )
            {
                Connection* connection = it_connections->second;
                if( connection->is_finished() )
                {
                    log("Message", au::str( "Removing connection %s" , connection->getFullName().c_str() ) ); 
                    
                    connection->cancel_connecton();
                    delete connection;
                    connections.erase( it_connections );
                }
            }
            
        }

        // Log system
        void Item::log( std::string type , std::string message )
        {
            log( new Log( getFullName() , type , message ) );
        }
        void Item::log( Log* log )
        {
            LogManager* log_manager = Singleton<LogManager>::shared();
            log_manager->log( log );
        }

        void Item::report_output_size( size_t size )
        {
            traffic_statistics.push_output(size);
            channel->report_output_size(size);
        }
        
        void Item::report_input_size( size_t size )
        {
            traffic_statistics.push_input(size);
            channel->report_input_size( size );
        }
        
    }
}