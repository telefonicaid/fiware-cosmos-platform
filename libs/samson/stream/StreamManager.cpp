

#include <string>                                   // std::string
#include <algorithm>                                // std::ort
#include <sstream>                                  // std::stringstream
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fnmatch.h>

#include "au/time.h"                                // au::todayString()
#include "au/file.h"            
#include "engine/Engine.h"                          // engine::Engine
#include "engine/MemoryManager.h"
#include "engine/Notification.h"
#include "engine/DiskOperation.h"
#include "engine/DiskManager.h"
#include "samson/common/EnvironmentOperations.h"    // copyEnviroment
#include "samson/common/SamsonSetup.h" 
#include "samson/common/MessagesOperations.h"
#include "samson/module/ModulesManager.h"           // ModulesManager
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/BlockBreakQueueTask.h"      // samson::stream::BlockBreakQueueTask
#include "samson/stream/BlockManager.h"             // samson::BlockManager
#include "samson/stream/PopQueue.h"                 // stream::PopQueue
#include "samson/stream/QueueTask.h"
#include "samson/stream/Queue.h"
#include "samson/stream/QueueTasks.h"               // samson::stream::ParserQueueTask
#include "samson/stream/BlockList.h"                // BlockList
#include "samson/stream/Queue.h"
#include "samson/stream/SystemQueueTask.h"
#include "samson/stream/PopQueueTask.h"
#include "samson/stream/StreamOperation.h"
#include "samson/stream/StreamOutput.h"
#include "Queue.h"                                  // samson::stream::Queue
#include "Block.h"                                  // samson::stream::Block

#include "StreamManager.h"                          // Own interface



namespace samson {
    namespace stream {
    
        
        
        StreamManager::StreamManager(::samson::SamsonWorker* _worker) : queueTaskManager( this )
        {
            worker = _worker;
            
            
            {
                engine::Notification *notification = new engine::Notification(notification_review_stream_manager_fast);
                engine::Engine::shared()->notify( notification, 1 );
            }
            {
                engine::Notification *notification = new engine::Notification(notification_review_stream_manager_save_state);
                engine::Engine::shared()->notify( notification, 5 );
            }
            
            
            id_pop_queue = 1;    // Init the id counter for pop queue operations

            // Init flag to detect if we are saving state to disk
            currently_saving = false;

            // Engine listening commands
            listen( notification_review_stream_manager_fast );
            listen( notification_review_stream_manager_save_state );
            listen( notification_samson_worker_check_finish_tasks );
            listen( notification_network_diconnected );
            listen( notification_recoverStateFromDisk );
            
            // Create a notification for the recover state from disk
            engine::Engine::shared()->notify( new engine::Notification( notification_recoverStateFromDisk ) );
        }

        
        int get_num_divisions( size_t size )
        {
            size = 0;
            return 2;   // Just for testing...
        }
        
        void StreamManager::notify( engine::Notification* notification )
        {
            
            if( notification->isName( notification_recoverStateFromDisk ) )
            {
                // Recover state from log-file
                LM_M(("Recovering state from disk..."));
                recoverStateFromDisk();
                LM_M(("Recovering state from disk...done"));
                return;
            }
            
            if( notification->isName( notification_network_diconnected ) )
            {
                int id = notification->environment.getInt("id",-1);
                //LM_M(("Removing stream out queue for id %d", id ));
                stream_out_connections.removeInMap( id );
                return;
            }
            
            if ( notification->isName(notification_review_stream_manager_fast) )
            {
                // Review stream operations to be executed
                reviewStreamOperations();
                
                // Review queues
                au::map< std::string , Queue >::iterator it_queues;                
                for ( it_queues = queues.begin() ; it_queues != queues.end() ; it_queues++ )
                    it_queues->second->review();

                // Review StreamOutConnections
                au::map< size_t , StreamOutConnection >::iterator it;
                for( it =  stream_out_connections.begin() ; it != stream_out_connections.end() ; it++ )
                    it->second->scheduleNextTasks();    // It returns NULL when no more operations are scheduled
                
                return;
            }
                
            if ( notification->isName(notification_review_stream_manager_save_state) )
            {
                
                // Save state to disk just in case it crases
                saveStateToDisk();
                return;
            }
            
            if( notification->isName( notification_samson_worker_check_finish_tasks ) )
            {
                std::vector<size_t> ids;
                
                au::map< size_t , PopQueue >::iterator q;
                for ( q = popQueues.begin() ; q != popQueues.end() ; )
                {
                    if ( q->second->finished )
                    {
                        // Remove the object contained here
                        delete q->second;
                        
                        // Remove the element in the map
                        popQueues.erase(q++);
                    }
                    else
                        ++q;
                }
                return;
            }
            
            if ( notification->isName( notification_disk_operation_request_response ) )
            {
                if( notification->environment.get("save_stream_state", "no") == "yes" )
                {
                    // Dissable flag to save again next time...
                    currently_saving = false;
                }
                else
                {
                    LM_W(("Unknown disk notifiction in stream manafer %s" , notification->getDescription().c_str() ));
                }
                
                return;
            }
            
            
            LM_W(("Unknown notification at StreamManager"));
        }
        
