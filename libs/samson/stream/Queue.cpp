
#include <sstream>       

#include "QueuesManager.h"              // samson::stream::QueuesManager
#include "Block.h"                      // samson::stream::Block
#include "BlockManager.h"               // samson::stream::BlockManager

#include "engine/ProcessManager.h"      // engine::ProcessManager
#include "engine/Notification.h"        // engine::Notification

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/Info.h"                     // samson::Info

#include "samson/module/ModulesManager.h"           
#include "QueueTask.h"
#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/ParserQueueTask.h"          // samson::stream::ParserQueueTask

#include "PopQueue.h"       // samson::stream::PopQueue

#include "Queue.h"          // Own interface


namespace samson {
    namespace stream
    {
        
        
#pragma Queue
        
        Queue::Queue( std::string _name , QueuesManager * _qm ) 
        {
            name = _name;
            qm = _qm;
            streamQueue = NULL; // By default it is not assigned
            
            listen( notification_review_task_for_queue );
        }
        
        void Queue::add( int channel , Block *block )
        {
            // Retain the block for being in a queue
            stream::BlockManager::shared()->retain( block );
            
            //printf("Adding block to channel %d in queue %s" , channel , getStatus().c_str() );
            
            // Add to the matrix of blocks
            matrix.add( channel , block );
            
            //printf("After Adding block to channel %d in queue %s" , channel , getStatus().c_str() );
            
        }

        void Queue::scheduleTasksForPopQueue( PopQueue *popQueue )
        {


            BlockList blockList;

            // Copy all the blocks from the selected channel
            blockList.copyFrom( &matrix , popQueue->getChannel() );
            
            LM_M(("Processing %d blocks for pop-queue operation" , blockList.getNumBlocks() ));
            
            while( !blockList.isEmpty() )
            {
                // Creating a parserOut operation

                ParserOutQueueTask *tmp = new ParserOutQueueTask( qm->queueTaskManager.getNewId() , popQueue ); 
                tmp->getBlocks( &blockList );
                
                // Retain blocs by the task and release the one form the queue
                tmp->retain();
                
                // add the id of this task in the list of running tasks of the popQueue operation
                popQueue->running_tasks.insert( tmp->id );
                
                tmp->environment.setSizeT("pop_queue_id", popQueue->id );
                
                // Schedule tmp task into QueueTaskManager
                qm->queueTaskManager.add( tmp );
                
            }
            
        }
        
