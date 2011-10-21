

#include <string>           // std::string
#include <algorithm>        // std::ort
#include <sstream>          // std::stringstream
#include "Queue.h"          // samson::stream::Queue
#include "Block.h"          // samson::stream::Block

#include "au/time.h"            // au::todayString()

#include "engine/Engine.h"              // engine::Engine
#include "engine/MemoryManager.h"
#include "engine/Notification.h"

#include "samson/common/Info.h"     // samson::Info
#include "samson/common/EnvironmentOperations.h"    // copyEnviroment

#include "samson/module/ModulesManager.h"   // ModulesManager

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/BlockBreakQueueTask.h"        // samson::stream::BlockBreakQueueTask
#include "samson/stream/BlockManager.h"         // samson::BlockManager
#include "samson/stream/PopQueue.h"             // stream::PopQueue
#include "samson/stream/QueueTask.h"
#include "samson/stream/Queue.h"
#include "samson/stream/QueueTasks.h"            // samson::stream::ParserQueueTask
#include "samson/stream/BlockList.h"                // BlockList
#include "samson/stream/Queue.h"
#include "samson/stream/WorkerCommand.h"
#include "samson/stream/SystemQueueTask.h"
#include "samson/stream/PopQueueTask.h"
#include "samson/stream/StreamOperation.h"
#include "samson/stream/StreamOutput.h"

#include "StreamManager.h"                      // Own interface


namespace samson {
    namespace stream{
    
        
        
        StreamManager::StreamManager(::samson::SamsonWorker* _worker) : queueTaskManager( this )
        {
            worker = _worker;
            
            // Init counter for worker task
            worker_task_id = 1;
            
            
            // Schedule a periodic notification ( every 10 seconds )
            {
                engine::Notification *notification = new engine::Notification(notification_review_stream_manager);
                engine::Engine::shared()->notify( notification, 10 );
            }
            {
                engine::Notification *notification = new engine::Notification(notification_review_stream_manager_fast);
                engine::Engine::shared()->notify( notification, 1 );
            }

            // Recover state from log-file
            recoverStateFromDisk();
            
            id_pop_queue = 1;    // Init the id counter for pop queue operations


            // Engine listening commands
            listen( notification_review_stream_manager );
            listen( notification_review_stream_manager_fast );
            listen( notification_samson_worker_check_finish_tasks );
            listen( notification_network_diconnected );
            
            // Init the old file checking at block manager
            BlockManager::shared()->initOldFilesCheck();
            
        }

        
        int get_num_divisions( size_t size )
        {
            return 2;   // Just for testing...
        }
        
