

#include "samson/module/ModulesManager.h"   
#include "samson/common/SamsonSetup.h"
#include "samson/worker/SamsonWorker.h"

#include "StreamManager.h"                 
#include "QueueTask.h"
#include "QueueTasks.h"
#include "Queue.h"

#include "BlockBreakQueueTask.h"

#include "samson/stream/StreamOperation.h"      // Own interface

namespace samson {
    namespace stream{
        
        size_t next_pow_2( size_t value )
        {
            if( value < 2)
                return 1;
            
            int p = 1;
            while ( true )
            {
                if( value == pow(2.0 , p)) 
                    return pow( 2.0 , p);
                
                if( value < pow( 2.0 , p ) )
                    return pow( 2.0 , p );
                p++;
            }
            
            LM_X(1,("Internal error"));
            return 1;
            
        }        
        
        StreamOperation::StreamOperation( )
        {
            streamManager = NULL;

            // Default enviroment parameters
            environment.set("max_latency" , "60" );
            environment.set("delayed_processing" , "no" );
            environment.set("priority","0" );
            
            // By default it is active
            setActive(true);
            
            // Additional information
            num_operations = 0;
            num_blocks = 0;
            temporal_size = 0;
            info.clear();
            temporal_core_seconds = 0;
            
        
        }
        
        StreamOperation* StreamOperation::newStreamOperation( StreamManager *streamManager , std::string command , au::ErrorManager& error )
        {
            // Expected format add_stream_operation name operation input_queues... output_queues ... parameters
            
            au::CommandLine cmd;
            cmd.parse( command );
            
            if( cmd.get_num_arguments() < 3 )
            {
                error.set("Usage: add_stream_operation name operation input1 input2 ... outputs1 outputs2 output3");
                return NULL;
            }
            
            std::string name            = cmd.get_argument( 1 );
            std::string operation       = cmd.get_argument( 2 );
            
            // If the operation exist, it will be replaced by this one, so no check if the stream operation is here
            
            // Check operation
            Operation* op = ModulesManager::shared()->getOperation(operation);

            if( !op )
            {
                error.set( "Unsupported operation " + operation );
                return NULL;
            }
            
            // Check the number of input / outputs
            if( cmd.get_num_arguments() < ( 3 + op->getNumInputs() +  op->getNumOutputs() ) )
            {
                error.set( au::str("Not enougth parameters for operation %s. It has %d inputs and %d outputs" , operation.c_str() , (int) op->getNumInputs() ,  (int) op->getNumOutputs() ) );
                return NULL;
                
            }
            
            StreamOperation *stream_operation = NULL;

            switch ( op->getType() ) {
                    
                case Operation::map:
                case Operation::parser:
                case Operation::parserOut:
                    
                    stream_operation = new StreamOperationForward();
                    break;
                    
                case Operation::reduce:
                {
                    LM_TODO(("In the future, forward reduce operations will be supported"));
                    
                    if( op->getNumInputs() != 2 )
                    {
                        error.set( au::str("Only reduce operations with 2 inputs are supported at the moment. ( In the furure, reducers with 3 or more inputs will be supported.") );
                        return NULL;
                    }
                    
                    stream_operation = new StreamOperationUpdateState();
                    
                }
                    break;
                    
                case Operation::script:
                {
                    error.set( "Script operations cannot be used to process stream queues. Only parsers, maps and spetial reducers" );
                    return NULL;
                }
                    break;
                    
                default:
                {
                    error.set( "Operation type is currently not supported... coming soon!" );
                    return NULL;
                    
                }
                    break;
            }
            
            // Common things for all StreamOperations
            stream_operation->streamManager = streamManager;
            stream_operation->command =  command;
            stream_operation->name = name;
            stream_operation->operation = operation;
            stream_operation->num_workers = streamManager->worker->network->getNumWorkers();
            
            // Get the inputs and output queues
            int num_inputs  = op->getNumInputs();
            int num_outputs = op->getNumOutputs();
            
            for (int i = 0 ; i < num_inputs ; i++ )
            {
                std::string queue_name = cmd.get_argument( 3 + i );
                stream_operation->input_queues.push_back( queue_name );
            }
            
            for (int i = 0 ; i < num_outputs ; i++ )
            {
                std::string queue_name = cmd.get_argument( 3 + num_inputs + i );
                stream_operation->output_queues.push_back( queue_name );
            }            
            
            if( stream_operation->isValid() )
                return stream_operation;
            
            delete stream_operation;
            
            error.set("Not valid operation");
            return NULL;
        }

        
        
        
        void StreamOperation::setActive( bool _active )
        {
            if( _active )
                environment.set("active", "yes" );
            else
                environment.set("active", "no" );
        }
        
