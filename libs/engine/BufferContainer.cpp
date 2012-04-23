

#include "engine/Buffer.h"
#include "engine/MemoryManager.h"

#include "BufferContainer.h" // Own interface


namespace engine {

    BufferContainer::BufferContainer()
    {
        buffer = NULL;
    }
    
    BufferContainer::~BufferContainer()
    {
        if( buffer )
            buffer->release();
    }
    
    Buffer* BufferContainer::getBuffer()
    {
        return buffer;
    }
    
    void BufferContainer::setBuffer( Buffer * b )
    {
        // If no input buffer, do nothing
        if( !b )
            return;
        
        // If we have internally the same buffer, do nothing
        if( b == buffer )
            return;
        
        // If previous buffer, just release
        if( buffer )
            buffer->release();
        
        // Keep a retained copy of this
        buffer = b;
        buffer->retain();
    }
    
    void BufferContainer::release()
    {
        if( buffer )
        {
            buffer->release();
            buffer = NULL;
        }
    }
    
    void BufferContainer::operator=( BufferContainer& other )
    {
        setBuffer( other.buffer );
    }
    
    // Create a buffer and retain internally
    Buffer* BufferContainer::create( std::string name , std::string type , size_t size )
    {
        release(); // Relase internal buffer if any
        buffer = MemoryManager::shared()->createBuffer("output_splitter", "connector", size );
        return buffer;
    }

    
}