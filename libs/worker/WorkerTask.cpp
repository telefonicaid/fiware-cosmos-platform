

#include "WorkerTask.h"			// OwnInterface
#include "WorkerTaskManager.h"			// ss::WorkerTaskManager
#include "ProcessManager.h"				// ss::ProcessManager
#include "Packet.h"						// ss::Packet
#include "ProcessManager.h"				// ss::ProcessManager
#include "WorkerSubTask.h"				// ss::WorkerSubTask
#include "FileManagerReadItem.h"		// ss::FileManagerReadItem
#include "BufferVector.h"
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "FileManager.h"				// ss::FileManager
#include "DataBufferProcessItem.h"		// ss::DataBufferProcessItem
#include "FileManagerReadItem.h"		// ss::FileManagerReadItem
#include "FileManagerWriteItem.h"		// ss::FileManagerWriteItem
#include "MemoryManager.h"				// ss::MemoryRequest

namespace ss
{
	
	WorkerTask::WorkerTask( WorkerTaskManager *_taskManager )
	{
		taskManager = _taskManager;		// Pointer to the task manager
		reduceInformation = NULL;		// By default this is not used
		
		// Initial status of this operation
		status = pending_definition;
		
		// Init the counter to give an id to each sub tasks
		subTaskId  = 0;
		
		// Status of confirmation from all the workers
		num_workers = taskManager->worker->network->getNumWorkers();
		num_finished_workers = 0;
		
		
		finish_message = NULL;
		complete_message = NULL;
	}
	
	WorkerTask::~WorkerTask()
	{
		if( reduceInformation )
			delete reduceInformation;
		
		if( finish_message )
			delete finish_message;
		
		if( complete_message )
			delete complete_message;
	}
	
	
	void WorkerTask::setup(Operation::Type _type , const network::WorkerTask &task)
	{
		assert( status == pending_definition);
		
		// Copy of the message form the controller
		workerTask = task;

		// Copy the type of operation
		type = _type;
		
		// Get the operation and task_id from the message
		operation = task.operation();	// Save the operation to perform		
		task_id = task.task_id();		// Save the task id
		
		// Messages prepared to be send to the controller
		finish_message = new network::WorkerTaskConfirmation();
		finish_message->set_task_id( task_id );
		finish_message->set_type( network::WorkerTaskConfirmation::finish );
		
		complete_message = new network::WorkerTaskConfirmation();
		complete_message->set_task_id( task_id );
		complete_message->set_type( network::WorkerTaskConfirmation::complete );
		
		
		status = ready;
		
		
	}

	void WorkerTask::run()
	{
		assert( status == ready );
		
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
				assert( workerTask.input_queue_size() == 1);	// Only one input
				
				for (size_t i = 0 ; i < (size_t) workerTask.input_queue(0).file_size() ; i++)
					addSubTask( new ParserSubTask( this, workerTask.input_queue(0).file(i).name() ) );
			}
				break;
				
