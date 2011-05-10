

#include "WorkerTask.h"			// OwnInterface
#include "WorkerTaskManager.h"			// ss::WorkerTaskManager
#include "Packet.h"						// ss::Packet
#include "WorkerSubTask.h"				// ss::WorkerSubTask
#include "BufferVector.h"
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "DataBufferProcessItem.h"		// ss::DataBufferProcessItem
#include "engine/MemoryManager.h"				// ss::MemoryRequest
#include "engine/Engine.h"						// ss::Engine
#include "engine/DiskOperation.h"				// ss::DiskOperation
#include "engine/Buffer.h"                     // engine::Buffer
#include "engine/ProcessManager.h"             // Notifications
#include "engine/DiskManager.h"                // Notifications
#include "au/Descriptors.h"         // au::Descriptors

namespace ss
{
	
	WorkerTask::WorkerTask( WorkerTaskManager *_taskManager )
	{
        num_file_output = 0;            // Init this counter to give name to the generated files
            
		taskManager = _taskManager;		// Pointer to the task manager
		reduceInformation = NULL;		// By default this is not used
		
		// Initial status of this operation
		status = pending_definition;
		
		// Init the counter to give an id to each sub tasks
		subTaskId  = 0;
		
		// Status of confirmation from all the workers
		num_workers = taskManager->worker->network->getNumWorkers();
		num_finished_workers = 0;
		
        // Init counter of pending things
        num_process_items = 0;
        num_disk_operations = 0;
		num_subtasks = 0;
        
		complete_message = NULL;
        
        
        // Add as a listener for notification_sub_task_finished
        engine::Engine::add( notification_sub_task_finished, this );
        
        engine::Engine::add( notification_process_request_response, this );
        engine::Engine::add( notification_disk_operation_request_response, this );
	}
	
	WorkerTask::~WorkerTask()
	{
		if( reduceInformation )
			delete reduceInformation;
		
		if( complete_message )
			delete complete_message;

        
        // Remove pending tasks ( if canceled before any of them finish )
        subTasks.clearMap();
        
        // Remove buffers in the output queues for killed jobs
        queueBufferVectors.clearMap();
        
        // Clear colection of output files ( delete is called for each one )
        outputFiles.clearMap(); 
        outputRemoveFiles.clearMap();
        
        // Remove myself as a listener
        engine::Engine::remove( this );
	}
	
	
	void WorkerTask::setupAndRun(Operation::Type _type , const network::WorkerTask &task)
	{
		if( status != pending_definition)
			LM_X(1,("Internal error: Unexpected status of a WorkerTask at setup" ));
		
		// Copy of the message form the controller
		workerTask = task;
        
		// Copy the type of operation
		type = _type;
		
		// Get the operation and task_id from the message
		operation = task.operation();	// Save the operation to perform		
		task_id = task.task_id();		// Save the task id
		job_id = task.job_id();		// Save the job id
		
		// Messages prepared to be send to the controller
		
		complete_message = new network::WorkerTaskConfirmation();
		complete_message->set_task_id( task_id );
		complete_message->set_type( network::WorkerTaskConfirmation::complete );
		
		// Set status to runnign until all the tasks are completed ( or error )
		status = running;
		
		switch (type) {
				
			case Operation::generator :
				if( workerTask.generator() )
				{
					GeneratorSubTask * tmp = new GeneratorSubTask( this );
					addSubTask( tmp );
				}
				break;
				
			case Operation::parser:
			{
				// An item per file
				if( workerTask.input_queue_size() != 1)
					LM_X(1,("Internal error: Trying to parse multiple inputs in one operation"));
				
				for (size_t i = 0 ; i < (size_t) workerTask.input_queue(0).file_size() ; i++)
					addSubTask( new ParserSubTask( this, workerTask.input_queue(0).file(i).name() ) );
			}
				break;
				
			case Operation::map :
			case Operation::parserOut :
			case Operation::parserOutReduce :
			case Operation::reduce :
			{
				OrganizerSubTask * tmp = new OrganizerSubTask( this );
				addSubTask( tmp );
			}
				break;
				
				
			case Operation::system:
			{
				// Spetial system operations 
				SystemSubTask *tmp = new SystemSubTask( this );
				addSubTask( tmp );
			}
				break;
				
			default:
				LM_X(1,("Internal error"));
				break;
		}
		
		
		// For those tasks with any particular task
		check();	
	}
	
