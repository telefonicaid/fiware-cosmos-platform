

#include "StreamProcessBase.h"			// Own interface
#include "samson/common/coding.h"					// All data definitions
#include "samson/network/Packet.h"					// samson::Packet
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker
#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "logMsg/logMsg.h"                 // LM_X
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

#include "engine/EngineNotification.h"      // engine::Notification...
namespace samson {
    
    namespace stream {
        
#pragma mark ProcessItemKVGenerator
        
        StreamProcessBase::StreamProcessBase( ProcessBaseType type , network::StreamQueue *_streamQueue ) : ProcessIsolated( type , _streamQueue->output_size() , _streamQueue->num_workers() )
        {
            
            // Get the task_id
            task_id = 0;
            
            // Copy environemnt
            //copyEnviroment( task->workerTask.environment() , &environment ); 
            
            // Copy queue information for this task
            streamQueue = new network::StreamQueue();
            streamQueue->CopyFrom( *_streamQueue );
            
        }
        
        StreamProcessBase::~StreamProcessBase()
        {
            delete streamQueue;
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
            
            LM_M(("Processing an output buffer of stream operation buffer_size=%s output=%d outputWorker=%d " , au::Format::string(buffer->getSize()).c_str() , output , outputWorker ));
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_id( 0 );
            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_size( buffer->getSize() );
            
            for ( int i = 0 ; i < streamQueue->output(output).queue_size() ; i++)
            {
                std::string queue_name = streamQueue->output(output).queue(i);
                network::QueueChannel *target = pb->add_target();
                target->set_queue( queue_name );
                target->set_channel(0);
            }
            
            // Send the packet using the "notification_send_to_worker"
            engine::Notification *notification = new engine::Notification( notification_send_to_worker , packet );
            notification->environment.setInt("outputWorker", outputWorker );
            engine::Engine::add( notification );
            
        }
        
        void StreamProcessBase::processOutputTXTBuffer( engine::Buffer *buffer , bool finish )
        {
            
            LM_X(1, ("Unimplemented section"));
            
            
        }
        
        
    }
}
