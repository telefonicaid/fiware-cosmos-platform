

#include "Connection.h"
#include "Channel.h"
#include "Item.h"        // Own interface

namespace samson 
{
    namespace connector
    {
        Item::Item ( Channel * _channel , ConnectionType _type , std::string _name , std::string short_name ) 
        : token("connector::Item")
        {
            channel = _channel;
            type = _type;
            name = _name;
            short_name_ = short_name;
            
            next_id = 0;
            
            removing = false;
        }
        
        void Item::add( Connection* connection )
        {
            au::TokenTaker tt(&token);
            
            if( removing )
            {
                // this should never happen...
                return; 
            }
            
            connections.insertInMap( next_id , connection );
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
            // Review item itself
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

                    // Review connection
                    connection->review();

                    // Remove if necessary
                    if ( connection->canBeRemoved() )
                        connections.erase( it_connections );
                }

            }
            
        }
        
        int Item::getNumConnections()
        {
            au::TokenTaker tt(&token);
            return connections.size();
        }
        
        bool Item::isRemoving()
        {
            return removing;
        }

        // Set removing
        void Item::set_removing()
        {
            au::TokenTaker tt(&token);
            
            if( removing )
                return; // Already in removing mode...

            removing = true;
            
            // Set current connection to removing...
            au::map<int, Connection>::iterator it_connections;
            
            for( it_connections = connections.begin() 
                ; it_connections != connections.end() 
                ; it_connections++ )
            {
                Connection* connection = it_connections->second;
                connection->set_removing();
            }
        }


        
    }
}