
#include "samson/common/coding.h"

#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

#include "QueueTask.h"      // Own interface

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

            if( ready_flag )
            {
                // Set the working size for statistics
                BlockInfo block_info;
                
                list_lock->update( block_info );
                state_lock->update( block_info );
                
                working_size = block_info.size;
 
            }
            
            return ready_flag;
        }
        
        
        void QueueTask::getInfo( std::ostringstream& output)
        {
            au::xml_open(output , "queue_task" );
            
            au::xml_simple( output , "id", id);
            
            au::xml_simple( output , "input_in_memory" , list->isContentOnMemory() );
            au::xml_simple( output , "state_in_memory" , state->isContentOnMemory() );
            
            list->getInfo( output );

            if( streamOperation )
            {
                output << "<description>" << streamOperation->operation() << "</description>\n";
            }
                        
            // Get all process item related information ( like progress )
            ProcessItem::getInfo( output );
            
            au::xml_close(output , "queue_task" );
        }
        
        
        
    }
        
}