        void Queue::scheduleNewTasksIfNecessary()
        {
            if( !streamQueue )
                return;     // No information about how to process data
            
            // Get a copy of the blocks for a particular channel
            
            
            if( matrix.isEmpty() )
            {
                //LM_M(("Not run operation since no data" ));
                return;
            }

            samson::Environment environment;
            if( streamQueue )
                copyEnviroment(streamQueue->environment(), &environment);

            // Get limits to consider
            size_t max_time     = environment.getSizeT("max_time", 0);
            size_t max_size     = environment.getSizeT("max_size", 0);
            size_t max_num_operations = environment.getSizeT("max_num_operations" , 0 );             // Only for parser / map operations
            
            size_t current_time      = cronometer.diffTimeInSeconds();
            size_t current_size      = matrix.getSizeOfChannel( 0 );
            
            
            if ( ( current_size <= max_size ) && ( current_time <= max_time ) )
            {
                /*
                LM_M((" Queue %s not running since values are still under limits size: %s/%s time: %s/%s  ", 
                      name.c_str(),
                      au::Format::string( current_size ).c_str(),
                      au::Format::string( max_size ).c_str() ,
                 au::Format::time_string( current_time ).c_str(),
                 au::Format::time_string( max_time ).c_str()   )); 
                 */
                return;
            }
            
            Operation* op = samson::ModulesManager::shared()->getOperation( streamQueue->operation() );
            
            switch (op->getType()) {
                case Operation::parser:
                    
                {
                    
                    
                    // Clear all the packets accumulated outputside the main channel
                    // TODO:  matrix.clearBlocksOutputsideChannel( 0 );
                    
                    while( !matrix.isEmpty() )
                    {
                        
                        if( ( max_num_operations >0 ) && ( running_tasks.size() >= max_num_operations ) )
                        {
                            // Not running new operations since we are in the limit
                            return;
                        }
                        
                        ParserQueueTask *tmp = new ParserQueueTask( qm->queueTaskManager.getNewId() ,   streamQueue ); 
                        tmp->environment.set("queue" , name );

                        tmp->getBlocks( &matrix );

                        
                        // Reatin blocs by the task and release the one form the queue
                        tmp->retain();
                        
                        
                        // Release the elements since there are not in the queue any more
                        tmp->matrix.release();  
                        
                        // add the id of this task in the list of running tasks
                        running_tasks.insert( tmp->id );
                        
                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                    }
                    
                    // reset the cronometer for the next operation
                    cronometer.reset();
                    
                }
                    break;
                    
                case Operation::map:
                {
                    // Clear all the packets accumulated outputside the main channel
                    // TODO:  matrix.clearBlocksOutputsideChannel( 0 );

                    if( ( max_num_operations >0 ) && ( running_tasks.size() >= max_num_operations ) )
                    {
                        // Not running new operations since we are in the limit
                        return;
                    }
                    
                    while( !matrix.isEmpty() )
                    {
                        
                        MapQueueTask *tmp = new MapQueueTask( qm->queueTaskManager.getNewId() , streamQueue ); 
                        tmp->environment.set("queue" , name );

                        
                        // Extract the necessary blocks
                        tmp->getBlocks( &matrix );
                        
                        
                        // Reatin blocs by the task and release the one form the queue
                        tmp->retain();

                        
                        // Release the elements since there are not in the queue any more
                        tmp->matrix.release();  

                        
                        // add the id of this task in the list of running tasks
                        running_tasks.insert( tmp->id );

                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                    }
                    
                    // reset the cronometer for the next operation
                    cronometer.reset();
                    
                }
                    break;
                    
                case Operation::reduce:
                {
                    
                    if( matrix.isEmpty( 0 , op->getNumInputs() -1 ) )
                    {
                        //LM_M(("Not processign reduce since it is empty"));
                        return;
                    }
                    
                    if( running_tasks.size() > 0 )
                        return;
                    
                    int num_inputs = op->getNumInputs();
                    
                    BlockMatrix _matrix;               // Matrix of blocks to process income data
                    BlockMatrix _matrix_status;        // Matrix of blocs to process status
                    
                    // Extract the blocks from inputs necessary until a maximum size of 1G
                    LM_M(("Original input channel 0 : %s " , matrix.getChannel(0)->getSummary().c_str() ));
                    matrix.extract( &_matrix , 0 , num_inputs-1 , 1000000000 );
                    LM_M(("Rest of input channel 0 : %s " , matrix.getChannel(0)->getSummary().c_str() ));
                    
                    // Extract blocks of the status ( without limit of size )
                    matrix.extract( &_matrix_status , num_inputs-1, num_inputs );
                    
                    // Create all the task for this reduce based on the blocks in the state
                    int num_tasks = 20; // At the moment 20 operation for the reduce are created ( this should be adapted to the size of input/data data )
                    int hash_groups_per_task = ( KVFILE_NUM_HASHGROUPS / num_tasks );
                    for ( int i = 0 ; i < num_tasks ; i++ )
                    {
                        
                        int hg_begin = hash_groups_per_task *( i );
                        int hg_end   = hash_groups_per_task *( i+1 );
                        
                        if( i == (num_tasks-1) )
                            hg_end = KVFILE_NUM_HASHGROUPS;
                        
                        // Create the reduce operation
                        ReduceQueueTask *tmp = new ReduceQueueTask( qm->queueTaskManager.getNewId() ,  streamQueue ); 
                        tmp->environment.set("queue" , name );
                        
                        tmp->setHashGroups( hg_begin , hg_end );
                        
                        // add input blocks to the task that fits 
                        tmp->matrix.copyFrom( &_matrix , hg_begin , hg_end );
                        
                        // Add state blocks to the task that fits
                        tmp->matrix.copyFrom( &_matrix_status , hg_begin , hg_end );
                        
                        // add the id of this task in the list of running tasks ( to keep track of that is running here )
                        running_tasks.insert( tmp->id );
                        
                        // Retain blocks with the task id
                        tmp->retain();
                        
                        //LM_M(("Adding a reduce task with %lu blocks containing %s bytes" , tmp->matrix.getNumBlocks() , tmp->matrix.getInfo().str().c_str() ));
                        
                        // Schedule tmp task into QueueTaskManager
                        qm->queueTaskManager.add( tmp );
                        
                    }
                    
                    
                    // Release the packets used in the process
                    _matrix.release();
                    
                    
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
            
            output << "Queue " << name << ": ";
            
            if( running_tasks.size() > 0)
                output << "( " << running_tasks.size() << " pending operations )";
            
            output << "\n";
            
            output << au::Format::indent( matrix.str() ) << "\n";
            
            if( streamQueue )
            {
                output << "\t --> Process with " << streamQueue->operation() << " to " ;
                
                for (int i = 0 ; i < streamQueue->output_size() ; i++)
                {
                    for (int j=0; j < streamQueue->output(i).target_size() ; j++)
                    {
                        output << streamQueue->output(i).target(j).queue() << ":" << streamQueue->output(i).target(j).channel();
                        if( j != (streamQueue->output(i).target_size() - 1) )
                        {
                            output << ",";
                        }
                    }
                    
                    output << " ";
                }
                
                output << "\n";
            }
            else
                output << "\t --> [ No information about how to process ] ";
            
            return output.str();
        }
        
        
        void Queue::notifyFinishTask( size_t task_id )
        {
            running_tasks.erase( task_id );
        }

        void Queue::getInfo( std::ostringstream& output)
        {
            
            output << "<queue name=\"" << name << "\">\n"; 

            output << "<name>" << name << "</name>\n";
            output << "<running_tasks>" << running_tasks.size() << "</running_tasks>\n";
            
            // Put the matrix information here
            matrix.getInfo(output);
         
            if( streamQueue )
            {
                output << "<description>";
                
                output << "Process with " << streamQueue->operation() << " to " ;
                
                for (int i = 0 ; i < streamQueue->output_size() ; i++)
                {
                    for (int j=0; j < streamQueue->output(i).target_size() ; j++)
                    {
                        output << streamQueue->output(i).target(j).queue() << ":" << streamQueue->output(i).target(j).channel();
                        if( j != (streamQueue->output(i).target_size() - 1) )
                        {
                            output << ",";
                        }
                    }
                    
                    output << " ";
                }
                
                output << "</description>\n";
            }
            else
                output << "<description>No information about how to process</description>\n";
            
            output << "</queue>\n";
            
        }
  
        
        
    }
}