

#ifndef _H_SAMSON_CONNECTOR_BUFFER_LIST
#define _H_SAMSON_CONNECTOR_BUFFER_LIST

#include "au/tables/Table.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/BufferContainer.h"
#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"


namespace samson 
{
    namespace connector
    {
        
        // BufferList
        // ------------------------------------------
        
        
        class BufferListItem : public engine::Object
        {
            engine::BufferContainer buffer_container_;    // Buffer if it is on memory
            std::string file_name_;                       // Full name of the file if it is necessary to persist on disk

            size_t buffer_size;
            std::string buffer_name;
            std::string buffer_type;

            au::Token token;
            
            typedef enum
            {
                on_memory,
                writing,
                on_memory_and_disk,
                reading,
                on_disk,
            } State;
            
            State state;
            
        public:
            
            BufferListItem( engine::Buffer * buffer , const std::string& file_name ) : token("BufferListItem")
            {
                buffer_container_.setBuffer(buffer);
                file_name_ = file_name;
                
                buffer_size = buffer->getSize();
                buffer_name = buffer->getName();
                buffer_type = buffer->getType();
                
                // Initial state
                state = on_memory;
                
                // Be able to receive notification from disk manager
                //listen( notification_disk_operation_request_response );
            }
            
            size_t getSize()
            {
                return buffer_size;
            }
            
            size_t getSizeOnMemory()
            {
                if( is_on_memory() ) 
                    return buffer_size;
                else
                    return 0;
            }
            
            bool is_on_memory()
            {
                switch (state) 
                {
                    case on_memory:
                    case on_memory_and_disk:
                    case writing:
                        return true;
                        
                    default:
                        return false;
                }
                return buffer_size;

            }
            
            
            // Return buffer if it is on memory
            engine::Buffer* getBuffer()
            {
                au::TokenTaker tt(&token);
                return buffer_container_.getBuffer();
            }
            
            // Method to receive a particular notification
            virtual void notify( engine::Notification* notification )
            {
                au::TokenTaker tt(&token);
                
                if( notification->isName( notification_disk_operation_request_response ) )
                {
                    
                    std::string type      = notification->environment.get("type", "-");
                    
                    if(  type == "write" )
                    {
                        if( state != writing )
                        {
                            LM_W(("Unexpected state in BufferListItem"  ));
                            return;
                        }
                        state = on_memory_and_disk;
                        return;
                    }
                    else if( type == "read" )
                    {
                        if( state != reading )
                        {
                            LM_W(("Unexpected state in BufferListItem"  ));
                            return;
                        }
                        state = on_memory_and_disk;
                        return;
                    }
                    else if( type == "remove" )
                    {
                        // Nothing to do here...
                    }
                    
                    return;
                }
                
                LM_W(("Unknown notification received in BufferListItem"));
            }
            
            void flush_to_disk()
            {
                au::TokenTaker tt(&token);

                switch (state) 
                {
                    case on_memory_and_disk:
                    {
                        // Direct transition to on_disk
                        state = on_disk;
                        buffer_container_.setBuffer(NULL);
                        break;
                    }
                     
                    case on_memory:
                    {
                        
                        // squedule_write
                        engine::Buffer* buffer = buffer_container_.getBuffer(); 
                        if( !buffer )
                            LM_X(1,("Internal error"));
                        
                        engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation(buffer, file_name_, getEngineId() );
                        engine::DiskManager::shared()->add( operation );        
                        
                        state = writing;
                        return;
                    }
                        
                    default:
                        break;
                }
            }
            
            void load_from_disk()
            {
                au::TokenTaker tt(&token);
                
                switch (state) 
                {
                    case on_disk:
                    {
                        // Squedule reading
                        // Create the buffer
                        buffer_container_.create(buffer_name, buffer_type, buffer_size );
                        engine::Buffer* buffer = buffer_container_.getBuffer();
                        
                        engine::DiskOperation::newReadOperation(buffer->getData(), file_name_, 0, buffer_size, getEngineId() );

                        state = reading;
                        return;
                    }
                        
                    default:
                        break;
                }
            }

            
            
            
        };
        
        
        // Buffer of blocks with a maximum size
        class BufferList
        {
            au::Token token;
            
            // Persistancy
            std::string persistence_directory_; // Place to store excedent messages
            size_t max_size_on_memory_;         // Max size to keep on memory

            size_t file_id_;                         // Interal counter to give names
            
            // List of objects
            au::list<BufferListItem> items; // List of items containing blocks
            
        public:
            
            BufferList( std::string persistence_directory , size_t max_size_on_memory ) : token("BufferList")
            {
                persistence_directory_ = persistence_directory;
                max_size_on_memory_ = max_size_on_memory;
                
                file_id_ = 1;
            }
            
            void review_persistence()
            {
                // Schedule read or write tasks
                au::list<BufferListItem>::iterator it;
                size_t total = 0;
                for ( it = items.begin() ; it != items.end() ; it++ )
                {
                    BufferListItem* item = *it;
                    
                    if( total < max_size_on_memory_ )
                        item->load_from_disk();
                    else
                    {
                        // Flushing to disk
                        item->flush_to_disk();
                    }
                    
                    total += item->getSize();
                }
            }
            
            // Insert a buffer in the system
            void push( engine::Buffer * buffer )
            {
                au::TokenTaker tt(&token);
                std::string name = au::str("%s/buffer_%lu",persistence_directory_.c_str() , file_id_++ );  
                items.push_back( new BufferListItem( buffer, name ) );
            }
            
            // Pop the next buffer to be sent
            void pop( engine::BufferContainer * container )
            {
                au::TokenTaker tt(&token);
                
                if( items.size() == 0 )
                    return;
                
                BufferListItem* item = items.front();
                
                engine::Buffer* buffer = item->getBuffer();
                
                if( buffer )
                {
                    // If it is memory, return
                    container->setBuffer(buffer);
                    items.pop_front();
                    delete item;
                }
                
            }
            
            size_t getSize()
            {
                au::TokenTaker tt(&token);
                size_t total = 0;
                au::list<BufferListItem>::iterator it;
                for ( it = items.begin() ; it != items.end() ; it++ )
                {
                    BufferListItem* item = *it;
                    total += item->getSize();
                }
                return total;
            }
            
            size_t getSizeOnMemory()
            {
                au::TokenTaker tt(&token);
                size_t total = 0;
                au::list<BufferListItem>::iterator it;
                for ( it = items.begin() ; it != items.end() ; it++ )
                {
                    BufferListItem* item = *it;
                    total += item->getSizeOnMemory();
                }
                return total;
            }

            
        };
    }
}

#endif
