

#ifndef _H_SAMSON_CONNECTOR_COMMON
#define _H_SAMSON_CONNECTOR_COMMON

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/BufferContainer.h"


// ----------------------------------------------------------------
#define SC_CONSOLE_PORT 5467
#define SC_WEB_PORT     9999
#define SAMSON_CONNECTOR_INTERCHANNEL_PORT 9890

// ----------------------------------------------------------------

namespace samson 
{
    namespace connector
    {
        // ConnectionType
        // ------------------------------------------
        
        typedef enum
        {
            connection_input,
            connection_output
        } ConnectionType;
        
        const char * str_ConnectionType( ConnectionType type );

        // BufferList
        // ------------------------------------------
        
        // Buffer of blocks with a maximum size
        class BufferList : private engine::BufferListContainer
        {
            au::Token token;
            size_t max_size;
            
        public:
            
            BufferList() : token("BufferList")
            {
                max_size = 0;
            }
            
            // Set maximum size
            void setMaxSize( size_t _max_size )
            {
                au::TokenTaker tt(&token);
                max_size = _max_size;
                
                // Remove packets if too much size accumulated here
                if( max_size > 0 )
                    while( getTotalSize() > max_size )
                        engine::BufferListContainer::pop();
            }

            // Insert a buffer in the system
            void push( engine::Buffer * buffer )
            {
                au::TokenTaker tt(&token);
                push_back( buffer );

                // Remove packets if too much size accumulated here
                if( max_size > 0 )
                    while( getTotalSize() > max_size )
                        engine::BufferListContainer::pop();
                
            }
            
            // Pop the next buffer to be sent
            void pop( engine::BufferContainer * container )
            {
                au::TokenTaker tt(&token);
                engine::Buffer* buffer = engine::BufferListContainer::front();
                container->setBuffer(buffer);
                engine::BufferListContainer::pop();
            }
            
            size_t getSize()
            {
                au::TokenTaker tt(&token);
                return getTotalSize();
            }
            
        };
        
        
    }
}

#endif