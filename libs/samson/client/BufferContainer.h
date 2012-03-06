#ifndef _H_Buffercontainer_SamsonClient
#define _H_Buffercontainer_SamsonClient


namespace  samson {
    
    class BufferList
    {
        au::list< engine::Buffer > buffers;
        
    public:
        
        void push( engine::Buffer* buffer )
        {
            if( !buffer )
                return;
            
            buffers.push_back( buffer );
        }
        
        engine::Buffer *pop()
        {
            return buffers.extractFront();
        }
        
    };
    
    
    class BufferContainer 
    {
        au::Token token;
        au::map< std::string , BufferList > buffer_lists;    // Buffers for live data
        
    public:
        
        BufferContainer( ) : token("BufferContainer")
        {
            // listen...
        }
        
        void push( std::string queue , engine::Buffer* buffer )
        {
            au::TokenTaker tt(&token);
            getBufferList( queue )->push( buffer );
        }
        
        engine::Buffer* pop( std::string queue )
        {
            au::TokenTaker tt(&token);
            return getBufferList(queue)->pop();
        }
        
    private:
        
        BufferList* getBufferList( std::string name )
        {
            return buffer_lists.findOrCreate(name);
        }
        
    };
}

#endif