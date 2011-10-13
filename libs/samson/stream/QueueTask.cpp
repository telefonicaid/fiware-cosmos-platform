
#include "samson/common/coding.h"

#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

#include "QueueTask.h"      // Own interface

#include "StreamManager.h"          // StreamOperation

namespace samson {
    namespace stream{
        
        QueueTask::QueueTask( size_t _id , StreamOperation* streamOperation  ) 
                : StreamProcessBase( _id , streamOperation ) , QueueTaskBase(_id)
        {
            // Set in the environemtn variables
            environment.setSizeT("system.queue_task_id",id);
            
        }

        
        void QueueTask::getInfo( std::ostringstream& output)
        {
            au::xml_open(output , "queue_task" );
            
            au::xml_simple( output , "id", id);

            // Common information about inputs used in this task contained in class QueueTask
            QueueTaskBase::getInfo( output );
            
            
            au::xml_simple(output, "state", queue_task_state );
            
            au::xml_simple(output, "input_0",  getBlockList("input_0")->strShortDescription() );
            au::xml_simple(output, "input_1",  getBlockList("input_1")->strShortDescription() );
            
            au::xml_simple(output, "operation", streamOperation->operation );
            
            if( streamOperation )
            {
                output << "<description>" << streamOperation->operation << "</description>\n";
            }
                        
            // Get all process item related information ( like progress )
            ProcessItem::getInfo( output );
            
            au::xml_close(output , "queue_task" );
        }
        
        
        
    }
        
}
