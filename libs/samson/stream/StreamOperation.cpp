

#include "samson/module/ModulesManager.h"   samson::ModulesManager

#include "StreamManager.h"                 
#include "QueueTask.h"

#include "samson/stream/StreamOperation.h"      // Own interface

namespace samson {
    namespace stream{
        
        
        StreamOperation::StreamOperation()
        {
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
            
            init();
        }
        
        
        void StreamOperation::init()
        {
            
            // Default latency
            environment.set("max_latency" , "60" );
            environment.set("delayed_processing" , "yes" );
            environment.set("block_break_mode" , "no" );

            // By default it is active
            setActive(true);
            
            // Additional information
            num_operations = 0;
            num_blocks = 0;
            size = 0;
            info.clear();
            
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
                environment.set("active", "yes" );
        }
        
        bool StreamOperation::isActive()
        {
            return  (environment.get("active","no") == "yes" );
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
                output << "[ " << num_operations << " ops / " << au::str( size , "B" ) << " / " << au::str( info.kvs , "kvs" ) << " ]";
            
            int core_seconds = environment.getInt("system.core_seconds" , 0 );
            if( core_seconds > 0 )
                output << "[ CoreSeconds " << core_seconds << " ] ";
            
            return output.str();
        }
        
        void StreamOperation::add( QueueTask* task )
        {
            // Set the environment property to make sure, it is removed when finished
            task->environment.set("system.stream_operation" , name );
           
            
            BlockList *blockList = task->getBlockList("input_0");
            BlockInfo _info = blockList->getBlockInfo();
            
            num_operations++;
            num_blocks += _info.num_blocks;
            size += _info.size;
            info.append( _info.info );
            
            // Add the task to the list of running task
            running_tasks.insert( task );
            
        }
        
        void StreamOperation::remove( QueueTask* task )
        {
            // Remove this task from the list of running tasks
            running_tasks.erase( task );
            
            int core_seconds = std::max( 1 , task->cronometer.getSeconds() );
            environment.appendInt("system.core_seconds" , core_seconds );
        }
    }
}