        std::vector<std::string> StreamManager::getConnectedQueuesForQueue( std::string queue_name )
        {
            QueueConnections* connections = queue_connections.findInMap( queue_name );
            if( !connections )
                return std::vector<std::string>();
            else
                return  connections->getTargetQueues();
        }
        
        std::vector<std::string> StreamManager::getStreamOperationsForQueues( std::string queue )
        {
        
            std::vector<std::string> stream_operation_names;
            
            // First review all stream operations
            au::map <std::string , StreamOperation>::iterator it_stream_operations;
            for (it_stream_operations = stream_operations.begin() ; it_stream_operations != stream_operations.end() ; it_stream_operations++ )
            {
                StreamOperation *stream_operation = it_stream_operations->second;
                
                if( stream_operation->input_queues.size() > 0 )
                    if( stream_operation->input_queues[0] == queue )
                        stream_operation_names.push_back( it_stream_operations->first );
            }
            return stream_operation_names;
        }
        
        
        void StreamManager::review_queue( std::string queue_name )
        {
            
            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Get the list of stream-operations and connections for this queue
            std::vector<std::string> stream_operation_names = getStreamOperationsForQueues( queue_name );
            std::vector<std::string> queue_connections = getConnectedQueuesForQueue( queue_name );
            
            if( ( stream_operation_names.size() > 0 ) || (queue_connections.size() > 0 ) )
            {
                // Extract data to be push to all connectd queues and operations...
                // Extract all blocks
                BlockList tmp( "queue_to_connections_or_stream_operations" );
                tmp.extractFrom( queue->list );
                
                
                // Connected queues
                for( size_t i = 0 ; i < queue_connections.size() ; i++ )
                    addBlocks( queue_connections[i] , &tmp );
                
                // Stream operations
                for( size_t i = 0 ; i < stream_operation_names.size() ; i++ )
                {
                    StreamOperation * stream_operation = stream_operations.findInMap( stream_operation_names[i] );
                    if (stream_operation)
                        stream_operation->push( &tmp );
                }
            }
            
            // review all the automatic operations ( maybe we can only review affected operations in the future... )
            reviewStreamOperations();            
        }
        
        void StreamManager::addBlocks( std::string queue_name ,  BlockList *list )
        {
            // Review stream_out_queues...
            au::map< size_t , StreamOutConnection >::iterator it_connections;
            for( it_connections = stream_out_connections.begin() ; it_connections != stream_out_connections.end() ; it_connections++)
                it_connections->second->push( queue_name , list );

            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Add the blocks to the queue
            queue->push( list );

            // Review if content of this queue is redistributed
            review_queue( queue_name );
        }
     
        void StreamManager::add( StreamOperation* operation )
        {
            
            StreamOperation* previous = stream_operations.extractFromMap( operation->name );
            if( previous )
                delete previous;

            stream_operations.insertInMap( operation->name , operation );
            
            // Review input queue
            if ( operation->input_queues.size() > 0 )
                review_queue( operation->input_queues[0] );
            
            // review all the operations...
            reviewStreamOperations();
        }
        
        
        void StreamManager::remove_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
            {
                // Remove all the elements in the blocklist to be reviews ( and free from memery if necessar y)
                queue->list->clearBlockList();
                delete queue;
            }

        }
        
        
        void StreamManager::push_queue( std::string from_queue_name , std::string to_queue_name )
        {
            Queue *fromQueue = getQueue( from_queue_name );
            Queue *toQueue = getQueue( to_queue_name );

            toQueue->push( fromQueue->list );
            
            
        }
        
        Queue* StreamManager::getQueue( std::string queue_name )
        {
            Queue *queue = queues.findInMap( queue_name );
            if (! queue )
            {
                queue = new Queue( queue_name , this );
                queues.insertInMap( queue_name, queue );
            }
            
            return queue;
            
        }
        
        StreamOperation* StreamManager::getStreamOperation( std::string name )
        {
            StreamOperation* so = stream_operations.findInMap(name);

            return so;
        }

        
        void StreamManager::notifyFinishTask( QueueTask *task )
        {
            task = NULL;
        }
        
        void StreamManager::notifyFinishTask( SystemQueueTask *task )
        {
            bool is_pop_queue_task = ( task->environment.get("system.pop_queue_task" ,"no") == "yes" );
            
            if( is_pop_queue_task )
            {
                size_t task_id      = task->getId();
                size_t pop_queue_id = task->environment.getSizeT( "system.pop_queue_id" , 0 );

                PopQueue*pq = popQueues.findInMap( pop_queue_id );
                
                if( pq )
                    pq->notifyFinishTask( task_id , &task->error );
                else
                    LM_W(("Received a finish task message for pop queue %lu for an unknown taskl %lu", pop_queue_id,  task_id ));
                
            }
        }
        