        bool StreamOperation::isActive()
        {
            return  (environment.get("active","no") == "yes" );
        }

        void StreamOperation::setPaused( bool _paused )
        {
            if( _paused )
                environment.set("paused", "yes" );
            else
                environment.set("paused", "no" );
        }
        
        bool StreamOperation::isPaused()
        {
            return  (environment.get("paused","no") == "yes" );
        }
        
        int StreamOperation::getPriority()
        {
            return environment.getInt( "priority" , -1 );
        }
        
        
        
        void StreamOperation::getInfo( std::ostringstream &output )
        {
            if( !isActive() )
                return;
            
            au::xml_open(output, "stream_operation");
            
            au::xml_simple(output, "name", name);
            au::xml_simple(output, "operation", operation);
            
            au::xml_open(output, "inputs");
            for ( size_t i = 0 ; i < input_queues.size() ; i++)
                output << input_queues[i] << " ";
            au::xml_close(output, "inputs");
            
            au::xml_open(output, "outputs");
            for ( size_t i = 0 ; i < output_queues.size() ; i++)
                output << output_queues[i] << " ";
            au::xml_close(output, "outputs");
            
            au::xml_simple(output, "properties", environment.getEnvironmentDescription() );
            
            au::xml_simple(output,"status" , getStatus() );

            au::xml_simple(output,"last_review" , last_review );
            
            au::xml_simple(output, "core_seconds", environment.getInt("system.core_seconds" , 0 ) );

            au::xml_simple(output, "running_tasks", running_tasks.size() );

            // Cost in core*seconds / Mb
            int cost = (double) temporal_core_seconds / ( (double) (temporal_size+1) / 1000000000.0 );
            au::xml_simple(output, "cost", cost );
            
            au::xml_close(output, "stream_operation");
        }
        

        
        void StreamOperation::add( QueueTask* task )
        {
            // Set the environment property to make sure, it is removed when finished
            task->environment.set("system.stream_operation" , name );
            
            // Add the task to the list of running task
            running_tasks.insert( task );
            
        }
        
        void StreamOperation::remove( QueueTask* task )
        {
            // Remove this task from the list of running tasks
            running_tasks.erase( task );
            
            int core_seconds = std::max( 1 , task->cronometer.getSeconds() );
            environment.appendInt("system.core_seconds" , core_seconds );
            
            // Increate the temporal 
            temporal_core_seconds += core_seconds;
            
            // Temporal updates
            BlockList *blockList = task->getBlockList("input_0");
            BlockInfo _info = blockList->getBlockInfo();
            
            num_operations++;
            num_blocks += _info.num_blocks;
            temporal_size += _info.size;
            info.append( _info.info );
        }
        

        
        Operation* StreamOperation::getOperation()
        {
            return samson::ModulesManager::shared()->getOperation( operation );
        }
        
        
#pragma mark StreamOperationForward
        
