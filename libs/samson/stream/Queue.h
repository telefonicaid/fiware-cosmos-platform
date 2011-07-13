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

#include "engine/Object.h"        // engine::Object

#include "samson/stream/BlockMatrix.h"      // samson::Stream::BlockMatrix

#define notification_review_task_for_queue "notification_review_task_for_queue"

namespace samson {
    
    class NetworkInterface;
    class Info;
    
    namespace stream
    {
        class Block;
        class QueuesManager;
        class PopQueue;
        
        class Queue : engine::Object
        {
            QueuesManager * qm;

            friend class QueuesManager;
            
            std::string name;                   // Name of the queue
            au::Cronometer cronometer;          // Time since the last command execution
            
            BlockMatrix matrix;                 // Matrix of blocks ( one list per input channel )
            
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
            
            void setStreamQueue( network::StreamQueue& new_streamQueue )
            {
                
                if( streamQueue )
                    delete streamQueue;
                
                streamQueue = new network::StreamQueue();
                streamQueue->CopyFrom(new_streamQueue);
                
            }
            
            void add( int channel , Block *block );
            
            
            // Create new tasks if necessary
            void scheduleNewTasksIfNecessary();

            // Create tasks if necessary for a pop-queue operation ( from delilah )
            void scheduleTasksForPopQueue( PopQueue *popQueue );
           
            std::string getStatus();
            
            // Notifications    
            void notify( engine::Notification* notification );
            
            void notifyFinishTask( size_t task_id );

            
            // Get information for monitorization
            
            Info* getInfo();
            
            
        };
        
        
    }
}

#endif
