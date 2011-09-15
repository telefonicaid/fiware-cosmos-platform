

#include "logMsg/logMsg.h"                 // LM_X

#include "au/CommandLine.h"				// au::CommandLine

#include "engine/Object.h"              // engine::Object
#include "engine/Notification.h"      // engine::Notification

#include "samson/common/coding.h"					// All data definitions
#include "samson/network/Packet.h"					// samson::Packet
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker
#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

#include "PopQueue.h"                   // samson::stream::PopQueue

#include "StreamProcessBase.h"			// Own interface


namespace samson {
    
    namespace stream {
        
#pragma mark ProcessItemKVGenerator
        
        StreamProcessBase::StreamProcessBase( size_t _task_id , const network::StreamOperation& _streamOperation ) 
        : ProcessIsolated( _streamOperation.operation() , ProcessIsolated::key_value , _streamOperation.output_queues_size() , _streamOperation.num_workers() )
        {
            
            // Get the task_id
            task_id = _task_id;
            
            // Set the order of the task
            task_order = 0;
            
            // Copy environemnt
            //copyEnviroment( task->workerTask.environment() , &environment ); 
            
            // Copy queue information for this task
            streamOperation = new network::StreamOperation();
            streamOperation->CopyFrom( _streamOperation );

                
        }


        
        
        StreamProcessBase::~StreamProcessBase()
        {
            if( streamOperation )
                delete streamOperation;
        }
        
        void StreamProcessBase::runIsolated()
        {
            switch (type) {
                case key_value:
                    // Generate the key-values
                    generateKeyValues( getWriter() );
                    getWriter()->flushBuffer(true);
                    break;
                case txt:
                    // Generate TXT content using the entire buffer
                    generateTXT( getTXTWriter() );
                    getTXTWriter()->flushBuffer(true);
                    break;
            }
            
        }
        
        
        void StreamProcessBase::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
        {
            //LM_M(("[%s] Processing buffer %s" , streamOperation->operation().c_str(), au::str(buffer->getSize()).c_str() ));
            
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues(output) );
            
        }
        
        void StreamProcessBase::sendBufferToQueue( engine::Buffer *buffer , int outputWorker , std::string queue_name  )
        {
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_id( 0 );
            
            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_size( buffer->getSize() );
            
            std::vector<std::string> queue_names = au::split( queue_name , ',' );
            for ( size_t i = 0 ; i < queue_names.size() ; i++)
                pb->add_queue( queue_names[i] );
            
            
            // Send the packet using the "notification_send_to_worker"
            engine::Notification *notification = new engine::Notification( notification_send_to_worker , packet );
            notification->environment.setInt("outputWorker", outputWorker );
            engine::Engine::shared()->notify( notification );
            
            
        }
        
        
        
        void StreamProcessBase::processOutputTXTBuffer( engine::Buffer *buffer , bool finish )
        {
            LM_M(("[%s] Processing buffer %s" , streamOperation->operation().c_str(), au::str(buffer->getSize()).c_str() ));

            int output = 0;
            int outputWorker = -1;  // Send always to myself ( txt is not distributed )
            
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues(output) );
            
        }
        
        
    }
}
