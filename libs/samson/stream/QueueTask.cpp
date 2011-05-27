
#include "QueueTask.h"      // Own interface
#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

namespace samson {
    namespace stream{

        void QueueTask::add( Block * b )
        {
            blocks.insert( b );
        }

        
        bool QueueTask::ready()
        {
            std::set<Block*>::iterator i;
            for ( i = blocks.begin() ; i != blocks.end() ; i++)
                if( !(*i)->isContentOnMemory() )
                    return false;
            return true;
        }
        
        void QueueTask::retain()
        {
            std::set<Block*>::iterator i;
            for ( i = blocks.begin() ; i != blocks.end() ; i++)
                BlockManager::shared()->retain(*i, id );
        }
        
        void QueueTask::release()
        {
            std::set<Block*>::iterator i;
            for ( i = blocks.begin() ; i != blocks.end() ; i++)
                BlockManager::shared()->release(*i, id );
        }

        bool QueueTask::lock()
        {
            return BlockManager::lock( blocks );
        }
        
        void QueueTask::unlock()
        {
            BlockManager::unlock( blocks );
        }
        
        
        
        
    }
        
}