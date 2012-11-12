/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks
#include "samson/common/MessagesOperations.h"

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
            
            if( ProcessItem::isRunning() )
                au::xml_simple(output, "state", "running" );
            else
                au::xml_simple(output, "state", queue_task_state );
            
            au::xml_simple(output, "input_0",  getBlockList("input_0")->strShortDescription() );
            au::xml_simple(output, "input_1",  getBlockList("input_1")->strShortDescription() );
            
            au::xml_simple(output, "operation", "system.BlockBreak" );
            
            output << "<description>system.BlockBreak</description>\n";
            
            // Get all process item related information ( like progress )
            ProcessItem::getInfo( output );
            
            au::xml_close(output , "queue_task" );
        }
        
        
        void SystemQueueTask::fill( samson::network::CollectionRecord* record , VisualitzationOptions options )
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


        