	void WorkerTask::addSubTask( WorkerSubTask *subTask )
	{
        if( !subTask )
            LM_X(1,("WorkerTask tried to add a NULL subTask"));
        
		// Information about ids
        subTask->task_id = task_id;
		subTask->sub_task_id = subTaskId++;
        
		subTasks.insertInMap(subTask->sub_task_id , subTask);
        num_subtasks++;
        
        subTask->run();   // Start the subtask querying a memory request / read operation / process / etc
	}
    
    void WorkerTask::notify( engine::Notification* notification )
    {
        //LM_M(("WorkerTask: %s", notification->getDescription().c_str() ));
        
        if( notification->isName(notification_disk_operation_request_response) )
        {
            if( !notification->containsObject() )
                LM_X(1,("Error since WorkerTasks receive a notification_disk_operation_request_response without an object '%s'",notification->getDescription().c_str() ));
            else
            {
                
                engine::DiskOperation *diskOperation =  (engine::DiskOperation*) notification->extractObject();
                diskOperation->destroyBuffer();
                delete diskOperation;
                
                // Internal operations to process this finish
                num_disk_operations--;
                check();
                
            }
        }
        else if( notification->isName( notification_process_request_response ) )
        {
            num_process_items--;
            
            if ( !notification->containsObject() )
                LM_W(("WorkerTask receive a notification_process_request_response without an object"));
            else
            {
                
                DataBufferProcessItem* tmp = (DataBufferProcessItem*) notification->extractObject();
                
                // New file to be saved
                std::string queue_name = tmp->bv->queue->name();
                std::string fileName = newFileName( );
                
                // Add a file as output for this operation
                addKVFile(fileName, queue_name, tmp->buffer);
                
                delete tmp;
            }
            
        }
        else if( notification->isName(notification_sub_task_finished) )
        {
            size_t sub_task_id = notification->environment.getSizeT("sub_task_id", 0);
            WorkerSubTask *subTask = subTasks.findInMap( sub_task_id );

            
            if( subTask )
            {
                
                //LM_W(("Subtask finished %lu ( %s )" , task_id , subTask->error.str().c_str() ));

                num_subtasks--;
                
                // Copy the error if necessary
                if( subTask->error.isActivated() )
                    setError( subTask->error.getMessage() );

                // Release resources
                subTask->releaseResources();
                
                // Not delite until the end of the task ( new policy )
                // delete subTasks.extractFromMap(sub_task_id);
            }
            else
            {
                LM_W(("Subtask finished and not found in a vector of %d positions", (int)subTasks.size() ));
            }
            
            check();
        }
        else
            LM_X(1, ("WorkerTask received an unexpected notification"));
    }
	
	void WorkerTask::check()
	{
		
		/**
		 Evolution of the status
		 
		 pending_definition,			// Pending to receive message from the controller
		 running,                       // Running operation
		 local_content_finished,		// Output content is completed ( a message is send to the other workers to notify ) 
		 all_content_finish,			// The content from all the workers is received ( file are starting to be saved )
		 finish,						// All the output files are generated ( not saved ). Controller is notified about this to continue scripts
		 completed                      // Output content is saved on disk ( task can be removed from task manager )
		 */
		
		if ( status == running )
		{
			// If no more tasks, then set to finish and send a message to the rest of workers
			if( num_subtasks == 0 )
            {				
                // Send a close message to all the workers
                sendCloseMessages();
                status = local_content_finished;
                //LM_M(("Task %lu: Status changed to local content finished", task_id));
            }
		}
		
		if( status == local_content_finished )
		{
			if( num_finished_workers == num_workers )
			{
				// FLush to emit all the pending buffers to a file
				flush();
				
				// Now the status is waiting for the all the workers confirm ( me included ) finish generating data
				status = all_content_finish;	
                //LM_M(("Task %lu: Status changed to all content finished", task_id));

			}
		}
		
		
		if (status == all_content_finish )
		{
			if( num_process_items == 0 )	 // No more files to be generated
			{
				status = finish;
				
				// Send a message indicating that operation finished
				sendFinishTaskMessageToController();
                //LM_M(("Task %lu: Status changed to finished", task_id));
			}
		}
		
		
		if ( status == finish)
			if( num_disk_operations == 0 )
			{
				// If no more elements to write
				status = completed;
				
				//Send a message to the controller to notify about this
				sendCompleteTaskMessageToController();
                //LM_M(("Task %lu: Status changed to completed", task_id));
			}
		
		
		
		// Spetial condition
		if( status != completed)
			if( error.isActivated() )
			{
				status = completed;
				
				// Send complete message with error
				sendCompleteTaskMessageToController();
				
			}
        
        if( status == completed )
        {
            engine::Notification *notification = new engine::Notification( notification_task_finished );
            notification->environment.setSizeT("task_id", task_id);
            notification->environment.setInt("worker", taskManager->worker->network->getWorkerId());
            engine::Engine::add(notification);
        }
		
	}
		
