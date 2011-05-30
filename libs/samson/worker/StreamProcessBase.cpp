

#include "StreamProcessBase.h"			// Own interface
#include "WorkerTask.h"				// samson::WorkerTask
#include "WorkerTaskManager.h"		// samson::WorkerTaskManager
#include "samson/common/coding.h"					// All data definitions
#include "samson/network/Packet.h"					// samson::Packet
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "WorkerTask.h"				// samson::WorkerTask
#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker
#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "logMsg/logMsg.h"                 // LM_X
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

namespace samson {
    
	
#pragma mark ProcessItemKVGenerator
	
    StreamProcessBase::StreamProcessBase( ProcessBaseType type , network::StreamQueue *_streamQueue ) : ProcessIsolated( type , 0 , 0 )
	{
		

        // Get the pointer to the network interface to send messages
		network = NULL;
		
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
        
        Packet* packet = new Packet( Message::PushBlock );
        packet->buffer = buffer;    // Set the buffer of data
        packet->message->set_delilah_id( 0 );
        network::PushBlock* pb =  packet->message->mutable_push_block();
        pb->set_size( buffer->getSize() );
        
        for ( int i = 0 ; i < streamQueue->output(output).queue_size() ; i++)
        {
            std::string queue_name = streamQueue->output(output).queue(i);
            pb->add_queue( queue_name );
        }
        
        network->send(NULL, network->workerGetIdentifier(outputWorker), packet);
        
    }
    
    void StreamProcessBase::processOutputTXTBuffer( engine::Buffer *buffer , bool finish )
    {
  
        LM_X(1, ("Unimplemented section"));
  
        
    }

	

}
