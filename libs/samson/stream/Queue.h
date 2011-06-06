#ifndef _H_STREAM_QUEUE
#define _H_STREAM_QUEUE

/* ****************************************************************************
 *
 * FILE                      Queue.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * All the information related with a queue ( data and state )
 *
 */


#include <ostream>      // std::ostream
#include <string>       // std::string

#include "au/list.h"      // au::list
#include "au/Cronometer.h"              // au::cronometer

#include "samson/common/coding.h"           // FullKVInfo

#include "samson/common/samson.pb.h"    // network::
#include "engine/EngineNotification.h"        // engine::NotificationListener

#include "samson/stream/BlockMatrix.h"      // samson::Stream::BlockMatrix

#define notification_review_task_for_queue "notification_review_task_for_queue"

namespace samson {
    
    class NetworkInterface;
    
    namespace stream
    {
        class Block;
        class QueuesManager;
        
        class Queue : engine::NotificationListener
        {
            QueuesManager * qm;

            friend class QueuesManager;
            
            std::string name;               // Name of the queue
            au::Cronometer cronometer;      // Time since the last command execution
            
            BlockMatrix matrix;             // Matrix of blocks ( one list per input channel )
            

            
            std::set<size_t> running_tasks;     // Tasks currently running 
            
            
        public:
            
            // Information about how to process this queue ( from controller )
            network::StreamQueue *streamQueue;
            
            Queue( std::string _name , QueuesManager * _qm );
            
            ~Queue()
            {
                if( streamQueue )
                    delete streamQueue;
                
            }
            
            void setStreamQueue( network::StreamQueue& _streamQueue )
            {
                // Only if not set before
                if( ! streamQueue )
                {
                    streamQueue = new network::StreamQueue();
                    streamQueue->CopyFrom(_streamQueue);
                }
            }
            
            void add( int channel , Block *block );
        

            // Create new tasks if necessary
            void scheduleNewTasksIfNecessary();
            
            std::string getStatus();
            
            // Notifications    
            void notify( engine::Notification* notification );
            bool acceptNotification( engine::Notification* notification )
            {
                return true;
            }
            
            void notifyFinishTask( size_t task_id );
            
        };
        
        
    }
}

#endif