	void WorkerTask::finishWorker( int worker_from )
	{
        
		num_finished_workers++;
        
        /*
        LM_M(("Finish worker message received for task %lu from worker_from %d. Now the state is %d / %d workers have finished producing content ", 
                task_id , worker_from , num_finished_workers , num_workers ));
         */
        
		check();
	}
	
	std::string WorkerTask::getStatus()
	{
		std::ostringstream output;
		output << "\t\t[ ID:" << task_id << " OP:" << operation << " State:";

		switch (status) {
			case pending_definition:
				output << "Def";
				break;
			case running:
				output << "Running";
				break;
			case finish:
				output << "Finish";
				break;
			case completed:
            {
				output << "Completed";
                if ( error.isActivated() )
                {
                    output << " with error: " << error.getMessage() << " ";
                }
            }
				break;
			case local_content_finished:
				output << "Local content finish";
				break;
			case all_content_finish:
				output << "All content finish";
				break;
		}

        if( status != completed )
            output << " Workers " << num_finished_workers << "/" << num_workers;
        
        if ( status == running )
        {
            output << " DiskOperations " << num_disk_operations;
            output << " BufferPreproces " << num_process_items;
            output << " PendingSubTasks " << num_subtasks;
        }
        
        output << " ]";
        
        
		output << "\n\t\t\t Subtasks:";

        
        au::Descriptors descriptors;
        for (au::map<size_t,WorkerSubTask>::iterator i = subTasks.begin() ; i != subTasks.end() ; i++)
            descriptors.add( i->second->getStatus() ); 

        output << descriptors.str();
		
		return output.str();
	}
	
	

	
#pragma mark Buffers processing
	
	void WorkerTask::addBuffer( network::WorkerDataExchange& workerDataExchange , engine::Buffer *buffer )
	{
        //LM_M(("Received a buffer from worker %d hash-set %d (%s)",  workerDataExchange.worker() , workerDataExchange.hg_set() , workerDataExchange.finish()?"finished":"not finished" ));
        
        network::Queue queue = workerDataExchange.queue();
        bool txt = workerDataExchange.txt();
		std::string queue_name = queue.name();
        
        if( txt )
        {
            // Add directly the append operation for each hash-group set
            int hg_set = workerDataExchange.hg_set();
            addFile( newFileNameForTXTOutput( hg_set ) , queue_name , KVInfo( buffer->getSize() , 0 ) , buffer );
        }
        else
        {
        
            QueueuBufferVector* bv = queueBufferVectors.findInMap( queue_name );
            
            if( !bv )
            {
                bv = new QueueuBufferVector(  queue , txt );
                queueBufferVectors.insertInMap( queue_name , bv  );
            }
            
            // Add the buffer to the queue vector
            bv->addBuffer( workerDataExchange, buffer );
            
            // Check if there is enougth data to produce a new file
            QueueuBufferVector* new_bv = bv->getQueueuBufferVectorToWrite();
            
            // Schedule this new buffer vector to be writted to disk
            if( new_bv )
            {
                DataBufferProcessItem* tmp = new DataBufferProcessItem( new_bv );
                addProcessItem(tmp);
            }
        }
	}
    
    void WorkerTask::addKVFile( std::string fileName , std::string queue , engine::Buffer *buffer )
    {
        KVHeader * header = (KVHeader*) ( buffer->getData() );
        addFile(fileName, queue, header->info , buffer );
    }
    
    
    void WorkerTask::addFile( std::string fileName , std::string queue , KVInfo info )
    {
        WorkerTaskOutputFile * outputFile = outputFiles.findInMap( fileName ); 
        
        if ( !outputFile )
        {
            outputFile = new WorkerTaskOutputFile( fileName , queue , taskManager->worker->network->getWorkerId() );
            outputFiles.insertInMap( fileName, outputFile );
        }

        // Append generated content for this file
        outputFile->info.append( info );
        
    }
    
