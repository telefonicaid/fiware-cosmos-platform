

#include "logMsg/logMsg.h"                 // LM_X

#include "samson/common/coding.h"					// All data definitions
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem

#include "samson/module/ModulesManager.h"           // samson::ModulesManager

#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker

#include "WorkerTask.h"				// samson::WorkerTask
#include "WorkerTaskManager.h"		// samson::WorkerTaskManager

#include "ProcessBase.h"			// Own interface

namespace samson {
    
	
#pragma mark ProcessItemKVGenerator
	
	ProcessBase::ProcessBase( WorkerTask *task , ProcessBaseType _type ) : ProcessIsolated( task->operation, _type )
	{
		
		// copy the message received from the controller
		workerTask = new network::WorkerTask();
		workerTask->CopyFrom( task->workerTask );
		
		// Get the pointer to the network interface to send messages
		network = task->taskManager->worker->network;
		
		// Get the task_id
		task_id = task->workerTask.task_id();

		// Copy environment							
		copyEnviroment( task->workerTask.environment() , &operation_environment ); 
		
		// Some context information
		workerTaskManager = task->taskManager;
        worker      = task->taskManager->worker->network->getWorkerId();        // Get the worker id information
        num_workers = task->taskManager->worker->network->getNumWorkers();      // Get the total number of workers
        hg_set = 0; // Default hg_set
        
        // Get the outputs from the operation
        std::string operation_name = task->operation;
        Operation *op = ModulesManager::shared()->getOperation( operation_name );

        if( !op )
            setUserError( au::str( "Operation %s not found" , operation_name.c_str() ) );
        
        // Set the rigth outputs for this operation    
        addOutputsForOperation(op);
        
        //Set number of worker
        setNumWorkers( task->workerTask.servers() );
        
	}
	
	ProcessBase::~ProcessBase()
	{
        // Remove local copy of the workerTask message
        delete workerTask;
	}
	
	
    void ProcessBase::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
    {
        
        network::Queue output_queue = workerTask->output_queue( output ).queue();
        
        // Create packet for this output
        
		//LM_M(("Creating packet Message::WorkerDataExchange"));
        Packet *p = new Packet( Message::WorkerDataExchange );
        p->buffer = buffer;
        network::WorkerDataExchange *dataMessage =  p->message->mutable_data();
        
        dataMessage->set_task_id(task_id);
        dataMessage->set_txt( false );
        dataMessage->mutable_queue( )->CopyFrom( output_queue );
        dataMessage->set_worker( worker );
        dataMessage->set_hg_set( hg_set );
        dataMessage->set_finish( finish );
        
        network->sendToWorker( outputWorker , p);
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
        
        network->send( network->getMyidentifier() , p);
        
    }

	

}
