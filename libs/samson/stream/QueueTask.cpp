
#include "QueueTask.h"      // Own interface
#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

namespace samson {
    namespace stream{
        
        bool QueueTask::ready()
        {
            if( ready_flag )
                return true;

            ready_flag = (list->isContentOnMemory() && state->isContentOnMemory() ) ;
            
            if( ready_flag )
            {
                state_lock->copyFrom( state );
                list_lock->copyFrom( list );
            }
            
            return ready_flag;
        }
        
        
        void QueueTask::getInfo( std::ostringstream& output)
        {
            output << "<queue_task id=\"" << id << "\">\n";
            
            output << "<id>" << id << "</id>\n";
            
            list->getInfo( output );

            if( streamOperation )
            {
                output << "<description>" << streamOperation->operation() << "</description>\n";
            }
                        
            // Get all process item related information ( like progress )
            ProcessItem::getInfo( output );
            
            output << "</queue_task>\n";
        }
        
        
        
    }
        
}