        void StreamManager::addPopQueue(const network::PopQueue& pq , size_t delilah_id, size_t delilah_component )
        {
            // Create a new pop queue
            PopQueue *popQueue = new PopQueue( pq , delilah_id , delilah_component );

            // Get a new id
            popQueue->id = id_pop_queue++;
            
            // Insert in the list
            popQueues.insertInMap( popQueue->id , popQueue );
            
            if( pq.has_queue() )
            {
                
                std::string queue_name = pq.queue();
                Queue* queue = getQueue( queue_name );
                
                // Create a pop operation for each element in the blocks
                au::list< Block >::iterator b;
                for ( b = queue->list->blocks.begin() ; b != queue->list->blocks.end() ; b++ )
                {
                    size_t id = queueTaskManager.getNewId();
                    
                    // Add this task id to wait until all of them are finished
                    popQueue->addTask( id );
                    
                    PopQueueTask *tmp = new PopQueueTask( id , popQueue , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                    BlockList *list = tmp->getBlockList("input_0");
                    list->add( *b );
                    
                    // Schedule tmp task into QueueTaskManager
                    queueTaskManager.add( tmp );
                }
            }
            popQueue->check();
        }
        

        // Get information for monitoring
        void StreamManager::getInfo( std::ostringstream& output)
        {
            output << "<stream_manager>\n";

            // Information about queues
            au::xml_iterate_map(output, "queues", queues);

            // StreamOutQueues
            //au::xml_iterate_map( output , "stream_out_connections" , stream_out_connections );
            
            // Tasks
            //output << "<queue_tasks>\n";
            //queueTaskManager.getInfo( output );
            //output << "</queue_tasks>\n";

            
            //Stream operation
            //au::xml_iterate_map( output , "stream_operations" , stream_operations );
            output << "</stream_manager>\n";
            
        }
        
        bool compareStreamOperation( StreamOperation *first , StreamOperation *second )
        {
            return ( first->getPriority() > second->getPriority() );
        }
        
        void StreamManager::reviewStreamOperations()
        {
            au::ExecesiveTimeAlarm alarm("StreamManager::reviewStreamOperations");

            
            while( true )
            {
                // Find the most urgent stream operation
                size_t max_priority_rank = 0;
                StreamOperation *next_stream_operation = NULL;
                
                
                // First review all stream operations
                au::map <std::string , StreamOperation>::iterator it_stream_operations;
                for (it_stream_operations = stream_operations.begin() ; it_stream_operations != stream_operations.end() ; it_stream_operations++ )
                {
                    StreamOperation *stream_operation = it_stream_operations->second;
                    
                    // Check non-valid operations
                    if( !stream_operation->isValid() )
                    {
                        stream_operation->last_review = "Operation paused";
                        continue;
                    }
                    
                    if( stream_operation->isPaused() )
                    {
                        stream_operation->last_review = "Operation paused";
                        continue;
                    }
                    
                    //  Review operation
                    stream_operation->last_review = "Pending revisions...";
                    
                    // Let's see if we are really the next stream operation to be schedule next task...
                    size_t tmp_max_priority_rank = stream_operation->getNextQueueTaskPriorityParameter();
                    
                    if( tmp_max_priority_rank > 0 )
                        stream_operation->last_review = au::str( "Pending task ( prioriry %s )" ,  au::str( tmp_max_priority_rank ).c_str() );
                    else
                        stream_operation->last_review = au::str( "No pending tasks" ,  au::str( tmp_max_priority_rank ).c_str() );
                        
                    if ( ( tmp_max_priority_rank > 0 ) && (tmp_max_priority_rank > max_priority_rank ) )
                    {
                        // we found a new stream operation that is more priority than the previous one
                        max_priority_rank = tmp_max_priority_rank;
                        next_stream_operation = stream_operation;
                    }
                    
                }
                
                double memory_usage = engine::MemoryManager::shared()->getMemoryUsage();
                if ( memory_usage >= 1.0 )
                {
                    LM_W(("Not schedulling new stream-tasks since memory usage is %s >= 100%", au::str_percentage( memory_usage ).c_str() ));
                    return;
                }
                
                
                // If it has enougth tasks, we do not schedule anything else
                if( queueTaskManager.hasEnougthTasks() )
                    return;
                
                if( !next_stream_operation )
                    return; // No more operations to schedule

                // Schedule next task ( or tasks in some particular cases... )
                next_stream_operation->scheduleNextQueueTasks( );
                
            }
            
        }
        
        
        void StreamManager::saveStateToDisk()
        {
            
            if( currently_saving )
            {
                LM_W(("Not possible to save stream manager state to disk because it is still writing the previous one"));
                return;
            }
            
            std::string fileName = SamsonSetup::shared()->streamManagerLogFileName();
            
            // Buffer to accumulate all ascii data  with state
            std::ostringstream output;

            // Header with time stamp
            output << "# BEGIN -- SamsonStreamManager log " << au::todayString() << "\n";

            // ---------------------------------------------------------------------------
            // Queue
            // ---------------------------------------------------------------------------
            
            au::map< std::string , Queue >::iterator it_queue;
            for ( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++)
            {
                Queue *queue = it_queue->second;
                output << "# Queue " << queue->name << "\n";
                
                // Properties of queue
                output << "queue_properties " << queue->name << " " << queue->environment.saveToString() << "\n";
                
                // Insert all the blocks in the queue
                au::list< Block >::iterator b;
                for ( b = queue->list->blocks.begin() ; b != queue->list->blocks.end() ; b++ )
                    output << "queue_push " << queue->name << " " << (*b)->getId() << "\n"; 
            }
            
            // ---------------------------------------------------------------------------
            // Stream Operations
            // ---------------------------------------------------------------------------
            
            au::map< std::string , StreamOperation >::iterator it_stream_operations;
            for ( it_stream_operations = stream_operations.begin() ; it_stream_operations != stream_operations.end() ; it_stream_operations++)
            {
                StreamOperation *stream_operation = it_stream_operations->second;
                output << "# Stream Operation " << stream_operation->name << "\n";
                
                // Creation command
                output << stream_operation->command << "\n";
                
                // Properties of this stream operation
                output << "stream_operation_properties " << stream_operation->name << " " << stream_operation->environment.saveToString() << "\n";
                
            }

            // ---------------------------------------------------------------------------
            // Queues Connections
            // ---------------------------------------------------------------------------

            au::map< std::string , QueueConnections >::iterator it;
            for ( it = queue_connections.begin() ; it != queue_connections.end() ; it++)
            {
                QueueConnections *connections = it->second;
                output << "# Queue connections " << connections->queue << "\n";
                
                // Creation command
                output << "add_queue_connection " << connections->queue << " " << connections->str_target_queues() << "\n";
            }
            
            
            // Foot just to make sure we finished correctly
            output << "# END -- SamsonStreamManager log " << au::todayString() << "\n";


            // Put everything in the state string
            std::string state_string = output.str();
            currently_saving = true;

            // Get a buffer with the rigth size
            engine::Buffer* buffer = engine::MemoryManager::shared()->newBuffer( "buffer to save stream state" 
                                                                                , "stream"
                                                                                , state_string.length() );
            if (!buffer )
                LM_X(1, ("No buffer to save stream state"));
            
            // Copy content of the buffer
            buffer->write( (char*) state_string.c_str() , state_string.length() );
            
            engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer  ,  fileName , getEngineId()  );
            operation->environment.set("save_stream_state" , "yes" );
            engine::DiskManager::shared()->add( operation );
            
        }

