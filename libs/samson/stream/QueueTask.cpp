
#include "QueueTask.h"      // Own interface
#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

namespace samson {
    namespace stream{
        
        bool QueueTask::ready()
        {
            return matrix.isContentOnMemory();
        }
        
        void QueueTask::retain()
        {
            matrix.retain(id);
        }
        
        void QueueTask::release()
        {
            matrix.release(id);
        }

        bool QueueTask::lock()
        {
            if( matrix.isContentOnMemory() )
            {
                matrix.lock();
                return true;
            }
            else
                return false;
        }
        
        void QueueTask::unlock()
        {
            matrix.unlock();
        }
        
        void QueueTask::getInfo( std::ostringstream& output)
        {
            output << "<queue_task id=\"" << id << "\">\n";
            
            output << "<id>" << id << "</id>\n";
            
            matrix.getInfo( output );

            if( streamQueue )
            {
                output << "<description>" << streamQueue->operation() << "</description>\n";
            }
            
            // Information about pop queue operation( parser Out from delilah )
            if ( pq )
            {
            }
            
            // Get all process item related information ( like progress )
            ProcessItem::getInfo( output );
            
            output << "</queue_task>\n";
        }
        
        
        
    }
        
}