        bool StreamOperationForward::isValid()
        {
            Operation* op = getOperation();

            switch (op->getType()) 
            {
                case Operation::map:
                case Operation::parser:
                case Operation::parserOut:
                    // Valid cases...
                    break;
                    
                default:
                    // Not valid cases
                    return false;
                    break;
            }
            
            if( !op )
                return false;
            
            if( op->getNumInputs() != (int)input_queues.size() )
                return false;
            if( op->getNumOutputs() != (int)output_queues.size() )
                return false;
            
            return true;
        }
        
        void StreamOperationForward::review()
        {
            last_review = "not considered";
            if( !isActive() )
                return;
            
            // Extract data from input queue to the "input" blocklist ( no size limit... all blocks )
            Queue *input = streamManager->getQueue( input_queues[0] );
            getBlockList("input")->extractFrom( input->list , 0 );
        }
        
        bool StreamOperationForward::scheduleNextQueueTasks( )
        {
            if( !isValid() )
            {
                last_review = "Not valid operation";
                return false;
            }
            
            // Get the operation itself
            Operation *op = getOperation( );
            
            // Properties for this stream operation
            size_t max_size         = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");
            size_t min_size         = SamsonSetup::shared()->getUInt64("stream.min_operation_input_size");
            size_t max_latency      = environment.getSizeT("max_latency", 0);                                   // Max acceptable time to run an operation
            bool delayed_processing = ( environment.get("delayed_processing", "yes") == "yes" );
            
            last_review = "";
            
            if( !isActive() )
            {
                last_review = "Not active operation";
                return false;
            }
            
            if( isPaused() )
            {
                last_review = "Operation paused";
                return false;
            }
            
            if( !isValid() )
            {
                last_review = au::str("Stream operation not valid" );
                return false;
            }
                        
            // Get the input BLockList
            BlockList *input = getBlockList("input");
            
            if( input->isEmpty() )
            {
                last_review = au::str("No data in queue %s" , input_queues[0].c_str() );
                return false;
            }
            
            // Get detailed informtion about data to be processed
            BlockInfo operation_block_info = input->getBlockInfo();

            bool cancel_operation = false;

            if( delayed_processing )
                if( operation_block_info.size < min_size )
                    cancel_operation = true;
            
            
            // Check if latency is too high....
            if( cancel_operation )
                if( ( max_latency > 0 ) && ( (size_t)operation_block_info.min_time_diff() > max_latency ) )
                    cancel_operation = false;

            
            if( cancel_operation )
            {
                last_review = au::str("Queue %s has %s ( time %s ). Required %s to fire, or time > %s" , 
                                      input_queues[0].c_str(),
                                      au::str( operation_block_info.size ,"B" ).c_str() , 
                                      au::time_string( operation_block_info.min_time_diff() ).c_str(),
                                      au::str( min_size , "B" ).c_str(),
                                      au::time_string( max_latency ).c_str()
                                      );
                return false;
            }
                
            // We create a queue task here...
                
            // Get a new id for the next opertion
            size_t id = streamManager->getNewId();
            
            QueueTask *tmp = NULL;
            switch ( op->getType() ) {
                case Operation::parser:
                {
                    tmp = new ParserQueueTask( id , this ); 
                }
                    break;
                    
                case Operation::map:
                {
                    tmp = new MapQueueTask( id , this , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                }
                    break;
                    
                case Operation::parserOut:
                {
                    tmp = new ParserOutQueueTask( id , this , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                }
                    break;
                    
                default:
                    LM_X(1,("Internal error"));
                    break;
            }
            
            // Set the outputs    
            tmp->addOutputsForOperation(op);
            
            // Copy input data
            tmp->getBlockList("input_0")->extractFrom( input , max_size );
            
            // Set working size for correct monitorization of data
            tmp->setWorkingSize();
            
            // Update information about this operation
            add( tmp );
                        
            // Schedule tmp task into QueueTaskManager
            streamManager->queueTaskManager.add( tmp );
            
            // Log activity    
            streamManager->worker->logActivity( au::str("[ %s:%lu ] Processing %s from queue %s" , 
                                                        name.c_str() , 
                                                        id,
                                                        tmp->getBlockList("input_0")->strShortDescription().c_str(),
                                                        input_queues[0].c_str() 
                                                ));
            
            return true;
        }
        
        std::string StreamOperationForward::getStatus()
        {
            std::ostringstream output;
            
            // Input data
            size_t size = getBlockList("input")->getBlockInfo().size;
            if ( size > 0 )
                output << "[ Input data " << au::str( size ) << " ]";

            if( running_tasks.size() > 0 )
                output << "[ Running " << running_tasks.size() << " operations ] ";
            
            output << "[ History " << au::str( num_operations , "ops" ) << "/";
            output << au::str( temporal_size , "B" ) << "/";
            output << au::str( info.kvs , "kvs" ) << "/";
            output << au::str(getCoreSeconds(),"cs") << " ]";
            
            return output.str();
        }
        
#pragma mark StreamOperationUpdateState::
 
        StreamOperationUpdateState::StreamOperationUpdateState()
        {
            num_divisions = SamsonSetup::shared()->getInt("general.num_processess");
            updating_division = new bool[KVFILE_NUM_HASHGROUPS];
            
            for (int i = 0 ; i < num_divisions ; i++ )
                updating_division[i] =  false;  // Falg as non updating
            
        }
        
        StreamOperationUpdateState::~StreamOperationUpdateState()
        {
            delete[] updating_division;
        }
        
        bool StreamOperationUpdateState::isValid()
        {
            Operation* op = getOperation();
            
            if( op->getType() != Operation::reduce )
                return false;
            
            if( op->getNumInputs() != 2 )
                return false;
            
            // Check input output format for state and same name for the queue
            // Check state format is coherent
            KVFormat a = op->getInputFormats()[op->getNumInputs() - 1];
            KVFormat b = op->getOutputFormats()[ op->getNumOutputs() - 1 ];
            
            if( !a.isEqual(b)  )
            {
                //error.set("Last input and output should be the same data type to qualify as stream-reduce");
                return false;                        
            }
            
            // Check that the last input and the last output are indeed the same queue
            std::string last_input = input_queues[ input_queues.size()-1 ];
            std::string last_output = output_queues[ output_queues.size()-1];
            
            if(  last_input !=  last_output )
            {
                //error.set( au::str("Last input and last output should be the same state. ( %s != %s)" , last_input.c_str() , last_output.c_str() ) ); 
                return false;
            }
            
            if( !op )
                return false;
            
            if( op->getNumInputs() != (int)input_queues.size() )
                return false;
            if( op->getNumOutputs() != (int)output_queues.size() )
                return false;
            
            return true;
        }
        
        void StreamOperationUpdateState::review()
        {
            last_review = "not considered";
            
            // Get the input queue
            Queue *input = streamManager->getQueue( input_queues[0] );
            Queue *state = streamManager->getQueue( input_queues[1] );
            
            // Increase the number of divisions if necessary
            int future_num_divisions = getMinNumDivisions();
            if( future_num_divisions > num_divisions )
            {
                if( numUpdatingDivisions() == 0)
                {
                    // Transform blocklists
                    transform_block_lists( num_divisions , future_num_divisions );
                    num_divisions = future_num_divisions;
                }
                else
                {
                    // Waiting update operations to finish...
                    
                }
            }
            
            // Set the minimum number of divisions for the state
            state->setMinNumDivisions( num_divisions );

            // Extract data from input queue to the "input" blocklist ( no size limit... all blocks )
            BlockList tmp;
            tmp.extractFrom( input->list , 0 );

            std::list<Block*>::iterator b;
            for ( b = tmp.blocks.begin() ; b != tmp.blocks.end() ; b++ )
            {
                Block* block = *b;
                
                for ( int d = 0 ; d < num_divisions ; d++ )
                    if( block->getKVRange().overlap( rangeForDivision(d, num_divisions ) ) )
                        getBlockList( au::str("input_%d", d ) )->add( block );
            }
            
        }
        
        bool StreamOperationUpdateState::scheduleNextQueueTasks(  )
        {
            if( !isValid() )
            {
                last_review = "Not valid operation";
                return false;
            }
            
            // State queue
            std::string queue_state_name = input_queues[1];
            Queue *state = streamManager->getQueue( queue_state_name );
            
            // Get the operation itself
            Operation *op = getOperation( );
            
            // Properties for this stream operation
            //size_t max_size         = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");
            size_t max_size         = SamsonSetup::shared()->getUInt64("general.memory") / 2;       // Max memory to get 
            
            size_t min_size         = SamsonSetup::shared()->getUInt64("stream.min_operation_input_size");
            size_t max_latency      = environment.getSizeT("max_latency", 0);                                   // Max acceptable time to run an operation
            bool delayed_processing = ( environment.get("delayed_processing", "yes") == "yes" );
            
            last_review = "";
            
            if( !isActive() )
            {
                last_review = "Not active operation";
                return false;
            }
            
            if( isPaused() )
            {
                last_review = "Operation paused";
                return false;
            }
            
            if( !state->isReadyForDivisions( ) )
            {
                last_review = "State queue is not ready, breaking...";
                return false;
            }
            
            if( getMinNumDivisions() > num_divisions )
            {
                last_review = "Waiting to break state...";
                return false;   // No schedule since we will break state
            }
            
            // Run a new update operation
            int division  = max_size_division();
            
            if( division == -1 )
            {
                last_review = "No more divisions to update";
                return false;
            }
            
            
            // Updating division "division"
            // --------------------------------------------------------------------------------
            if( updating_division[division] )
                LM_X(1,("Internal error"));
            
            updating_division[division] = true;
            
            // Get the input and state data 
            BlockList inputBlockList("inputBlockList");
            BlockList stateBlockList("stateBlockList");
            
            BlockList *input = getBlockList( au::str("input_%d",division) );
            inputBlockList.extractFrom( input, max_size );
            
            if( !state->getAndLockBlocksForKVRange( rangeForDivision(division , num_divisions) , & stateBlockList ) )
                LM_X(1,("Internal error"));
                        
            if( inputBlockList.isEmpty() )
            {
                LM_X(1,("Internal error for updating division  %d / %d with no content... %s " , division , num_divisions,  inputBlockList.strShortDescription().c_str() ));     // division == -1
                return false;
            }
            
            // Get detailed informtion about data to be processed
            BlockInfo operation_block_info = input->getBlockInfo();
            
            bool cancel_operation = false;
            
            if( delayed_processing )
                if( operation_block_info.size < min_size )
                    cancel_operation = true;
            
            
            // Check if latency is too high....
            if( cancel_operation )
                if( ( max_latency > 0 ) && ( (size_t)operation_block_info.min_time_diff() > max_latency ) )
                    cancel_operation = false;
            
            
            if( cancel_operation )
            {
                last_review = au::str("Division %d has %s ( time %s ). Required %s to fire, or time > %s" , 
                                      division,
                                      au::str( operation_block_info.size ,"B" ).c_str() , 
                                      au::time_string( operation_block_info.min_time_diff() ).c_str(),
                                      au::str( min_size , "B" ).c_str(),
                                      au::time_string( max_latency ).c_str()
                                      );
                return false;
            }
            
            // We create a queue task here...
            
            // Get a new id for the next operation
            size_t id = streamManager->getNewId();
            
            ReduceQueueTask * task = new ReduceQueueTask( id , this , rangeForDivision( division, num_divisions ) );
            task->addOutputsForOperation(op);
            
            // Spetial flag to indicate update_state mode ( process different output buffers )
            task->setUpdateStateDivision( division );
            
            task->getBlockList("input_0")->copyFrom( &inputBlockList );
            task->getBlockList("input_1")->copyFrom( &stateBlockList );
            
            // Set the working size to get statictics at ProcessManager
            task->setWorkingSize();
            
            // Update information about this operation
            add( task );
            
            // Schedule tmp task into QueueTaskManager
            streamManager->queueTaskManager.add( task );
            
            return true;
            
        }

        int StreamOperationUpdateState::numUpdatingDivisions()
        {
            int total = 0 ;
            for ( int i = 0 ; i < num_divisions ; i++ )
                if( updating_division[i] )
                    total++;
            return total;
        }
        
        int StreamOperationUpdateState::max_size_division()
        {
            // State queue to check if the state blocks can be locked
            std::string queue_state_name = input_queues[1];
            Queue *state = streamManager->getQueue( queue_state_name );
            
            size_t max_size = 0;
            int division = -1;
            
            for (int i = 0 ; i < num_divisions ; i++ )
            {
                if( !updating_division[i] )
                {
                    
                    if( state->canLockBlocksForKVRange( rangeForDivision(i, num_divisions) ) )
                    {
                        
                        size_t size = getBlockList( au::str("input_%d", i ) )->getBlockInfo().size;
                        
                        if( size > 0 )
                            if( ( division == -1 ) || (size > max_size ) )
                            {
                                max_size = size;
                                division = i;
                            }
                    }
                }
            }
            return division;
        }
        
        int StreamOperationUpdateState::getMinNumDivisions()
        {
            // Get the state queue & information about content
            Queue *state = streamManager->getQueue( input_queues[1] );
            BlockInfo block_info = state->list->getBlockInfo();
            
            double _min_num_divisions = (double)block_info.size / (double) SamsonSetup::shared()->getUInt64("stream.max_state_division_size");
            int min_num_divisions = next_pow_2( (size_t) _min_num_divisions ); 
            
            int num_divisions_base = SamsonSetup::shared()->getInt("general.num_processess");
            
            // Minimum the number of cores and then 4*num_cores...
            while( num_divisions_base < min_num_divisions )
                num_divisions_base *= 4;
            
            return num_divisions_base;
        }
        
        std::string StreamOperationUpdateState::getStatus()
        {
            std::ostringstream output;
            
            int num_Updating_Divisions = numUpdatingDivisions();
            
            BlockList allBlocks;
            getAllInputBlocks( &allBlocks );
            BlockInfo allBlocksInfo = allBlocks.getBlockInfo();

            if( allBlocksInfo.size > 0)
                output << "[ Input data " << au::str(  allBlocksInfo.size , "B" ) << " ]";
            
            if( num_Updating_Divisions > 0 )
                output << "[ Updating " << num_Updating_Divisions << " / " << num_divisions << " divisions ]"; 
            else
                output << "[ #Divs " << num_divisions << " ]";
            
            if( running_tasks.size() > 0 )
                output << "[ Running " << running_tasks.size() << " operations ] ";
            
            output << "[ History " << au::str( num_operations , "ops" ) << "/";
            output << au::str( temporal_size , "B" ) << "/";
            output << au::str( info.kvs , "kvs" ) << "/";
            output << au::str(getCoreSeconds(),"cs") << " ]";
            
            return output.str();
        }        
        
        void StreamOperationUpdateState::getAllInputBlocks( BlockList *blockList )
        {
            std::set<Block*> blocks;
            
            for (int i = 0 ; i < num_divisions ; i++ )
            {
                BlockList *inputBlockList = getBlockList( au::str("input_%d", i ) );

                au::list< Block >::iterator b;
                for (b = inputBlockList->blocks.begin() ;  b != inputBlockList->blocks.end() ; b++)
                {
                    Block *block = *b;
                    blocks.insert( block );
                }
            }
            
            // Add all the blocks
            for ( std::set<Block*>::iterator it = blocks.begin() ; it != blocks.end() ; it++ )
                blockList->add( *it );
            
        }
        
    }
}
