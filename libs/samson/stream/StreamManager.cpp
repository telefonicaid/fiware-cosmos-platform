

#include <string>           // std::string
#include <sstream>          // std::stringstream
#include "Queue.h"          // samson::stream::Queue
#include "Block.h"          // samson::stream::Block

#include "au/time.h"            // au::todayString()

#include "engine/Engine.h"              // engine::Engine
#include "engine/MemoryManager.h"
#include "engine/Notification.h"

#include "samson/common/Info.h"     // samson::Info

#include "samson/module/ModulesManager.h"   // ModulesManager

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/BlockManager.h"         // samson::BlockManager
#include "samson/stream/PopQueue.h"                 // stream::PopQueue
#include "samson/stream/QueueTask.h"
#include "samson/stream/Queue.h"
#include "samson/stream/QueueItem.h"
#include "samson/stream/ParserQueueTask.h"            // samson::stream::ParserQueueTask
#include "samson/stream/BlockList.h"                // BlockList
#include "samson/stream/Queue.h"
#include "samson/stream/WorkerTask.h"

#include "StreamManager.h"                      // Own interface


namespace samson {
    namespace stream{
    
        StreamManager::StreamManager(::samson::SamsonWorker* _worker) : queueTaskManager( this )
        {
            worker = _worker;
            operation_list = NULL;
            
            // Init counter for worker task
            worker_task_id = 1;
            
            listen( notification_review_stream_manager );
            
            // Schedule a periodic notification ( every 10 seconds )
            {
                engine::Notification *notification = new engine::Notification(notification_review_stream_manager);
                engine::Engine::shared()->notify( notification, 10 );
            }

            // Recover state from log-file
            recoverStateFromDisk();
            
        }

        void StreamManager::notify( engine::Notification* notification )
        {
            if ( notification->isName(notification_review_stream_manager) )
            {
                // Remove finished worker tasks elements
                workerCommands.removeInMapIfFinished();
                
                // Save state to disk just in case it crases
                saveStateToDisk();
                return;
            }
            
            LM_W(("Unknown notification at StreamManager"));
        }
        
        
        void StreamManager::addBlocks( std::string queue_name ,  BlockList *bl )
        {
            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Add the blocks to the queue ( no information about any particular task at the moment )
            queue->push( bl );

            // review all the automatic operations ( maybe we can only review affected operations in the future... )
            reviewStreamOperations();            
        }
     
        void StreamManager::setOperationList( network::StreamOperationList *list )
        {
            if( operation_list) 
                delete operation_list;
            
            operation_list = new network::StreamOperationList();
            operation_list->CopyFrom( *list );
            
            
            // review all the operations...
            reviewStreamOperations();
        }
        
        void StreamManager::addWorkerCommand( WorkerCommand *workerCommand )
        {
            workerCommand->setStreamManager( this );
            
            size_t id = worker_task_id++;
            workerCommands.insertInMap( id , workerCommand );
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
        
        void StreamManager::pause_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
                queue->paused = true;
            
        }
        
        void StreamManager::play_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
                queue->paused = false;
        }
        
        void StreamManager::cp_queue( std::string from_queue_name , std::string to_queue_name )
        {
            Queue *fromQueue = getQueue( from_queue_name );
            Queue *toQueue = getQueue( to_queue_name );
            
            toQueue->copyFrom( fromQueue );
            
        }
        
        Queue* StreamManager::getQueue( std::string queue_name )
        {
            Queue *queue = queues.findInMap( queue_name );
            if (! queue )
            {
                queue = new Queue( queue_name , this , 1 );
                queues.insertInMap( queue_name, queue );
            }
            
            return queue;
            
        }
        
        void StreamManager::notifyFinishTask( QueueTask *task )
        {
            
        }
        
        void StreamManager::notifyFinishTask( PopQueueTask *task )
        {
            // Recover identifiers
            size_t pop_queue_id = task->pop_queue_id;
            size_t task_id      = task->id;
            
            popQueueManager.notifyFinishTask( pop_queue_id , task_id , &task->error );
            
        }

