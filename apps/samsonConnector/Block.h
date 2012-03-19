#ifndef _H_SAMSON_CONNECTOR_BLOCK
#define _H_SAMSON_CONNECTOR_BLOCK

#include "logMsg/logMsg.h"

#include "au/string.h"

#include "engine/MemoryManager.h"
#include "engine/Buffer.h"

#include "samson/common/KVHeader.h"
#include "samson/delilah/TXTFileSet.h" // DataSource

namespace engine {
    class Buffer;
}

namespace samson {
    
    class Block
    {
        int retain_counter;
        
    public:

        engine::Buffer *buffer;
        
        Block( engine::Buffer *_buffer )
        {
            buffer = _buffer;
            retain_counter = 1;
            LM_V(("Created a block %s" , au::str(buffer->getSize()).c_str() ));
            
            if( !buffer )
                LM_X(1, ("Internall error creating block with a NULL buffer"));
            

        }
        
        ~Block()
        {
            LM_V(("Destroying a block %s" , au::str(buffer->getSize()).c_str() ));
            engine::MemoryManager::shared()->destroyBuffer(buffer);
        }
        
        
        
        void retain()
        {
            retain_counter++;
            LM_V(("Retain (%d) a block %s" , retain_counter , au::str(buffer->getSize()).c_str() ));
        }
        
        void release()
        {
            retain_counter--;
            LM_V(("Release (%d) a block %s" , retain_counter , au::str(buffer->getSize()).c_str() ));
            
            if( retain_counter == 0 )
                delete this;
        }
        
    };
    
    class BlockDataSource : public DataSource
    {
        Block* block;
        bool finish;
        
        public:
        
        BlockDataSource( Block* _block )
        {
            block = _block;
            finish = false;
            
            block->retain();
        }
        
        ~BlockDataSource()
        {
            block->release();
        }
        
        bool isFinish()
        {
            return finish;
        }
        
		virtual int fill( engine::Buffer *b )
        {
            if( b->getMaxSize() < ( block->buffer->getSize() + sizeof(KVHeader) ) )
                LM_X(1,("BlockDataSource: Not possible to fill this buffer."));
            
            KVHeader header;
            header.initForTxt( block->buffer->getSize() );

            memcpy(b->getData() , &header , sizeof(KVHeader) );
            memcpy(b->getData() + sizeof(KVHeader), block->buffer->getData(), block->buffer->getSize() );
            b->setSize( block->buffer->getSize() + sizeof(KVHeader) );
            
            finish = true;
            
            return 0;
        }
        
        size_t getTotalSize()
        {
            return block->buffer->getSize();
        }

        
    };


}


#endif