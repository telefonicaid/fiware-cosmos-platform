#ifndef _H_SAMSON_CONNECTOR_BLOCK
#define _H_SAMSON_CONNECTOR_BLOCK

#include "logMsg/logMsg.h"

#include "au/string.h"

#include "engine/MemoryManager.h"
#include "engine/BufferContainer.h"

#include "samson/common/KVHeader.h"
#include "samson/delilah/TXTFileSet.h" // DataSource

namespace engine {
    class Buffer;
}

namespace samson 
{
    

    class BlockDataSource : public DataSource
    {
        // Container of the buffer
        engine::BufferContainer buffer_container;
        
        bool finish;
        
        public:
        
        BlockDataSource( engine::Buffer* buffer )
        {
            buffer_container.setBuffer(buffer);
            finish = false;
        }
        
        ~BlockDataSource()
        {
        }
        
        bool isFinish()
        {
            return finish;
        }
        
		virtual int fill( engine::Buffer *b )
        {
            engine::Buffer* buffer = buffer_container.getBuffer();
            
            if( b->getMaxSize() < ( buffer->getSize() + sizeof(KVHeader) ) )
                LM_X(1,("BlockDataSource: Not possible to fill this buffer."));
            
            KVHeader header;
            header.initForTxt( buffer->getSize() );

            memcpy(b->getData() , &header , sizeof(KVHeader) );
            memcpy(b->getData() + sizeof(KVHeader), buffer->getData(), buffer->getSize() );
            b->setSize( buffer->getSize() + sizeof(KVHeader) );
            
            finish = true;
            
            return 0;
        }
        
        size_t getTotalSize()
        {
            return buffer_container.getBuffer()->getSize();
        }

        
    };


}


#endif