        void StreamManager::recoverStateFromDisk()        
        {
            // Recovery list for the blocks...
            BlockList recovery_list("recovery_list");
            
            // Recover all the blocks in current blocks directory
            // ------------------------------------------------------------------------------------------------
            std::string blocks_dir = SamsonSetup::shared()->blocksDirectory();
            DIR *dp;
            struct dirent *dirp;
            if((dp  = opendir( blocks_dir.c_str() )) != NULL) 
            {
                while ((dirp = readdir(dp)) != NULL) 
                {
                    std::string fileName = dirp->d_name;
                    
                    // Full path of the file
                    std::string path = au::path_from_directory( blocks_dir , dirp->d_name );
                    
                    struct ::stat info;
                    stat(path.c_str(), &info);
                    
                    if( S_ISREG(info.st_mode) )
                        if ( !recovery_list.createBlockFromFile(path) )
                            LM_W(("Problems recovering block from file %s", path.c_str() ));
                }                                
                closedir(dp);                
            }
            
            
            
            std::string fileName = SamsonSetup::shared()->streamManagerLogFileName();
            
            FILE *file = fopen( fileName.c_str() , "r" );

            if( !file )
            {
                LM_W(("Not possible to recover state for stream manager from %s", fileName.c_str()));
                LM_W(("If this is first time running SAMSON, this is ok." ));
                return;
            }
            
            char line[1024];
            while( !feof(file) )
            {
                if( fgets(line, sizeof(line)-1, file) )
                {
                    if( (strlen( line )>0) &&  (line[0] != '#') )
                    {
                        //LM_M(("Processing line '%s'", line ));
                        
                        au::CommandLine commandLine;
                        commandLine.parse( line );
                        
                        if ( commandLine.get_num_arguments() < 1 )
                            continue;
                        
                        std::string main_command = commandLine.get_argument(0);
                        
                        if ( main_command == "queue_push" )
                        {
                            // Format: queue_item name range_from range_to block-ids
                            if ( commandLine.get_num_arguments() < 3 )
                            {
                                LM_W(("Not valid queue_push command: %s" , line ));
                                continue;
                            }
                            
                            std::string queue_name = commandLine.get_argument(1);
                            size_t block_id = strtoll(  commandLine.get_argument(2).c_str() , (char **)NULL, 10);
                            
                            // Get this queue ( if necessary, create without any queue-item )
                            Queue *queue = getQueue( queue_name );
                            
                            // Set the minim block id to not overwrite previous blocks
                            BlockManager::shared()->setMinimumNextId( block_id );
                            Block* b = recovery_list.getBlock( block_id );
                            
                            if( b )
                                queue->push( b );
                            else
                                LM_W(("Not possible to push block %lu to queue %s." , block_id , queue_name.c_str() ));
                            
                        }
                        else if( main_command == "queue_properties" )
                        {
                            if ( commandLine.get_num_arguments() < 3 )
                                continue;
                            
                            std::string queue_name = commandLine.get_argument(1);
                            std::string properties = commandLine.get_argument(2);
                            
                            // Get this queue ( if necessary, create without any queue-item )
                            Queue *queue = getQueue( queue_name );
                            queue->environment.recoverFromString( properties );
                            
                        }
                        else if ( ( main_command == "add_stream_operation" ) || ( main_command == "aso" ) )
                        {
                            au::ErrorManager error;
                            StreamOperation *stream_operation = StreamOperation::newStreamOperation( this , std::string(line) , error );
                            
                            if( !stream_operation )
                                LM_W(("Error recovering stream operation '%s'.Error: %s" , line , error.getMessage().c_str() ));
                            else
                            {
                                
                                StreamOperation* previous = stream_operations.extractFromMap( stream_operation->name );
                                if( previous )
                                    delete previous;
                                stream_operations.insertInMap( stream_operation->name , stream_operation );
                           }
                            
                        }
                        else if ( main_command == "stream_operation_input" )
                        {
                            if ( commandLine.get_num_arguments() < 3 )
                                continue;
                            
                            std::string stream_operation_name = commandLine.get_argument(1);
                            std::string queue_name = commandLine.get_argument(2);
                            
                            StreamOperation *stream_operation = getStreamOperation( stream_operation_name );
                            if( stream_operation )
                                stream_operation->input_queues.push_back( queue_name );
                            else
                                LM_W(("Not possible to recover state of stream operation %s since it does not exist. Skipping..." , stream_operation_name.c_str() ));
                            
                        }
                        else if ( main_command == "stream_operation_output" )
                        {
                            if ( commandLine.get_num_arguments() < 3 )
                                continue;
                            
                            std::string stream_operation_name = commandLine.get_argument(1);
                            std::string queue_name = commandLine.get_argument(2);
                            
                            StreamOperation *stream_operation = getStreamOperation( stream_operation_name );
                            if( stream_operation )
                                stream_operation->output_queues.push_back( queue_name );
                            else
                                LM_W(("Not possible to recover state of stream operation %s since it does not exist. Skipping..." , stream_operation_name.c_str() ));
                            
                        }
                        else if ( main_command == "stream_operation_properties" )
                        {
                            if ( commandLine.get_num_arguments() < 3 )
                                continue;
                            
                            std::string stream_operation_name = commandLine.get_argument(1);
                            std::string properties = commandLine.get_argument(2);
                            
                            // Get this queue ( if necessary, create without any queue-item )
                            StreamOperation *stream_operation = getStreamOperation( stream_operation_name );
                            
                            if( stream_operation )
                                stream_operation->environment.recoverFromString( properties );
                            else
                                LM_W(("Not possible to recover state of stream operation %s since it does not exist. Skipping..." , stream_operation_name.c_str() ));
                            
                        }  
                        else if ( main_command == "add_queue_connection" )
                        {
                            if ( commandLine.get_num_arguments() < 3 )
                                continue;
                            
                            std::string queue_source = commandLine.get_argument(1);
                            for ( int i = 2 ; i < commandLine.get_num_arguments() ; i++ )
                            {
                                std::string queue_target = commandLine.get_argument(i);
                                add_queue_connection( queue_source, queue_target);
                            }
                        }
                        else
                        {
                            LM_W(("Ignoring message from stream-manager recovering: '%s'" , line ));
                        }
                        
                    }
                }
            }
            
            fclose( file );
            
        }