        void StreamManager::notify( engine::Notification* notification )
        {
            
            if( notification->isName( notification_network_diconnected ) )
            {
                int id = notification->environment.getInt("id",-1);
                LM_M(("Removing stream out queue for id %d", id ));
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
                au::map< int , StreamOutConnection >::iterator it;
                for( it =  stream_out_connections.begin() ; it != stream_out_connections.end() ; it++ )
                    it->second->scheduleNextTasks();    // It returns NULL when no more operations are scheduled
                
                return;
            }
                
            
            if ( notification->isName(notification_review_stream_manager) )
            {
                // Remove finished worker tasks elements
                workerCommands.removeInMapIfFinished();
                
                // Save state to disk just in case it crases
                saveStateToDisk();
                
                // Review all WorkerCommand is necessary
                au::map< size_t , WorkerCommand >::iterator it_workerCommands; 
                for( it_workerCommands = workerCommands.begin() ; it_workerCommands != workerCommands.end() ; it_workerCommands++ )
                    it_workerCommands->second->run();   // Excute if necessary

                
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
            
            
            LM_W(("Unknown notification at StreamManager"));
        }
        
        
        void StreamManager::addBlocks( std::string queue_name ,  BlockList *list )
        {
            
            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Add the blocks to the queue ( no information about any particular task at the moment )
            queue->push( list );

            // Review stream_out_queues...
            au::map< int , StreamOutConnection >::iterator it_connections;
            for( it_connections = stream_out_connections.begin() ; it_connections != stream_out_connections.end() ; it_connections++)
                it_connections->second->push( queue_name , list );
            
            // review all the automatic operations ( maybe we can only review affected operations in the future... )
            reviewStreamOperations();            
        }
     
        void StreamManager::add( StreamOperation* operation )
        {
            
            StreamOperation* previous = stream_operations.extractFromMap( operation->name );
            if( previous )
                delete previous;

            stream_operations.insertInMap( operation->name , operation );
            
            // review all the operations...
            reviewStreamOperations();
        }
        
        void StreamManager::addWorkerCommand( WorkerCommand *workerCommand )
        {
            // Set the internal pointer to stream manager
            workerCommand->setStreamManager( this );
            
            size_t id = worker_task_id++;
            workerCommands.insertInMap( id , workerCommand );
            
            // First run of this worker command
            workerCommand->run();
        }
        
        void StreamManager::remove_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
            {
                delete queue;
            }

        }
        
        
        void StreamManager::cp_queue( std::string from_queue_name , std::string to_queue_name )
        {
            Queue *fromQueue = getQueue( from_queue_name );
            Queue *toQueue = getQueue( to_queue_name );
            
            toQueue->list->copyFrom( fromQueue->list );
            
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
            return stream_operations.findInMap(name);
        }

        
        void StreamManager::notifyFinishTask( QueueTask *task )
        {
            
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
        
        void StreamManager::addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId )
        {
            // Create a new pop queue
            PopQueue *popQueue = new PopQueue( pq , delilahId, fromId );

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
        

        // Get information for monitorization
        void StreamManager::getInfo( std::ostringstream& output)
        {
            output << "<stream_manager>\n";

            // Information about queues
            au::xml_iterate_map(output, "queues", queues);

            // StreamOutQueues
            au::xml_iterate_map( output , "stream_out_connections" , stream_out_connections );
            
            // Tasks
            output << "<queue_tasks>\n";
            queueTaskManager.getInfo( output );
            output << "</queue_tasks>\n";

            // WorkerCommands
            au::xml_iterate_map(output, "worker_commands", workerCommands );
            
            
            //Stream operation
            au::xml_iterate_map( output , "stream_operations" , stream_operations );
            
            output << "</stream_manager>\n";
            
        }
        
        bool compareStreamOperation( StreamOperation *first , StreamOperation *second )
        {
            return ( first->getPriority() > second->getPriority() );
        }
        
        void StreamManager::reviewStreamOperations()
        {
            // List of operations to run in order...
            std::vector<StreamOperation*> ordered_stream_operations;
            
            // First review all stream operations
            au::map <std::string , StreamOperation>::iterator it_stream_operations;
            for (it_stream_operations = stream_operations.begin() ; it_stream_operations != stream_operations.end() ; it_stream_operations++ )
            {
                it_stream_operations->second->review();
                
                ordered_stream_operations.push_back( it_stream_operations->second );
            }

            // Sort ordered_stream_operations
            std::sort( ordered_stream_operations.begin() , ordered_stream_operations.end() , compareStreamOperation );
            

            for ( size_t i = 0 ; i < ordered_stream_operations.size() ; i++)
            {
                while( true )
                {
                    
                    // If it has enougth tasks, we do not schedule anything else
                    if( queueTaskManager.hasEnougthTasks() )
                        break;
                    
                    if( !ordered_stream_operations[i]->scheduleNextQueueTasks( ) )
                        break;
                }
                
            }
        }
        
        
        void StreamManager::saveStateToDisk()
        {
            std::string fileName = SamsonSetup::shared()->logDirectory + "/" + "log_stream_state.txt";
            std::string tmp_fileName = SamsonSetup::shared()->logDirectory + "/" + "tmp_log_stream_state.txt";
            
            std::ostringstream output;

            // Header with time stamp
            output << "# BEGIN -- SamsonStreamManager log " << au::todayString() << "\n";

            // Contents following all queues
            
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
            
            
            
            // Foot just to make sure we finished correctly
            output << "# END -- SamsonStreamManager log " << au::todayString() << "\n";

            
            FILE *file = fopen( tmp_fileName.c_str() , "w" );
            if( !file )
                LM_X(1,("Not possible to write stream log at %s" , tmp_fileName.c_str() ));

            std::string content = output.str();
            int w = fwrite( content.c_str() , content.length() + 1 , 1 , file );
            if( w != 1 )
                LM_X(1,( "Error while writing stream log at %s" , tmp_fileName.c_str() ));

            fclose( file );
            
            int r = rename( tmp_fileName.c_str() , fileName.c_str() );
            
            if( r!= 0 )
                LM_X(1,( "Error renaming log-file from stream data from %s to %s" , tmp_fileName.c_str() , fileName.c_str() ));
            
            
        }

        void StreamManager::recoverStateFromDisk()        
        {
            // Recovery list for the blocks...
            BlockList recovery_list("recovery_list");
            
            std::string fileName = SamsonSetup::shared()->logDirectory + "/" + "log_stream_state.txt";
            FILE *file = fopen( fileName.c_str() , "r" );

            if( !file )
            {
                LM_W(("Not possible to recover state for stream manager since it is not possible to open %s. If this is first time running SAMSON, this is ok." , fileName.c_str() ));
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
                            if( !b )
                                b = recovery_list.createBlockFromDisk( block_id );
                            
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
                        else if ( main_command == "add_stream_operation" )
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
                        else
                        {
                            LM_W(("Ignoring message from stream-manager recovering: '%s'" , line ));
                        }
                        
                    }
                }
            }
            
            fclose( file );
            
        }

        void StreamManager::connect_to_queue( int fromId , std::string queue )
        {
            StreamOutConnection* stream_out_connection = stream_out_connections.findInMap( fromId );
            if( !stream_out_connection)
            {
                stream_out_connection = new StreamOutConnection( this , fromId );
                stream_out_connections.insertInMap(fromId, stream_out_connection );
            }
            
            StreamOutQueue * stream_out_queue = stream_out_connection->add_queue( queue );
            
            // Push the current content of the queue ( this should be optional in the future )
            stream_out_queue->push( getQueue(queue)->list );
            
        }
        
        void StreamManager::disconnect_from_queue( int fromId , std::string queue )
        {
            StreamOutConnection* stream_out_connection = stream_out_connections.findInMap( fromId );
            if( !stream_out_connection)
            {
                stream_out_connection = new StreamOutConnection( this , fromId );
                stream_out_connections.insertInMap(fromId, stream_out_connection );
            }
            
            stream_out_connection->remove_queue( queue );
        }
        
        
        void StreamManager::reset()
        {
            queues.clearMap();
            popQueues.clearMap();
            queueTaskManager.reset();

            // Pending study about how to do this..
            //workerCommands.clearMap();

        }

        

        
    }
}