			case Operation::map :
			case Operation::parserOut :
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
				assert( false ); // Operation not supported
				break;
		}
		
		
		// For those tasks with any particular task
		check();	
	}
	
	void WorkerTask::addSubTask( WorkerSubTask *subTask )
	{
		// Give the task an id
		subTask->id = subTaskId++;
		
		if( error.isActivated() )
		{
			delete subTask;
			return;
		}
		
		// Only mode to add new tasks
		assert( status == running);
		
		MemoryRequest *mr = subTask->getMemoryRequest();
		if( mr )
		{
			subTasksWaitingForMemory.insertInMap( subTask->id , subTask );
			MemoryManager::shared()->addMemoryRequest( mr );
			return;
		}
		
		// If no memory request go to the inputs
		std::vector<FileManagerReadItem*>* items = subTask->getFileMangerReadItems();
		if( items )
		{
		    assert( items->size() > 0);  // Otherwise return NULL

			subTasksWaitingForReadItems.insertInMap( subTask->id , subTask );

			for ( size_t i = 0 ; i < items->size() ; i++ )
				FileManager::shared()->addItemToRead( items->at(i) );

			delete items;
			
			// Submit all the items to be read...
			return;
		}

		// If not take the process to run...
		ProcessItem* processItem =  subTask->getProcessItem();
		
		if ( processItem )
		{
			// Add to the list of waiting sub-tasks
			subTasksWaitingForProcess.insertInMap( subTask->id , subTask );

			// Schedule the proces item
			ProcessManager::shared()->addProcessItem( processItem );
			
			return;
		}
		
		// Nothing to do
		delete subTask;
		check();
	}
	
	
	void WorkerTask::check()
	{
		
/**
 Evoluction of the status
 
	pending_definition,			// Pending to receive message from the controller
	running,					// Running operation
	local_content_finished,		// Output content is completed ( a message is send to the other workers to notify ) 
	all_content_finish,			// The content from all the workers is received ( file are starting to be saved )
	finish,						// All the output files are generated ( not saved ). Controller is notified about this to continue scripts
	completed					// Output content is saved on disk ( task can be removed from task manager )
 */
		
		if ( status == running )
		{
			// If no more tasks, then set to finish and send a message to the rest of workers
			if( subTasksWaitingForMemory.size() == 0 )
				if( subTasksWaitingForReadItems.size() == 0 )
					if( subTasksWaitingForProcess.size() == 0 )
				    {				
						// Send a close message to all the workers
						sendCloseMessages();
						
						status = local_content_finished;
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
			}
		}

		
		if (status == all_content_finish )
		{
			if( processWriteItems.size() == 0 )	 // No more files to be generated
			{
				status = finish;
				
				// Send a message indicating that operation finished
				sendFinishTaskMessageToController();
			}
		}
		
		
		if ( status == finish)
			if( ( writeItems.size() == 0 ) && ( processWriteItems.size() == 0 ) )
			{
				// If no more elements to write
				
				status = completed;
				
				//Send a message to the controller to notify about this
				sendCompleteTaskMessageToController();
			}
		
		
		
		// Spetial confition if
		if( status != completed)
			if( error.isActivated() )
			{
				status = completed;
				
				// Send complete message with error
				sendCompleteTaskMessageToController();
				
			}
		
		
	}
	
	void WorkerTask::notifyFinishProcess( ProcessItem * i )
	{
		
		switch (i->component) {
			case WORKER_TASK_COMPONENT_PROCESS:
			{
				
				WorkerSubTask *st = subTasksWaitingForProcess.extractFromMap( i->sub_tag );
				assert( st );
				delete st;

				// Get the possible error from the process execution
				if( i->error.isActivated() )
					setError(i->error.getMessage() );
				
				check();
				
			}
				break;
				
			case WORKER_TASK_COMPONENT_DATA_BUFFER_PROCESS:
			{
				DataBufferProcessItem* tmp = (DataBufferProcessItem*) i;
				
				processWriteItems.erase(tmp);
				
				// New file to be saved
				std::string queue_name = tmp->bv->queue->name();
				std::string fileName = newFileName( );
				
				// Notify the controller that a new file is created
				sendAddFileMessageToController( tmp->bv , fileName , tmp->buffer );
				
				FileManagerWriteItem *item = new FileManagerWriteItem( fileName , tmp->buffer , taskManager);
				item->tag = task_id;											// Tag is used with the number of the task
				item->component = WORKER_TASK_COMPONENT_DATA_BUFFER_PROCESS;	// Component is used to route the notification at the TaskManager
				writeItems.insert( item );
				
				// Schedule the new element to be saved on disk
				FileManager::shared()->addItemToWrite( item );
								
			}
				break;
			default:
				assert( false );
				break;
		}
		
		
	}
	
	void WorkerTask::notifyFinishReadItem( FileManagerReadItem *item  )
	{
		
		WorkerSubTask * subTask = subTasksWaitingForReadItems.findInMap( item->sub_tag );
		assert( subTask );

		
		if ( subTask->notifyReadFinish() )
		{
			// Extract 
			subTask = subTasksWaitingForReadItems.extractFromMap( item->sub_tag );
				
			// Once read all files go to the process

			ProcessItem* processItem =  subTask->getProcessItem();
			
			if ( processItem )
			{
				// Add to the list of waiting sub-tasks
				subTasksWaitingForProcess.insertInMap( subTask->id , subTask );
				
				// Schedule the proces item
				ProcessManager::shared()->addProcessItem( processItem );
				
				return;
			}
			else
			{
				// Remove the subtask since it will be not necessary any more
				delete subTask;
			}
			
			// Internal check for status
			check();
			
		}
		
	}
	
	void WorkerTask::notifyFinishWriteItem( FileManagerWriteItem *item  )
	{
		// Buffers have been saved to disk
		
		switch (item->component) {
			case WORKER_TASK_COMPONENT_PROCESS:
			{
				assert( false );
			}
				break;
				
			case WORKER_TASK_COMPONENT_DATA_BUFFER_PROCESS:
			{
				writeItems.erase( item );
				check();
			}
				break;
			case WORKER_TASK_COMPONENT_ADD_FILE:
			{
				writeItems.erase( item );
				check();
			}
				break;
			default:
				assert( false );
				break;
		}
		
	}
	
	void WorkerTask::notifyFinishMemoryRequest( MemoryRequest *request )
	{
		WorkerSubTask * subTask = subTasksWaitingForMemory.extractFromMap( request->sub_tag );
		assert( subTask );
		
		
		// If no memory request go to the inputs
		std::vector<FileManagerReadItem*>* items = subTask->getFileMangerReadItems();
		if( items )
		{
			subTasksWaitingForReadItems.insertInMap( subTask->id , subTask );
			
			for ( size_t i = 0 ; i < items->size() ; i++ )
				FileManager::shared()->addItemToRead( items->at(i) );
			
			delete items;
			
			return;
		}
		
		// If not take the process to run...
		ProcessItem* processItem =  subTask->getProcessItem();
		
		if ( processItem )
		{
			// Add to the list of waiting sub-tasks
			subTasksWaitingForProcess.insertInMap( subTask->id , subTask );
			
			// Schedule the proces item
			ProcessManager::shared()->addProcessItem( processItem );
			
			return;
		}
		
		assert( false );
		
		
	}
	
	
	
	void WorkerTask::finishWorker()
	{
		num_finished_workers++;
		check();
	}

    void printSubTasks( std::ostringstream & output , au::map<size_t , WorkerSubTask> & subtasks)
	{
	   au::map<size_t , WorkerSubTask>::iterator i;
	   for (i = subtasks.begin() ; i != subtasks.end() ; i++)
		  output << "[" << i->second->description << "]";
	}
	
	std::string WorkerTask::getStatus()
	{
		std::ostringstream output;
		output << "ID:" << task_id << " OP:" << operation << " ";

		switch (status) {
			case ready:
				output << "Ready";
				break;
			case running:
				output << "Running";
				break;
			case finish:
				output << "Finish";
				break;
			case completed:
				output << "Completed";
				break;
			default:
				break;
		}
		
		output << " ";
		
		//output << "Memory: " << subTasksWaitingForMemory.size();
		//output << " Read:" << subTasksWaitingForReadItems.size() 
		//output << " Run:" << subTasksWaitingForProcess.size();

		output << "Memory: ";
		printSubTasks( output, subTasksWaitingForMemory );
		output << " Read:";
		printSubTasks( output, subTasksWaitingForReadItems ); 
		output << " Run:";
		printSubTasks( output, subTasksWaitingForProcess );

		output << " Writing:" << processWriteItems.size() << "/" <<writeItems.size(); 
		
		if( queueBufferVectors.size() > 0)
		{
			output << " B: ";
			au::map<std::string , QueueuBufferVector>::iterator q;
			for ( q = queueBufferVectors.begin() ; q != queueBufferVectors.end() ; q++)	
				output << "(" << q->first << "=" << q->second->getStatus() << ")";
		}
		
		return output.str();
	}
	
	

	