        void StreamManager::connect_to_queue( size_t fromId , std::string queue , bool flag_new , bool flag_remove )
        {
            LM_M(("Connecting to queue %s delilah %lu", queue.c_str() , fromId));
            
            StreamOutConnection* stream_out_connection = stream_out_connections.findInMap( fromId );
            if( !stream_out_connection)
            {
                stream_out_connection = new StreamOutConnection( this , fromId );
                stream_out_connections.insertInMap(fromId, stream_out_connection );
            }
            
            StreamOutQueue * stream_out_queue = stream_out_connection->add_queue( queue , flag_remove );
            
            // Push the current content of the queue
            if ( !flag_new )
                stream_out_queue->push( getQueue(queue)->list );
            
        }
        
        void StreamManager::disconnect_from_queue( size_t fromId , std::string queue )
        {
            StreamOutConnection* stream_out_connection = stream_out_connections.findInMap( fromId );
            if( !stream_out_connection)
            {
                stream_out_connection = new StreamOutConnection( this , fromId );
                stream_out_connections.insertInMap(fromId, stream_out_connection );
            }
            
            stream_out_connection->remove_queue( queue );
        }
        
        
        Status StreamManager::remove_stream_operation( std::string name )
        {
            // Remove a particular stream operation
            stream::StreamOperation * operation = stream_operations.extractFromMap( name );
            if( operation )
            {
                delete operation;
                return OK;
            }
            else
                return Error;
        }