	// add a buffer to be saved as a key-value file
	void WorkerTask::addFile(  std::string fileName , std::string queue , KVInfo info , engine::Buffer *buffer )
	{
        
        // Add the file to be reported to the controller
        addFile( fileName , queue , info );

		// Submit the operation
        engine::DiskOperation *operation = engine::DiskOperation::newAppendOperation(  buffer ,  SamsonSetup::dataFile( fileName ) );
        addDiskOperation(operation);
	}
	
	void WorkerTask::removeFile(  std::string fileName , std::string queue )
	{
        WorkerTaskOutputFile * outputFile = outputRemoveFiles.findInMap( fileName ); 
        
        if ( !outputFile )
        {
            outputFile = new WorkerTaskOutputFile( fileName , queue , taskManager->worker->network->getWorkerId() );
            outputRemoveFiles.insertInMap( fileName, outputFile );
        }
	}
	
	void WorkerTask::flush()
	{
		au::map<std::string , QueueuBufferVector>::iterator iter;
		
		for ( iter = queueBufferVectors.begin() ; iter != queueBufferVectors.end() ; iter++ )
        {
            DataBufferProcessItem* tmp = new DataBufferProcessItem( iter->second );
            addProcessItem(tmp);
        }
        
        // Not remove, since they have been transfered to DataBufferProcessItem
		queueBufferVectors.clear();
	}	
	
	
#pragma mark Messages

	void WorkerTask::sendCloseMessages( )
	{
		NetworkInterface *network = taskManager->worker->network;

		int num_workers = network->getNumWorkers();
		
		for (int s = 0 ; s < num_workers ; s++)
		{				
			Packet *p = new Packet( Message::WorkerDataExchangeClose );
			network::WorkerDataExchangeClose *dataMessage =  p->message->mutable_data_close();
			dataMessage->set_task_id(task_id);
			network->send(taskManager->worker, network->workerGetIdentifier(s) , p);
		}
	}	

	void WorkerTask::sendUpdateMessageToController( FullKVInfo running_info , FullKVInfo processed_info )
	{
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet( Message::WorkerTaskConfirmation );

		network::WorkerTaskConfirmation *confirmation_message = p->message->mutable_worker_task_confirmation();
        
        confirmation_message->set_task_id( task_id );
        
        confirmation_message->set_type( network::WorkerTaskConfirmation::update );
        
        network::KVInfo * process = confirmation_message->mutable_progressprocessed();
        process->set_size( processed_info.size );
        process->set_kvs( processed_info.kvs );

        network::KVInfo * running = confirmation_message->mutable_progressrunning();
        running->set_size( running_info.size );
        running->set_kvs( running_info.kvs );
        
		network->send( NULL, network->controllerGetIdentifier(), p);	
    }	
	
    
	void WorkerTask::sendFinishTaskMessageToController( )
	{		
		
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet( Message::WorkerTaskConfirmation );
		network::WorkerTaskConfirmation *finish_message = p->message->mutable_worker_task_confirmation();
        
        finish_message->set_task_id( task_id );

        // Add a file for each generated file
        for ( au::map<std::string , WorkerTaskOutputFile>::iterator f = outputFiles.begin( ) ; f!=outputFiles.end() ; f++)
            f->second->fill( finish_message->add_add_file( ) );
        
		
		if( error.isActivated() )
		{
			finish_message->set_type( network::WorkerTaskConfirmation::error );
			finish_message->set_error_message( error.getMessage() );
		}
		else
			finish_message->set_type( network::WorkerTaskConfirmation::finish );
        
		network->send( NULL, network->controllerGetIdentifier(), p);
		
	}	
	
	void WorkerTask::sendCompleteTaskMessageToController( )
	{	
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet(Message::WorkerTaskConfirmation);
		network::WorkerTaskConfirmation *confirmation = p->message->mutable_worker_task_confirmation();

		// Copy all the information from the prepared message
		confirmation->CopyFrom( *complete_message );
		
		if( error.isActivated() )
		{
            //LM_M(("Sending a WorkerTaskConfirmationError message for task %lu", task_id ));
			confirmation->set_type( network::WorkerTaskConfirmation::error );
			confirmation->set_error_message( error.getMessage() );
		}
		else
		{
            //LM_M(("Sending a WorkerTaskConfirmation message for task %lu", task_id ));
			confirmation->set_type( network::WorkerTaskConfirmation::complete );
		}
		
		network->send( NULL, network->controllerGetIdentifier(), p);
	}
	
		
#pragma mark FileName
	
