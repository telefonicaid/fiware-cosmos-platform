

#include "ProcessBase.h"			// Own interface
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
	
	ProcessBase::ProcessBase( WorkerTask *task , ProcessBaseType _type ) 
        : ProcessIsolated( _type , task->workerTask.output_queue_size() , task->workerTask.servers() )
	{
		
		// copy the message received from the controller
		workerTask = new network::WorkerTask();
		workerTask->CopyFrom( task->workerTask );
		
		// Get the pointer to the network interface to send messages
		network = task->taskManager->worker->network;
		
		// Get the task_id
		task_id = task->workerTask.task_id();

									
		copyEnviroment( task->workerTask.environment() , &environment ); 
		
		
		workerTaskManager = task->taskManager;
        worker = task->taskManager->worker->network->getWorkerId();    // Get the worker id information
        hg_set = 0; // Default hg_set
        
	}
	
	ProcessBase::~ProcessBase()
	{
        // Remove local copy of the workerTask message
        delete workerTask;
	}
	
	void ProcessBase::runIsolated()
	{
		switch (type) {
			case key_value:
				runIsolatedKV();
				break;
			case txt:
				runIsolatedTXT();
				break;
		}
		
	}
	
	void ProcessBase::runIsolatedTXT()
	{
        // Generate TXT content using the entire buffer
		generateTXT( getTXTWriter() );
		getTXTWriter()->flushBuffer(true);
	}
	
	void ProcessBase::runIsolatedKV()
	{
		
		// Generate the key-values
		generateKeyValues( getWriter() );
		getWriter()->flushBuffer(true);
	}


	
    void ProcessBase::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
    {
        network::Queue output_queue = workerTask->output_queue( output ).queue();
        
        // Create packet for this output
        
        Packet *p = new Packet( Message::WorkerDataExchange );
        p->buffer = buffer;
        network::WorkerDataExchange *dataMessage =  p->message->mutable_data();
        
        dataMessage->set_task_id(task_id);
        dataMessage->set_txt( false );
        dataMessage->mutable_queue( )->CopyFrom( output_queue );
        dataMessage->set_worker( worker );
        dataMessage->set_hg_set( hg_set );
        dataMessage->set_finish( finish );
        
        network->send(NULL, network->workerGetIdentifier( outputWorker ) , p);
        
    }

    void ProcessBase::processOutputTXTBuffer( engine::Buffer *buffer , bool finish )
    {
        network::Queue output_queue = workerTask->output_queue( 0 ).queue();
        
        // There is only one output queue
        if( workerTask->output_queue_size() == 0)
            LM_X(1,("Internal error: WorkerTask message without output queue"));
        
        Packet *p = new Packet( Message::WorkerDataExchange );
        p->buffer = buffer;
        network::WorkerDataExchange *dataMessage =  p->message->mutable_data();
        
        dataMessage->set_task_id(task_id);
        dataMessage->mutable_queue( )->CopyFrom( output_queue );
        dataMessage->set_txt(true);
        dataMessage->set_worker( worker );
        dataMessage->set_hg_set( hg_set );
        dataMessage->set_finish( finish );
        
        network->send(NULL, network->getMyidentifier() , p);
        
    }

	

}