#pragma mark Buffers processing
	
	void WorkerTask::addBuffer( network::Queue queue , Buffer *buffer ,bool txt )
	{
		std::string queue_name = queue.name();
		QueueuBufferVector* bv = queueBufferVectors.findInMap( queue_name );
		
		if( !bv )
		{
			bv = new QueueuBufferVector(  queue , txt );
			queueBufferVectors.insertInMap( queue_name , bv  );
		}
		
		// Compute the size of the new buffer
		size_t buffer_size;
		if( txt )
			buffer_size = buffer->getSize();
		else
		{
			// It is suppoused to be a KVHeader
			KVHeader * header = (( KVHeader *) buffer->getData());
			assert( header->check() );					// Assert magic number of incoming data packets
			buffer_size = header->info.size;
		}
		
		// If the new buffer will exceeed max file size, then create a new one
		
		if( buffer_size + bv->size > SamsonSetup::shared()->max_file_size )
		{
			QueueuBufferVector* bv = queueBufferVectors.extractFromMap( queue_name );
			assert( bv );
			
			// Process bv to generate a new file
			flush( bv );
			
			// Create a new one to store the new one
			bv = new QueueuBufferVector( queue , txt );
			queueBufferVectors.insertInMap( queue_name , bv  );
		}
		
		// Add the buffer properly
		bv->addBuffer( buffer );
	}
	
	void WorkerTask::flush( QueueuBufferVector *bv )
	{
		DataBufferProcessItem* tmp = new DataBufferProcessItem( bv );
		tmp->tag = task_id;											// Tag is used with the number of the task
		tmp->component = WORKER_TASK_COMPONENT_DATA_BUFFER_PROCESS;	// Component is used to route the notification at the TaskManager
		tmp->setProcessManagerDelegate(taskManager);
		processWriteItems.insert( tmp );
		
		// Add the process to joint the buffer
		ProcessManager::shared()->addProcessItem( tmp );
	}
	
	// add a buffer to be saved as a key-value file
	void WorkerTask::addFile( network::QueueFile &qf , Buffer *buffer )
	{
		// Add as an "add file" in the finish message ( it will be notified when the finish message is sent )
		finish_message->add_add_file( )->CopyFrom( qf );

		// Create and submit the FileManagerWriteItem to be saved on disk
		FileManagerWriteItem *item = new FileManagerWriteItem( qf.file().name() , buffer , taskManager);
		item->tag = task_id;									// Tag is used with the number of the task
		item->component = WORKER_TASK_COMPONENT_ADD_FILE;		// Component is used to route the notification at the TaskManager
		writeItems.insert( item );								// Insert in out local list of files to be saved
		
		FileManager::shared()->addItemToWrite( item );			// Schedule the new element to be saved on disk
		
	}
	
	void WorkerTask::removeFile( network::QueueFile &qf)
	{
		// Add as an "remove file" in the finish message ( it will be notified when the finish message is sent )
		finish_message->add_remove_file( )->CopyFrom( qf );
	}
	
	
	
	void WorkerTask::flush()
	{
		au::map<std::string , QueueuBufferVector>::iterator iter;
		
		for ( iter = queueBufferVectors.begin() ; iter != queueBufferVectors.end() ; iter++)
			flush( iter->second );
		queueBufferVectors.clear();
	}	
	
	
