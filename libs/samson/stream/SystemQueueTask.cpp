
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList

#include "Queue.h"                                  // samson::stream::Queue

#include "SystemQueueTask.h"                        // Own interface

namespace samson {
    namespace stream {
        
        SystemQueueTask::SystemQueueTask(size_t _id  , std::string _concept) : engine::ProcessItem( PI_PRIORITY_NORMAL_OPERATION ) , QueueTaskBase( _id )
        {
            concept = _concept;
            
            // Set the id
            environment.setSizeT("system.queue_task_id", id);
            
            // To be recognized by the QueueTaskManager as a system task
            environment.set("system.system_queue_task" , "yes" );
        }
        
        SystemQueueTask::~SystemQueueTask()
        {
        }
        
        void SystemQueueTask::getInfo( std::ostringstream& output )
        {
            au::xml_open(output , "queue_task" );
            
            au::xml_simple( output , "id", id);
            
            // Common information about inputs used in this task contained in class QueueTask
            QueueTaskBase::getInfo( output );
            
            output << "<description>" << concept << "</description>\n";
            
            // Get all process item related information ( like progress )
            ProcessItem::getInfo( output );
            
            au::xml_close(output , "queue_task" );
        }
        
    }
}


        