        Status StreamManager::set_stream_operation_property( std::string name , std::string property, std::string value )
        {
            stream::StreamOperation * operation = stream_operations.findInMap( name );
            
            // Check if queue exist
            if( !operation  )
                return Error;
            else
            {
                operation->environment.set( property , value );
                return OK;
            }
            
        }

        Status StreamManager::unset_stream_operation_property( std::string name , std::string property )
        {
            stream::StreamOperation * operation = stream_operations.findInMap( name );
            
            // Check if queue exist
            if( !operation  )
                return Error;
            else
            {
                operation->environment.unset( property );
                return OK;
            }
        }
        
        samson::network::Collection* StreamManager::getCollection( Visualization* visualizaton )
        {
            samson::network::Collection* collection = new samson::network::Collection();
            collection->set_name("queues");
            au::map< std::string , Queue >::iterator it_queue;
            for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
            {
                std::string name = it_queue->second->name;
                if( match( visualizaton->pattern , name ) )
                   it_queue->second->fill( collection->add_record() , visualizaton );
            }
            return collection;
        }
        
        samson::network::Collection* StreamManager::getCollectionForQueueConnections( Visualization* visualizaton )
        {
            samson::network::Collection* collection = new samson::network::Collection();
            collection->set_name("queue_connections");
            au::map< std::string , QueueConnections >::iterator it;
            for( it = queue_connections.begin() ; it != queue_connections.end() ; it++ )
            {
                std::string name = it->second->queue;
                if( match( visualizaton->pattern , name ) )
                    it->second->fill( collection->add_record() , visualizaton );
            }
            return collection;
        }
        
        
        samson::network::Collection* StreamManager::getCollectionForStreamOperations(
                                                VisualitzationOptions options 
                                                ,  std::string pattern )
        {
            samson::network::Collection* collection = new samson::network::Collection();
            collection->set_name("stream_operations");
            au::map< std::string , StreamOperation >::iterator it;
            for( it = stream_operations.begin() ; it != stream_operations.end() ; it++ )
            {
                std::string name = it->second->name;
                if( match( pattern , name ) )
                    it->second->fill( collection->add_record() , options );
            }
            return collection;
            
        }
        
        
        samson::network::Collection* StreamManager::getCollectionForStreamBlock( std::string path , Visualization* visualizaton )
        {
            std::string pattern_inputs;
            std::string pattern_states;
            std::string pattern_outputs;
            
            pattern_inputs += path;
            pattern_states += path;
            pattern_outputs += path;
            
            pattern_inputs  += "in_*";
            pattern_states  += "s_*";
            pattern_outputs += "out_*";

            samson::network::Collection* collection = new samson::network::Collection();
            collection->set_name("stream_block");
            
            
            // Input queues            
            Visualization queue_visualization;
            if( visualizaton->options == rates )
                queue_visualization.options = stream_block_rates;
            else
                queue_visualization.options = stream_block;
            
            {
                queue_visualization.environment.set("type","input");
                au::map< std::string , Queue >::iterator it_queue;
                for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                {
                    std::string name = it_queue->second->name;
                    if( match( pattern_inputs , name ) )
                        it_queue->second->fill( collection->add_record() , &queue_visualization );
                }
            }
            
            // State queues            
            {
                queue_visualization.environment.set("type","state");
                au::map< std::string , Queue >::iterator it_queue;
                for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                {
                    std::string name = it_queue->second->name;
                    if( match( pattern_states , name ) )
                        it_queue->second->fill( collection->add_record() , &queue_visualization );
                }
            }
            
            // Output queues            
            {
                queue_visualization.environment.set("type","output");
                au::map< std::string , Queue >::iterator it_queue;
                for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                {
                    std::string name = it_queue->second->name;
                    if( match( pattern_outputs , name ) )
                        it_queue->second->fill( collection->add_record() , &queue_visualization );
                }
            }
            
            
            // Stream operations
            // ------------------------------------------------------------------------
            au::map< std::string , StreamOperation >::iterator it;
            for( it = stream_operations.begin() ; it != stream_operations.end() ; it++ )
            {
                std::string name = it->second->name;

                if( name.substr( 0 , path.length() ) == path )
                    if( name.find(".", path.length() ) == std::string::npos )
                        it->second->fill( collection->add_record() , queue_visualization.options );
            }

            
            // Stream blocks inside.....
            // ------------------------------------------------------------------------
            std::string stream_block_pattern = path + "*.*";
            std::set<std::string> stream_blocks;
            {
                au::map< std::string , Queue >::iterator it_queue;
                for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                {
                    std::string name = it_queue->second->name;
                    if( match( stream_block_pattern , name ) )
                    {
                        size_t init = path.length();
                        size_t finish = name.find(".",path.length());
                        if( finish != std::string::npos )
                        {
                            std::string stream_block_name = name.substr( init , finish-init );
                            stream_blocks.insert(stream_block_name);

                        }
                    }
                }
            }
            
            
            std::set<std::string>::iterator it_stream_blocks;
            for( it_stream_blocks = stream_blocks.begin() ; it_stream_blocks != stream_blocks.end() ; it_stream_blocks++ )
            {
                network::CollectionRecord * record = collection->add_record();
                
                std::string stream_name = *it_stream_blocks;
                
                std::string pattern_in = path + stream_name    + ".in_*";
                std::string pattern_state = path + stream_name + ".s_*";
                std::string pattern_out = path + stream_name   + ".out_*";

                
                
                if( visualizaton->options == rates )
                {
                    size_t in_kvs=0;
                    size_t in_size=0;
                    size_t state_kvs=0;
                    size_t state_size=0;
                    size_t out_kvs=0;
                    size_t out_size=0;

                    au::map< std::string , Queue >::iterator it_queue;
                    for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                    {
                        Queue *queue = it_queue->second;
                        std::string name = queue->name;
                        
                        if( match( pattern_in , name ) )
                        {
                            in_kvs += queue->rate.get_rate_kvs();
                            in_size += queue->rate.get_rate_size();
                        }
                        
                        if( match( pattern_state , name ) )
                        {
                            state_kvs += queue->rate.get_rate_kvs();
                            state_size += queue->rate.get_rate_size();
                        }
                        
                        if( match( pattern_out , name ) )
                        {
                            out_kvs += queue->rate.get_rate_kvs();
                            out_size += queue->rate.get_rate_size();
                        }
                    }
                    
                    ::samson::add( record , "type" , "block" , "left,different" );
                    ::samson::add( record , "name" , stream_name , "left,different" );
                    ::samson::add( record , "In: #kvs/s" , in_kvs , "f=uint64,sum" );
                    ::samson::add( record , "In: size/s" , in_size , "f=uint64,sum" );
                    ::samson::add( record , "State: #kvs/s" , state_kvs , "f=uint64,sum" );
                    ::samson::add( record , "State: size/s" , state_size , "f=uint64,sum" );
                    ::samson::add( record , "Out: #kvs/s" , out_kvs , "f=uint64,sum" );
                    ::samson::add( record , "Out: size/s" , out_size , "f=uint64,sum" );
                }
                else
                {
                    size_t in_kvs=0;
                    size_t in_size=0;
                    size_t state_kvs=0;
                    size_t state_size=0;
                    size_t out_kvs=0;
                    size_t out_size=0;
                    
                    au::map< std::string , Queue >::iterator it_queue;
                    for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                    {
                        Queue *queue = it_queue->second;
                        std::string name = queue->name;
                        
                        if( match( pattern_in , name ) )
                        {
                            BlockInfo blockInfo;
                            queue->update( blockInfo );
                            in_kvs += blockInfo.info.kvs;
                            in_size += blockInfo.info.size;
                        }
                        
                        if( match( pattern_state , name ) )
                        {
                            BlockInfo blockInfo;
                            queue->update( blockInfo );
                            state_kvs += blockInfo.info.kvs;
                            state_size += blockInfo.info.size;
                        }
                        
                        if( match( pattern_out , name ) )
                        {
                            BlockInfo blockInfo;
                            queue->update( blockInfo );
                            out_kvs += blockInfo.info.kvs;
                            out_size += blockInfo.info.size;
                        }
                    }
                    ::samson::add( record , "type" , "block" , "left,different" );
                    ::samson::add( record , "name" , stream_name , "left,different" );
                    ::samson::add( record , "In: #kvs" , in_kvs , "f=uint64,sum" );
                    ::samson::add( record , "In: size" , in_size , "f=uint64,sum" );
                    ::samson::add( record , "State: #kvs" , state_kvs , "f=uint64,sum" );
                    ::samson::add( record , "State: size" , state_size , "f=uint64,sum" );
                    ::samson::add( record , "Out: #kvs" , out_kvs , "f=uint64,sum" );
                    ::samson::add( record , "Out: size" , out_size , "f=uint64,sum" );
                }                    
                        
                
                
            }
                
                
            // Internal states visualitzation....
            std::string internal_state_pattern = path + "*.s_*";
            {
                queue_visualization.environment.set("type","internal_state");
                au::map< std::string , Queue >::iterator it_queue;
                for( it_queue = queues.begin() ; it_queue != queues.end() ; it_queue++ )
                {
                    std::string name = it_queue->second->name;
                    if( match( internal_state_pattern , name ) )
                        it_queue->second->fill( collection->add_record() , &queue_visualization );
                }
            }
            
            return collection;
        }
                             
        

        
        void StreamManager::reset()
        {
            queues.clearMap();
            popQueues.clearMap();
            queueTaskManager.reset();

            // Remove automatic stream operations...
            stream_operations.clearMap();

            // Remove queue connections
            queue_connections.clearMap();
            
            // Pending study about how to do this..
            //workerCommands.clearMap();
        }