#pragma mark Messages

	void WorkerTask::sendCloseMessages( )
	{
		NetworkInterface *network = taskManager->worker->network;

		int num_workers = network->getNumWorkers();
		
		for (int s = 0 ; s < num_workers ; s++)
		{				
			Packet *p = new Packet();
			network::WorkerDataExchangeClose *dataMessage =  p->message.mutable_data_close();
			dataMessage->set_task_id(task_id);
			network->send(taskManager->worker, network->workerGetIdentifier(s) , Message::WorkerDataExchangeClose, p);
		}
	}	
	
	void WorkerTask::sendFinishTaskMessageToController( )
	{		
		
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		
		// Copy all the information from the prepared message
		confirmation->CopyFrom(*finish_message);
		
		if( error.isActivated() )
		{
			confirmation->set_type( network::WorkerTaskConfirmation::error );
			confirmation->set_error_message( error.getMessage() );
		}
		else
		{
			confirmation->set_type( network::WorkerTaskConfirmation::finish );
		}
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
		
	}	
	
	void WorkerTask::sendCompleteTaskMessageToController( )
	{	
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();

		// Copy all the information from the prepared message
		confirmation->CopyFrom(*complete_message);
		
		if( error.isActivated() )
		{
			confirmation->set_type( network::WorkerTaskConfirmation::error );
			confirmation->set_error_message( error.getMessage() );
		}
		else
		{
			confirmation->set_type( network::WorkerTaskConfirmation::complete );
		}
		
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
	}
	
	
	
	void WorkerTask::sendAddFileMessageToController( QueueuBufferVector *bv , std::string fileName , Buffer *b )
	{
		NetworkInterface *network = taskManager->worker->network;
		
		network::QueueFile qf;
		qf.set_queue( bv->queue->name() );
		network::File *file = qf.mutable_file();
		file->set_name( fileName );
		file->set_worker( network->getWorkerId() );
		network::KVInfo *info = file->mutable_info();
		
		// This is suppoused to be a file ( txt or kv )
		if( bv->txt )
		{
			info->set_size( b->getSize() );
			info->set_kvs( 1 );
		}
		else
		{
			KVHeader * header = (KVHeader*) ( b->getData() );
			info->set_size( header->info.size);
			info->set_kvs(header->info.kvs);
		}
		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_type( network::WorkerTaskConfirmation::update );
		confirmation->add_add_file()->CopyFrom( qf );
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
		
	}
	
	
		
#pragma mark FileName
	
	// Get a new file Name for a file

	std::string WorkerTask::newFileName( )
	{
		std::ostringstream fileName;
		
		int worker_id = taskManager->worker->_myWorkerId; 
		
		//fileName << SamsonSetup::shared()->dataDirectory << "/" << "file_" << worker_id << "_" << task_id << "_" << queue << "_" << rand()%10000 << rand()%10000 << rand()%10000;
		fileName << worker_id << "_" << task_id << "_" << rand()%10000 << rand()%10000 << rand()%10000 << rand()%10000;
		
		return fileName.str();
	}
	
	
}
