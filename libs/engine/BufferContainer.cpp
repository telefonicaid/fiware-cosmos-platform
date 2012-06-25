

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
        // If we have internally the same buffer, do nothing
        if( b == buffer )
            return;
        
        // If previous buffer, just release
        if( buffer )
            buffer->release();
        
        // If no input buffer, do nothing
        if( !b )
            return;
        
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

    
    
    
    BufferListContainer::~BufferListContainer()
    {
        clear(); // Clear vector releasing all included buffers
    }
    
    void BufferListContainer::push_back( Buffer* buffer )
    {
        buffer->retain();
        buffers.push_back(buffer);
    }
    
    void BufferListContainer::clear()
    {
        au::list<Buffer>::iterator it_buffers;      
        for( it_buffers = buffers.begin() ; it_buffers != buffers.end() ; it_buffers ++ )
            (*it_buffers)->release();
        buffers.clear();
    }
    
    Buffer* BufferListContainer::front()
    {
        return buffers.findFront();
    }
    
    void BufferListContainer::pop()
    {
        Buffer*buffer =  buffers.extractFront();
        if( buffer )
            buffer->release();
    }
    
    size_t BufferListContainer::getTotalSize()
    {
        size_t total = 0;
        au::list<Buffer>::iterator it; 
        for( it = buffers.begin() ; it != buffers.end() ; it++ )
            total += (*it)->getSize();
        return total;
    }
    
    void BufferListContainer::pop( engine::BufferContainer * container )
    {
        // Get the ponter to the next buffer
        engine::Buffer* buffer = front();
        if( !buffer )
            return;
        
        // Include the buffer in the container
        container->setBuffer( buffer );
        
        // Real pop from this list
        pop(); 
        
    }
    
    
    void BufferListContainer::extractFrom( BufferListContainer* other , size_t maximum_size )
    {
        int num=0;
        size_t total = 0;
        
        while( true )
        {
            Buffer* buffer = other->front();
            
            if( !buffer )
                return;
            
            
            if( num>0 )
                if( ( total + buffer->getSize() ) > maximum_size )
                    return;
            
            // Add this buffer in this list
            push_back( buffer );
            other->pop(); // Pop from the original list
        }
        
    }
    
    size_t BufferListContainer::getNumBuffers()
    {
        return buffers.size();
    }

    
    
    
}