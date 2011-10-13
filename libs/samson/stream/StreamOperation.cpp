
#include "StreamManager.h"                 
#include "QueueTask.h"
#include "samson/stream/StreamOperation.h"      // Own interface

namespace samson {
    namespace stream{
        
        
        StreamOperation::StreamOperation()
        {
            
        }
        
        StreamOperation::StreamOperation( StreamOperation* streamOperation )
        {
            name = streamOperation->name;
            operation = streamOperation->operation;
            num_workers = streamOperation->num_workers;
            
            environment.environment.clear();
            environment.copyFrom( &streamOperation->environment );
            
            input_queues.insert( input_queues.begin(), streamOperation->input_queues.begin() , streamOperation->input_queues.end() );
            output_queues.insert( output_queues.begin(), streamOperation->output_queues.begin() , streamOperation->output_queues.end() );
        }
        
        
        StreamOperation::StreamOperation( const network::StreamOperation& streamOperation)
        {
            update( streamOperation );
            
            // Additional information
            num_operations = 0;
            num_blocks = 0;
            size = 0;
            info.clear();
            
            update_state_counter = 0;
            
        }
        
        
        void StreamOperation::update( const network::StreamOperation& streamOperation )
        {
            // UPdate form controller
            name = streamOperation.name();
            operation = streamOperation.operation();
            
            input_queues.clear();
            for (int i = 0 ; i < streamOperation.input_queues_size() ; i++ )
                input_queues.push_back( streamOperation.input_queues(i) );
            
            output_queues.clear();
            for (int i = 0 ; i < streamOperation.output_queues_size() ; i++ )
                output_queues.push_back( streamOperation.output_queues(i) );
            
            
            // Something to remove
            num_workers = streamOperation.num_workers();
            
            // Copy environment
            environment.environment.clear();
            const network::Environment & _environment = streamOperation.environment();
            copyEnviroment( _environment , &environment );
            
            active = true;
        }
        
        void StreamOperation::setActive( bool _active )
        {
            active = _active;
        }
        
        
        void StreamOperation::add_update_state()
        {
            update_state_counter++;
        }
        
        void StreamOperation::getInfo( std::ostringstream &output )
        {
            if( !active )
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
                output << "[ " << num_operations << " ops " << au::str( size , "B" ) << " ( " << au::str( info.kvs , "kvs" ) << " )";
            
            return output.str();
        }
        
        void StreamOperation::add( QueueTask* task )
        {
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
            
        }
    }
}