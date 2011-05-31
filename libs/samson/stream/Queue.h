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
#include "au/cronometer.h"              // au::cronometer
#include "samson/common/samson.pb.h"    // network::
#include "engine/EngineNotification.h"        // engine::NotificationListener

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
            au::list< Block > blocks;       // Blocks currently in the input queue
            
            au::Cronometer cronometer;      // Time since the last command execution
            
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
            
            void add( Block *block );

            // Create new tasks if necessary
            void scheduleNewTasksIfNecessary();
            
            std::string getStatus();
          
            ::samson::NetworkInterface *getNetwork();
            
            size_t getSize();
            
            // Notifications    
            void notify( engine::Notification* notification );
            bool acceptNotification( engine::Notification* notification )
            {
                return true;
            }
            
        };
        
        
    }
}

#endif