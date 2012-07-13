

#include "samson/stream/BlockList.h"        // samson::stream::BlockList
#include "samson/stream/StreamManager.h"    // samson::stream::StreaMmanager
#include "samson/stream/PopQueueTask.h"     // StreamOutQueueTask

#include "StreamOutput.h"   // Own interface


namespace samson {
    namespace stream {

#pragma mark StreamOutQueue
        
        StreamOutQueue::StreamOutQueue( std::string _queue , bool _flag_remove )
        {
            queue = _queue;
            list = new BlockList("StreamOutQueue " + queue );
            
            flag_remove = _flag_remove;
        }
        
        StreamOutQueue::~StreamOutQueue()
        {
            //LM_M(("Destructor StreamOutQueue. Removing list %s", list->strShortDescription().c_str() ));
            delete list;
        }
        
        void StreamOutQueue::push( BlockList* _list )
        {
            if( flag_remove )
                list->extractFrom( _list , 0 );
            else
                list->copyFrom( _list );
        }
        
        bool StreamOutQueue::extractBlockFrom( BlockList *_list )
        {
            return _list->extractBlockFrom( list );
        }
        
#pragma mark StreamOutConnection
        
        StreamOutConnection::StreamOutConnection( StreamManager *_streamManager , size_t _fromId  )
        {
            streamManager = _streamManager;         // Pointer to the stream manager
            fromId = _fromId;
            
            max_num_stream_out_queue_tasks = 2;     // Some default value
        }
        
        StreamOutConnection::~StreamOutConnection()
        {
            //LM_M(("Destructor StreamOutConnection"));
            stream_out_queues.clearMap();
            stream_out_queues.clear();
        }
        
        void StreamOutConnection::push( std::string queue , BlockList* list )
        {
            StreamOutQueue *stream_out_queue = stream_out_queues.findInMap( queue );
            if( stream_out_queue )
                stream_out_queue->push( list );
        }
        
        StreamOutQueue* StreamOutConnection::add_queue( std::string queue , bool _flag_remove )
        {
            StreamOutQueue* stream_out_queue = new StreamOutQueue(queue , _flag_remove);
            
            // Add a new listener for this queue ( if there was a previous one, is removed )
            stream_out_queues.insertInMap(queue, stream_out_queue );
                
            return stream_out_queue;
        }
        
        void StreamOutConnection::remove_queue( std::string queue )
        {
            // Add a new listener for this queue ( if there was a previous one, is removed )
            stream_out_queues.removeInMap( queue );
                
        }
        
        void StreamOutConnection::scheduleNextTasks()
        {
            
            //LM_M(("scheduleNextTasks"));
            
            // Check all the queues
            au::map<std::string,StreamOutQueue>::iterator  it_stream_out_queues;
            for( it_stream_out_queues = stream_out_queues.begin() ; it_stream_out_queues != stream_out_queues.end() ; it_stream_out_queues++)
            {
                StreamOutQueue* stream_out_queue = it_stream_out_queues->second;
                std::string queue = it_stream_out_queues->first;
                
                //LM_M(("scheduleNextTasks queue %s" , queue.c_str() ));
                
                while( true )
                {
                    
                    // If we have created the maximum number of operations, just quit...
                    if( running_stream_out_queue_tasks.size() >= (size_t ) max_num_stream_out_queue_tasks )
                        return;  // No more operations can be scheduled
                    
                    // Extract the next block...
                    BlockList list;
                    
                    if(  stream_out_queue->extractBlockFrom( &list ) )
                    {
                        if( list.isEmpty() )
                            LM_X(1,("Internal error"));
                        
                        // Create a new  StreamOutQueue operation
                        size_t id = streamManager->getNewId();   // New id for this operation
                        StreamOutQueueTask * stream_out_queue_task = new StreamOutQueueTask( id , fromId, queue );
                        stream_out_queue_task->getBlockList("input_0")->copyFrom( &list );
                        
                        // Schedule the new task
                        streamManager->queueTaskManager.add( stream_out_queue_task );

                        //LM_M(("scheduleNextTasks created a packet for queue %s" , queue.c_str() ));
                        
                    }
                    else
                    {
                        //LM_M(("scheduleNextTasks No more blocks for queue %s" , queue.c_str() ));
                        break;  // Break the while loop
                    }
                    
                }
            }
        }
        
        
        // Get XML monitoring information
        void StreamOutConnection::getInfo( std::ostringstream& output)
        {
            au::xml_open(output,"stream_out_connection");
            
            au::xml_simple( output , "fromId" , fromId );
            au::xml_simple( output , "max_num_tasks" , max_num_stream_out_queue_tasks );
            
            au::xml_iterate_map(output, "stream_out_queues", stream_out_queues);
            
            au::xml_close(output,"stream_out_connection");
        }
        
        
        void StreamOutConnection::fill( samson::network::CollectionRecord* record , Visualization* visualization )
        {
            // Delilah identifier
            add( record , "delilah" , au::code64_str( fromId ) , "left,different" );

            // List of queues with pending size
            std::ostringstream queues_description;
            au::map<std::string,StreamOutQueue>::iterator it;
            for (it = stream_out_queues.begin() ; it != stream_out_queues.end() ; it++ )
                queues_description << it->second->str() << " ";
                
            add( record , "queues" , queues_description.str() , "left,different" );

            // Max number of operations for this connection
            add( record , "Max pop operations" , max_num_stream_out_queue_tasks , "f=uint64,sum" );
        }
        
    }
}