        void StreamManager::addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId )
        {
            // Create a new pop queue
            PopQueue *popQueue = new PopQueue( pq , delilahId, fromId );
            popQueueManager.add( popQueue );

            if( pq.has_queue() )
            {
                
                std::string queue_name = pq.queue();
                Queue* q = getQueue( queue_name );

                // Iterate thougth all the queue-items
                au::list< QueueItem >::iterator item;
                for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                {
                    
                    // Create a pop operation for each element in the blocks
                    au::list< Block >::iterator b;
                    for ( b = (*item)->list->blocks.begin() ; b != (*item)->list->blocks.end() ; b++ )
                    {
                        size_t id = queueTaskManager.getNewId();
                        
                        popQueue->addTask( id );
                        
                        PopQueueTask *tmp = new PopQueueTask( id , popQueue , (*item)->range ); 
                        
                        tmp->addBlock( *b );
                        
                        // Schedule tmp task into QueueTaskManager
                        queueTaskManager.add( tmp );
                    }
                }
            }
            popQueue->check();
        }
        

        // Get information for monitorization
        void StreamManager::getInfo( std::ostringstream& output)
        {
            output << "<stream_manager>\n";
            
            output << "<queues>\n";

            au::map< std::string , Queue >::iterator q;
            for ( q = queues.begin() ; q != queues.end() ; q++ )
                q->second->getInfo(output);

            output << "</queues>\n";
            
            // Tasks
            output << "<queue_tasks>\n";
            queueTaskManager.getInfo( output );
            output << "</queue_tasks>\n";

            // WorkerCommands
            au::xml_iterate_map(output, "worker_commands", workerCommands );
            
            output << "</stream_manager>\n";
            
        }
        
        void StreamManager::reviewStreamOperations()
        {
            // Review all the stream operations to see if a new squedule is necessary
            if( operation_list )
            {
                for ( int i = 0 ; i < operation_list->operation_size() ; i++)
                {
                    const network::StreamOperation& operation = operation_list->operation(i);
                    reviewStreamOperation( operation );
                }
            }
        }
        
        
        // Auxiliar function
        std::string getCommandWorker( const network::StreamOperation& operation )
        {
            std::ostringstream output;
            output << "run_stream_operation " << operation.operation() << " ";
            
            for (int i = 0 ; i < operation.input_queues_size() ; i++ )
                output << operation.input_queues(i) << " ";

            for (int i = 0 ; i < operation.output_queues_size() ; i++ )
                output << operation.output_queues(i) << " ";
            
            output << "-clear_inputs";  // Necessary in all automatic stream task
            return output.str();
        }
        
        
        void StreamManager::reviewStreamOperation(const network::StreamOperation& operation)
        {
            // Get the operation
            Operation* op = samson::ModulesManager::shared()->getOperation( operation.operation() );
            
            // No valid operation
            if( !op )
            {
                LM_W(("StreamOperation %s failed since operation %s is not valid",operation.name().c_str(), operation.operation().c_str()));
                return;
            }

            /*
            // Get the limit information for this operation    
            samson::Environment environment;
            copyEnviroment(operation.environment(), &environment);
            
            // Get limits to consider
            size_t max_time             = environment.getSizeT("max_time", 0);
            size_t max_size             = environment.getSizeT("max_size", 0);
            */
            
            switch (op->getType()) {
                    
                case Operation::map:
                case Operation::parser:
                case Operation::parserOut:
                {
                    
                    //LM_W(("Reviwing StreamOperation parser %s " , operation.name().c_str()));
                    
                    if( operation.input_queues_size() != 1 )
                    {
                        //LM_W(("StreamOperation %s failed since has no input queue", operation.name().c_str()));
                        return;
                    }
                    
                    // Get the input queue
                    Queue *q = getQueue( operation.input_queues(0) );
                    
                    // Get information about this queue
                    BlockInfo block_info;
                    q->update( block_info );
                                        
                    // If necessary, run a worker command
                    if( block_info.num_blocks > 0 )
                    {
                        WorkerCommand* wc = new WorkerCommand( getCommandWorker( operation ) );
                        addWorkerCommand( wc );
                    }
                    
                }
                    break;
                    
                case Operation::reduce:
                {

                    if( operation.input_queues_size() != 2 )
                    {
                        LM_W(("StreamOperation %s failed since reduce has not 2 inputs", operation.name().c_str()));
                        return;
                    }
                    
                    
                    // Get "pre input queue" and input queue
                    Queue *inputQueue = getQueue( operation.input_queues(0) );
                    
                    // Get state state associated with the second input
                    Queue *stateQueue = getQueue( operation.input_queues(1) );
  
                    if( stateQueue->paused )
                    {
                        // no new tasks since state queue is paused
                        return;
                    }

                    // distribute the input queue in the same way state queue ( same items )
                    inputQueue->distributeItemsAs( stateQueue );

                    au::list< QueueItem >::iterator input_item = inputQueue->items.begin();
                    au::list< QueueItem >::iterator state_item = stateQueue->items.begin();
                    
                    for (  ; state_item != stateQueue->items.end() ; input_item++ , state_item++  )
                    {
                        QueueItem * stateItem = *state_item;
                        QueueItem * inputItem = *input_item;
                        
                        if( ( !stateItem->isWorking() )  && ( !inputItem->list->isEmpty() ) )
                        {
                            // Create a reduce task with all possible input
                            ReduceQueueTask *tmp = new ReduceQueueTask( queueTaskManager.getNewId() , operation , stateItem , stateItem->range ); 
                            tmp->addOutputsForOperation(op);
                            
                            // Setup content of this task
                            tmp->getBlocks( inputItem->list , stateItem->list );
                            
                            // Set this item to "running mode"
                            stateItem->setRunning( tmp );
                            
                            // Schedule tmp task into QueueTaskManager
                            queueTaskManager.add( tmp );
                            
                        }
                        
                    }
                    
                }
                    break;
                  
                    
                default:
                    // Not supported operation at the moment
                    break;
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
                output << "# Queue " << queue->name << " ( " << queue->items.size() << " items )\n";
                
                // Iterate thougth all the queue-items
                au::list< QueueItem >::iterator it_item;
                for ( it_item = queue->items.begin() ; it_item != queue->items.end() ; it_item++ )
                {
                    QueueItem *item = *it_item;
                    
                    std::ostringstream line;
                    line << "queue_item " << queue->name << " " << item->range.hg_begin << " " << item->range.hg_end << " ";
                    
                    // Iterate thorugh all the blocks
                    au::list< Block >::iterator b;
                    for ( b = item->list->blocks.begin() ; b != item->list->blocks.end() ; b++ )
                        line << (*b)->getId() << " ";
                    
                    output << line.str() << "\n";
                    
                }
                
                
                
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
                        
                        if ( main_command == "queue_item" )
                        {
                            // Format: queue_item name range_from range_to block-ids
                            if ( commandLine.get_num_arguments() < 5 )
                                continue;
                            
                            std::string queue_name = commandLine.get_argument(1);
                            int hg_begin = atoi( commandLine.get_argument(2).c_str() );
                            int hg_end = atoi( commandLine.get_argument(3).c_str() );
                            
                            // Get this queue ( if necessary, create without any queue-item )
                            Queue *queue = queues.findInMap( queue_name );
                            if (! queue )
                            {
                                queue = new Queue( queue_name , this , 0 ); // Note 0 queueu-items
                                queues.insertInMap( queue_name, queue );
                            }
                            
                            // Create queue-item
                            KVRange range( hg_begin , hg_end );
                            QueueItem* item = new QueueItem( queue  , range );
                            queue->items.push_back( item );
                            
                            for ( int i = 4 ; i < commandLine.get_num_arguments() ; i++ )
                            {
                                std::string block_id_txt = commandLine.get_argument(i);
                                size_t block_id = strtoll(  block_id_txt.c_str() , (char **)NULL, 10);
                                
                                BlockManager::shared()->setMinimumNextId( block_id );
                                
                                Block* b = recovery_list.getBlock( block_id );
                                
                                if( !b )
                                {                                    
                                    // Do something to fill b
                                    b = recovery_list.createBlockFromDisk( block_id );
                                }
                                
                                if( b )
                                    item->list->add( b );
                                else
                                    LM_W(("Not possible to push block %lu to queue %s [%d %d]." , block_id , queue_name.c_str() , hg_begin , hg_end));
                                
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
        
    }
}
