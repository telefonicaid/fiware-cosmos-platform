/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "engine/BufferContainer.h"

#ifndef _H_Buffercontainer_SamsonClient
#define _H_Buffercontainer_SamsonClient


namespace  samson {
    
    class BufferContainer 
    {
        au::Token token;
        au::map< std::string , engine::BufferListContainer > buffer_lists;    // Buffers for live data
        
    public:
        
        BufferContainer( ) : token("BufferContainer")
        {
            // listen...
        }
        
        void push( std::string queue , engine::Buffer* buffer )
        {
            au::TokenTaker tt(&token);
            
            if( buffer->getSize() == 0 )
                return;
            
            getBufferList( queue )->push_back( buffer );
        }
        
        void pop( std::string queue , engine::BufferContainer* buffer_container )
        {
            au::TokenTaker tt(&token);
         
            engine::BufferListContainer* list = getBufferList(queue);
            
            engine::Buffer* buffer = list->front();
            
            if( buffer )
            {
                buffer_container->setBuffer( buffer );
                list->pop();
            }
            
        }
        
    private:
        
        
        engine::BufferListContainer* getBufferList( std::string name )
        {
            return buffer_lists.findOrCreate(name);
        }
        
    };
}

#endif