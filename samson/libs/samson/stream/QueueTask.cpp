
#include "samson/common/coding.h"

#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

#include "StreamOperation.h"          // StreamOperation

#include "QueueTask.h"      // Own interface

namespace samson {
    namespace stream{
        
        QueueTask::QueueTask( size_t _id , StreamOperationBase* streamOperation  ) 
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
            
            
            if( ProcessItem::isRunning() )
                au::xml_simple(output, "state", "running" );
            else
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

         
        void QueueTask::fill( samson::network::CollectionRecord* record , VisualitzationOptions options )
        {
            
            // Get block information for this queue
            BlockInfo blockInfo;
            update( blockInfo );
            
            add( record , "id" , id , "left,different" );
            add( record , "worker_command_id" , environment.get("worker_command_id", "no_id") , "left,different" );
            add( record , "creation" , creation_cronometer.diffTime() , "f=time,different" );
            add( record , "running " , cronometer.getSeconds() , "f=time,different" );
            add( record , "progress " , progress , "f=percentadge,different" );
            
            if( ProcessItem::isRunning() )
                add( record , "state" , "running" , "left,different" );
            else
                add( record , "state" , queue_task_state , "left,different" );
            
            add( record , "operation" , operation_name , "left,different" );
            
            add( record , "input_0" , getBlockList("input_0")->getBlockInfo().strShort() , "different");
            add( record , "all_inputs" , getUniqueBlockInfo().strShort()  , "different");
            
            if( options == verbose )
            {
                // NO more informaiton for verbose
            }
            
            
        }   
        
    }
        
}