        void StreamManager::add_queue_connection( std::string source_queue , std::string target_queue )
        {
            // Create the queue if not previously created
            getQueue( source_queue );
            getQueue( target_queue );
            
            QueueConnections *connections = queue_connections.findOrCreate(source_queue);
            connections->queue = source_queue;
            connections->add_connection( target_queue );
            
            // Review if there is data waiting for you
            review_queue( source_queue );
        }
        
        void StreamManager::remove_queue_connection( std::string source_queue , std::string target_queue )
        {
            QueueConnections * connections = queue_connections.findInMap( source_queue );
            if( ! connections )
                return;
            
            connections->remove_connection( target_queue );
            
            if( !connections->hasConnections() )
            {
                queue_connections.extractFromMap( source_queue );
                delete connections;
            }
        }
        
        std::string StreamManager::getState(std::string queue_name, const char* key)
        {
            LM_T(LmtRest, ("looking up key '%s' in queue '%s'", key, queue_name.c_str()));

            Queue* queue = queues.findInMap(queue_name);
            if (!queue)
                return au::xml_simple( "error" , au::str("Queue '%s' not found" , queue_name.c_str() ));
            
            // Data instances
            KVFormat format = queue->getFormat();
            if (format.isGenericKVFormat())
                return au::xml_simple( "error" , au::str("Queue '%s' is of generic format '%s'" , queue_name.c_str() , format.str().c_str() ));
            
            Data* key_data   = ModulesManager::shared()->getData( format.keyFormat );
            Data* value_data = ModulesManager::shared()->getData( format.valueFormat );

            if( !key_data || !value_data )
                return au::xml_simple( "error" , au::str("Queue '%s' has wrong format" , queue_name.c_str() ));
                
            DataInstance* reference_key_data_instance  = (DataInstance*)key_data->getInstance();

            DataInstance* key_data_instance            = (DataInstance*)key_data->getInstance();
            DataInstance* value_data_instance          = (DataInstance*)value_data->getInstance();

            // Get all the information from the reference key
            reference_key_data_instance->setFromString(key);
            
            // Get hashgroup
            int hg = reference_key_data_instance->hash(KVFILE_NUM_HASHGROUPS);
            LM_T(LmtRest, ("Hash group: %d", hg));

            // Cluster information to discover if we redirect this query
            std::vector<size_t> worker_ids    = worker->network->getWorkerIds();
            int                 server        = reference_key_data_instance->partition(  worker_ids.size() );
            size_t              worker_id     = worker_ids[server];
            size_t              my_worker_id  = worker->network->getMynodeIdentifier().id;

            if (worker_ids[server] != my_worker_id)
            {
                std::string host = worker->network->getHostForWorker( worker_id );

                LM_T(LmtRest, ("Redirect to the right server (%s)", host.c_str()));
                return au::xml_simple(  "error" 
                                      , au::str("Redirect to %s/samson/state/%s/%s" , host.c_str() , queue_name.c_str() , key )
                                      );
            }

            if ( queue->list->blocks.size() == 0 )
                return au::xml_simple(  "error" , au::str("No data in queue %s" , queue_name.c_str() ));

            // Look up this key
            BlockList list;
            list.copyFirstBlockFrom(queue->list, hg);

            if (list.blocks.size() == 0)
                return au::xml_simple(  "error" , au::str("No data in queue %s" , queue_name.c_str() ));

            Block* block = *list.blocks.begin();
            LM_T(LmtRest, ("Any faster way to get the block ?"));

            if (!block->isContentOnMemory())
            {
                LM_T(LmtRest, ("Sorry, block not in memory ..."));
                delete reference_key_data_instance;
                delete key_data_instance;
                delete value_data_instance;
                return au::xml_simple(  "error" , au::str("Block not in memory for queue %s" , queue_name.c_str() ) );
            }
            
            delete reference_key_data_instance;
            delete key_data_instance;
            delete value_data_instance;
                
            return block->lookup(key);
        }
    }
}
