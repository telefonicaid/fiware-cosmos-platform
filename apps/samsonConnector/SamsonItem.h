#ifndef _H_SAMSON_CONNECTOR_SAMSON_CONNECTION
#define _H_SAMSON_CONNECTOR_SAMSON_CONNECTION

#include "au/mutex/Token.h"

#include "Item.h"
#include "Connection.h"
#include "common.h"



namespace samson {
    namespace connector {
        
        class Block;
        
        class SamsonConnection : public Connection , public DelilahLiveDataReceiverInterface
        {
            
            SamsonClient * client;
            std::string queue;
            
        public:
            
            
            SamsonConnection( Item  * _item , ConnectionType _type , std::string name , SamsonClient * _client , std::string _queue )
            : Connection( _item , _type , name  )
            {
                // Keep a pointer to the client
                client = _client;
                
                // Keep the name of the queue
                queue = _queue;
                
                // Set me as the receiver of live data from SAMSON
                client->set_receiver_interface(this);
            }
            
            ~SamsonConnection()
            {
                delete client;
            }
            
            virtual void start_connection()
            {
                // ?
            }
            
            virtual void stop_connection()
            {
                // TODO: Stop all theads since it will be removed
            }
            
            virtual void review_connection()
            {
                // Nothing to do here
            }
            
            size_t getSize()
            {
                if( getType() == connection_output )
                    return client->getNumPendingOperations(); // It is not the size in bytes but at least is >0 if not all data is emitted
                else
                    return 0;
            }

            
            // Overload method to push blocks using samsonClient
            void push( engine::Buffer* buffer )
            {
                if( getType() == connection_input )
                    return; // Nothing to do if we are input

                // Report manually size ( not we are overloading Connection class )
                report_output_size( buffer->getSize() );
                
                // Push this block directly to the SAMSON client
                //client->push( queue , new BlockDataSource( buffer ) );
                client->push( queue , buffer );
            }
            
            // Overwriteen method of SamsonClient
            void receive_buffer_from_queue(std::string queue , engine::Buffer* buffer)
            {
                // Transformation of buffer
                KVHeader *header = (KVHeader*) buffer->getData();
                
                if( header->isTxt() )
                {
                    // Push the new buffer
                    pushInputBuffer( buffer );
                    
                }
                else
                {
                    LM_W(("Received a binary buffer %s from %s. Still not implemented how to process this" 
                          , au::str( buffer->getSize() , "B" ).c_str() , getFullName().c_str() ));
                }
            }
            
            std::string getStatus()
            {
                return client->getStatisticsString();
            }
            
        };
        
        class SamsonItem : public Item
        {
            // Information to stablish the connection with the SAMSON system
            std::string host;
            int port;
            std::string queue;
            
            // Last connection trial
            au::Cronometer cronometer;
            
        public:
            
            SamsonItem( Channel * _channel , ConnectionType _type , 
                       std::string _host 
                       , int _port 
                       , std::string _queue );
                        

            // Get status of this element
            std::string getStatus()
            {
                if( getNumConnections() == 0 )
                    return au::str( "Non connected... ( last trial %s )" , cronometer.str().c_str() );
                else
                    return "Connection created";
            }
            
            void review_item()
            {
                
                if( getNumConnections() > 0 )
                    return; // Already connected with this SAMSON
 
                // Reset the cornometer
                cronometer.reset();
                
                // Try to connect to this SAMSON
                SamsonClient* client = new SamsonClient("connector");
                au::ErrorManager error;
                client->initConnection( &error , host , port );
                
                if( error.isActivated() )
                {
                    // Not possible to establish connection
                    delete client;
                    return;
                }
                
                // At the moment, it is not possible to specify flags new of clear here
                if( getType() == connection_input )
                    client->connect_to_queue(queue, false, false); 
                
                // Add the connection 
                std::string name = au::str("SAMSON at %s:%d (%s)" , host.c_str() , port , queue.c_str() );
                
                // Add connection
                add( new SamsonConnection( this , getType() , name , client , queue ) );
                
            }
            
            
        };
        
                

        
    }
}

#endif

