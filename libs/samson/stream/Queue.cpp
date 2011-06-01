
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
        
        void QueueChannel::add( Block *block )
        {
            // Retain the block to be an owner of the block
            stream::BlockManager::shared()->retain( block );
            
            
            // Insert in the back of the list
            blocks.push_back( block );
        }

        Block* QueueChannel::extract( )
        {
            return blocks.extractFront();
        }
        
        size_t QueueChannel::getSize()
        {
            size_t total = 0;
            for (  au::list< Block >::iterator i = blocks.begin() ; i!= blocks.end() ; i++)
                total += (*i)->getSize();
            return total;
        }
        
        std::string QueueChannel::getStatus()
        {
            size_t size=0;
            size_t size_on_memory=0;
            size_t size_on_disk=0;
            
            std::ostringstream output;
            std::list<Block*>::iterator b;
            
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                size += (*b)->getSize();
                size_on_memory += (*b)->getSizeOnMemory();
                size_on_disk += (*b)->getSizeOnDisk();
            }
            
            output << "[ " << blocks.size() << " blocks with " << au::Format::string( size, "Bytes" );
            output << " " << au::Format::percentage_string(size_on_memory, size) << " on memory";
            output << " & " << au::Format::percentage_string(size_on_disk, size) << " on disk ] ";

            return output.str();
        }
        
#pragma Queue
        
        Queue::Queue( std::string _name , QueuesManager * _qm ) 
        {
            name = _name;
            qm = _qm;
            streamQueue = NULL; // By default it is not assigned
            
            listen( notification_review_task_for_queue );
        }
        
        // Add a block to a particular queue
        
        void Queue::add( int _channel , Block *block )
        {
            QueueChannel * channel = channels.findOrCreate( _channel );
            channel->add( block );
        }
        
        void Queue::scheduleNewTasksIfNecessary()
        {
            if( !streamQueue )
                return;     // No information about how to process data
            
            int input_channel = 0 ;
            
            QueueChannel*channel = channels.findOrCreate( input_channel );
            
            size_t total_size   = channel->getSize();
            int total_time      = cronometer.diffTimeInSeconds();

            if( total_size == 0 )
                return;
            
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
                        while( channel->getNumBlocks() > 0 )
                            tmp->add( channel->extract() );
                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                        // reset the cronometer for the next operation
                        cronometer.reset();
                        
                    }
                    break;
                    
                case Operation::map:
                    {
                        MapQueueTask *tmp = new MapQueueTask( streamQueue ); 
                        while( channel->getNumBlocks() > 0 )
                            tmp->add( channel->extract() );
                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                        // reset the cronometer for the next operation
                        cronometer.reset();
                        
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
            
            output << "\tQueue " << name << ":\n";
            
            // Information about channels
            au::map<int , QueueChannel>::iterator c;
            for ( c = channels.begin() ; c != channels.end() ; c++)
                output << "\t\tChannel " << c->first << " : " << c->second->getStatus() << "\n";
            
            if( streamQueue )
                output << "\t\t\t --> Process with " << streamQueue->operation() << "\n";
            
            return output.str();
        }
        
        ::samson::NetworkInterface* Queue::getNetwork()
        {
            return qm->worker->network;
        }

        
        
    }
}