	// Get a new file Name for a file

	std::string WorkerTask::newFileName( )
	{
		// Get the worker id
		int worker_id = taskManager->worker->network->getWorkerId();
        
		// Create the nme of the file
        char fileName[2000];

#ifdef __LP64__
        sprintf(fileName, "worker_%d_job_%lu_task_%lu_file_%010d", worker_id, job_id, task_id, num_file_output++);
#else
        sprintf(fileName, "worker_%d_job_%d_task_%d_file_%010d", worker_id, job_id, task_id, num_file_output++);
#endif

		return fileName;
        
	}
    
    std::string WorkerTask::newFileNameForTXTOutput( int hg_set )
    {
		// Get the worker id
		int worker_id = taskManager->worker->network->getWorkerId();

		// Create the nme of the file
        char fileName[256];

#ifdef __LP64__
        snprintf(fileName, sizeof(fileName), "worker_%d_job_%lu_task_%lu_file_txt_%010d" , worker_id , job_id, task_id , hg_set );
#else
		snprintf(fileName, sizeof(fileName), "worker_%d_job_%d_task_%d_file_txt_%010d" , worker_id , job_id, task_id , hg_set );
#endif
		return fileName;
    }
    
	
	
#pragma mark KILL	
	
	void WorkerTask::kill()
	{
        LM_W(("Kill task &l" , task_id ));
        error.set("Killed by user");
        check();
        
		/*
         
         // Revove the subtasks
         for ( au::map<size_t , WorkerSubTask>::iterator i = subTasksWaitingForMemory.begin() ; i != subTasksWaitingForMemory.end() ; i++)
         delete i->second;
         subTasksWaitingForMemory.clear();
         
         for ( au::map<size_t , WorkerSubTask>::iterator i = subTasksWaitingForReadItems.begin() ; i != subTasksWaitingForReadItems.end() ; i++)
         delete i->second;
         subTasksWaitingForReadItems.clear();

		for ( au::map<size_t , WorkerSubTask>::iterator i = subTasksWaitingForProcess.begin() ; i != subTasksWaitingForProcess.end() ; i++)
			delete i->second;
		subTasksWaitingForProcess.clear();
		
		
		
		// Remove the output buffers
		for ( au::map<std::string , QueueuBufferVector>::iterator i =  queueBufferVectors.begin() ; i != queueBufferVectors.end() ; i++ )
		{
			QueueuBufferVector* qbv = i->second;
			qbv->clear();
			delete qbv;
		}
		
		queueBufferVectors.clear();
         
         */
		
	}
    
    void WorkerTask::setError(std::string _error_message)
    {
        LM_W(("Seting error for task %lu: %s" , task_id , _error_message.c_str() ));
        
        // Set the error of the operation
        error.set( _error_message );

        // Set the flag of completed to cancel this task automatically
        status = completed;
        
        // Send the confirmation to the controller
        sendCompleteTaskMessageToController();

        // Check everything
        check();
    }
    
#pragma Notifications
    
    void WorkerTask::addDiskOperation( engine::DiskOperation *operation )
    {
        engine::Notification *notification = new engine::Notification( notification_disk_operation_request , operation );
        setNotificationCommonEnvironment( notification );
        num_disk_operations++;
        engine::Engine::add( notification );
    }

    void WorkerTask::addProcessItem( engine::ProcessItem *item )
    {
        engine::Notification *notification = new engine::Notification( notification_process_request , item );
        setNotificationCommonEnvironment( notification );
        num_process_items++;
        engine::Engine::add( notification );
    }
    
    
#pragma mark Basic notifications
    
    void WorkerTask::setNotificationCommonEnvironment( engine::Notification *notification )
    {
        notification->environment.set("target" , "WorkerTask" );
        notification->environment.setSizeT("task_id", task_id );
        notification->environment.setInt("worker",  taskManager->worker->network->getWorkerId() );
        
    }    
        
    bool WorkerTask::acceptNotification( engine::Notification* notification )
    {
        //LM_M(("WorkerTask accept? %s", notification->getDescription().c_str() ));
        
        if( notification->environment.get("target","") != "WorkerTask" )
            return false;
        if( notification->environment.getSizeT("task_id", 0) != task_id )
            return false;
        if( notification->environment.getInt("worker", -1) != taskManager->worker->network->getWorkerId() )
            return false;
        
        return  true;
    }

    

	
}
