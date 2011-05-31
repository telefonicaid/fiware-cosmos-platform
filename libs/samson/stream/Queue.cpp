
#include <sstream>       

#include "Queue.h"          // Own interface
#include "QueuesManager.h"  // samson::stream::QueuesManager
#include "Block.h"          // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager
    
#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/module/ModulesManager.h"           
#include "QueueTask.h"
#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/ParserQueueTask.h"          // samson::stream::ParserQueueTask


namespace samson {
    namespace stream
    {
        
        
        Queue::Queue( std::string _name , QueuesManager * _qm ) 
        {
            name = _name;
            qm = _qm;
            streamQueue = NULL; // By default it is not assigned
            
            listen( notification_review_task_for_queue );
        }
        
        // Add a block to a particular queue
        
        void Queue::add( Block *block )
        {
            
            // Retain the block to be an owner of the block
            stream::BlockManager::shared()->retain( block );
            
            if ( blocks.size() == 0)
                cronometer.reset();
            
            // Insert in the back of the list
            blocks.push_back( block );
        }
        
        void Queue::scheduleNewTasksIfNecessary()
        {
            if( !streamQueue )
                return;     // No information about how to process data
            
            if( blocks.size() == 0)
                return;     // Nothing to process
            
            size_t total_size = getSize();
            int total_time = cronometer.diffTimeInSeconds();
            
            if( ( total_size < 1000000) && (total_time < 3) )
            {
                LM_M(("Not run parser for size-time reasons size=%s time=%s", au::Format::string(total_size).c_str() , au::Format::time_string( total_time).c_str()  ));
                return;
            }
            
            Operation* op = samson::ModulesManager::shared()->getOperation( streamQueue->operation() );
            
            switch (op->getType()) {
                case Operation::parser:
                    
                    {
                        ParserQueueTask *tmp = new ParserQueueTask( streamQueue ); 
                        while( blocks.size() > 0 )
                            tmp->add( blocks.extractFront() );
                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                    }
                    break;
                    
                case Operation::map:
                    {
                        MapQueueTask *tmp = new MapQueueTask( streamQueue ); 
                        while( blocks.size() > 0 )
                            tmp->add( blocks.extractFront() );
                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                    }
                    break;
                    
                    
                default:
                    LM_X(1, ("Operation not supported"));
                    // No processing at the moment
                    break;
            }
            
        }

        void Queue::notify( engine::Notification* notification )
        {
            if( !notification->isName(notification_review_task_for_queue) )
                LM_X(1, ("Internal error: Unexpected notification at Queue (stream)"));

            scheduleNewTasksIfNecessary();
            
        }

        // Print the status of a particular queue
        
        std::string Queue::getStatus()
        {
            
            std::ostringstream output;
            
            // Get information about size on memory / on disk
            
            size_t size=0;
            size_t size_on_memory=0;
            size_t size_on_disk=0;
            
            std::list<Block*>::iterator b;
            
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                size += (*b)->getSize();
                size_on_memory += (*b)->getSizeOnMemory();
                size_on_disk += (*b)->getSizeOnDisk();
            }
            
            output << "[ Queue " << name << " : " << blocks.size() << " blocks with " << au::Format::string( size, "Bytes" );
            output << " " << au::Format::percentage_string(size_on_memory, size) << " on memory";
            output << " & " << au::Format::percentage_string(size_on_disk, size) << " on disk ] ";
            
            if( streamQueue )
                output << " ( Process with " << streamQueue->operation() << " ) ";
            
            return output.str();
        }
        
        ::samson::NetworkInterface* Queue::getNetwork()
        {
            return qm->worker->network;
        }

        size_t Queue::getSize()
        {
            size_t total = 0;
            for (  au::list< Block >::iterator i = blocks.begin() ; i!= blocks.end() ; i++)
                total += (*i)->getSize();
            return total;
        }
        
        
    }
}