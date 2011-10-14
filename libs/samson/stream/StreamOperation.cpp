

#include "samson/module/ModulesManager.h"   
#include "samson/common/SamsonSetup.h"
#include "StreamManager.h"                 
#include "QueueTask.h"

#include "samson/stream/StreamOperation.h"      // Own interface

namespace samson {
    namespace stream{
        
        
        StreamOperation::StreamOperation( StreamManager *_streamManager )
        {
            streamManager = _streamManager;
            init();
        }
        
        StreamOperation::StreamOperation( StreamOperation* streamOperation )
        {
            name = streamOperation->name;
            operation = streamOperation->operation;
            
            environment.environment.clear();
            environment.copyFrom( &streamOperation->environment );
            
            input_queues.insert( input_queues.begin(), streamOperation->input_queues.begin() , streamOperation->input_queues.end() );
            output_queues.insert( output_queues.begin(), streamOperation->output_queues.begin() , streamOperation->output_queues.end() );
            
            streamManager = streamOperation->streamManager;
            
            init();
        }
        
        
        void StreamOperation::init()
        {
            
            // Default latency
            environment.set("max_latency" , "60" );
            environment.set("delayed_processing" , "yes" );
            environment.set("block_break_mode" , "no" );
            environment.set("priority","0" );

            // By default it is active
            setActive(true);
            
            // Additional information
            num_operations = 0;
            num_blocks = 0;
            temporal_size = 0;
            info.clear();
            temporal_core_seconds = 0;
            
            update_state_counter = 0;
        }
        
        bool StreamOperation::isValid()
        {
            Operation* op = samson::ModulesManager::shared()->getOperation( operation );
            
            if( !op )
                return false;
            
            if( op->getNumInputs() != (int)input_queues.size() )
                return false;
            if( op->getNumOutputs() != (int)output_queues.size() )
                return false;
            
            return true;
            
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
        
        bool StreamOperation::isPaused()
        {
            return  (environment.get("paused","no") == "yes" );
        }
        
        void StreamOperation::setNumWorkers( int num_workers )
        {
            environment.setInt( "num_workers" , num_workers );
        }
        
        int StreamOperation::getNumWorkers()
        {
            return environment.getInt( "num_workers" , -1 );
        }
        
        
        void StreamOperation::add_update_state()
        {
            update_state_counter++;
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
            int cost = (double) temporal_core_seconds / ( (double) (temporal_size+1) / 1000000.0 );
            au::xml_simple(output, "cost", cost );
            
            au::xml_close(output, "stream_operation");
        }
        
        std::string StreamOperation::getStatus()
        {
            std::ostringstream output;
            
            if( running_tasks.size() > 0 )
                output << "[ Running " << running_tasks.size() << " operations ] ";
            
            if( update_state_counter > 0 )
                output << "[ Updates states " << update_state_counter << " ] ";
            
            if( num_operations > 0)
                output << "[ " << num_operations << " ops / " << au::str( temporal_size , "B" ) << " / " << au::str( info.kvs , "kvs" ) << " ]";
            
            int core_seconds = environment.getInt("system.core_seconds" , 0 );
            if( core_seconds > 0 )
                output << "[ CoreSeconds " << core_seconds << " ] ";
            
            return output.str();
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
        
        bool StreamOperation::ready( )
        {
            
            last_review = "";
            
            if( !isActive() )
                return false;
            
            if( isPaused() )
            {
                last_review = "Operation paused";
                return false;
            }
            
            Operation* op = ModulesManager::shared()->getOperation( operation );
            
            if( !op )
            {
                last_review = au::str("Operation %s not found" , operation.c_str() );
                return false;
            }
            
            if( !isValid() )
            {
                last_review = au::str("Stream operation not valid" );
                return false;
            }
            
            // Properties for this stream operation
            size_t min_size         = SamsonSetup::shared()->getUInt64("stream.min_operation_input_size");      // Minimum size to run an operation
            
            // Get the input queue
            Queue *input = streamManager->getQueue( input_queues[0] );
            
            
            // In reduce-update-state operations, the minimum should be different
            if( op->getType() == Operation::reduce )
            {
                Queue *state = streamManager->getQueue( input_queues[1] );
                state->setMinimumNumDivisions();
                
                // Change the minimum to fire process
                min_size = 0.25 * ((double) SamsonSetup::shared()->getUInt64("general.memory"));
                
                // Properties for this stream operation
                bool block_break_mode = ( environment.get("block_break_mode", "yes") == "yes" );
                
                if( block_break_mode )
                {
                    // Make sure, input is divided at least as state
                    if( input->num_divisions < state->num_divisions )
                        input->setNumDivisions( state->num_divisions );
                }
                else
                {
                    if( state->getNumUpdatingDivisions() != 0)
                    {
                        last_review = au::str("State is still updating some divisions" );
                        return false;
                    }
                }
                
                if( !state->isQueueReadyForStateUpdate() )
                {
                    last_review = au::str("State %s is not ready for update" , input_queues[1].c_str() );
                    return false;
                }
            }
            
            size_t max_latency      = environment.getSizeT("max_latency", 0);                                   // Max acceptable time to run an operation
            bool delayed_processing = ( environment.get("delayed_processing", "yes") == "yes" );
            
            // Get information about content of this queue
            BlockInfo operation_block_info = input->getBlockInfoForProcessing();
            
            if( operation_block_info.size == 0 )
            {
                last_review = au::str("No data in queue %s" , input_queues[0].c_str() );
                return false;       // No data to be processed
            }
            
            if( !delayed_processing )
            {
                last_review = "ready";
                return true;        // No delayed processing
            }
            
            
            // Check if latency is too high....
            if( ( max_latency > 0 ) && ( (size_t)operation_block_info.min_time_diff() > max_latency ) )
            {
                last_review = "ready";
                return true;    // Max latency acceptable
            }
            
            // If there is not enougth size, do not run the operation
            if( (min_size>0) && ( operation_block_info.size < min_size ) )
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
            
            return true;
            
        }
        
        bool StreamOperation::compare( StreamOperation *other_stream_operation )
        {
            if( !other_stream_operation )
                return true;

            
            int priority = environment.getInt("environment",0);
            int _priority = other_stream_operation->environment.getInt("environment",0);
            
            if( priority > _priority )
                return true;
            else if( priority < _priority )
                return false;
            
            int core_seconds = environment.getInt("system.core_seconds",0);
            int _core_seconds = other_stream_operation->environment.getInt("system.core_seconds",0);

            // Rigth now, let run the less demanding operation
            return (core_seconds < _core_seconds);
            
        }
        
        
    }
}
