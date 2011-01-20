

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

	}
	
	WorkerTask::~WorkerTask()
	{
		if( reduceInformation )
			delete reduceInformation;
	}
	
	
	void WorkerTask::setup(Operation::Type type , const network::WorkerTask &task)
	{
		// Copy of the message form the controller
		workerTask = task;
		
		assert( status == pending_definition);
		
		operation = task.operation();	// Save the operation to perform		
		task_id = task.task_id();		// Save the task id
		
		// By default no error
		error = false;
		
		// Set status to runnign until all the tasks are completed ( or error )
		status = running;
		
		switch (type) {
				
			case Operation::generator :
				if( task.generator() )
				{
					GeneratorSubTask * tmp = new GeneratorSubTask( this );
					addSubTask( tmp );
				}
				break;
				
			case Operation::parser:
			{
				// An item per file
				assert( task.input_size() == 1);	// Only one input
				network::FileList fl = task.input(0);
				
				for (size_t i = 0 ; i < (size_t) fl.file_size() ; i++)
					addSubTask( new ParserSubTask( this, fl.file(i).name() ) );
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
		
		if( status == error)
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
		
		if ( status == running )
		{
			// If no more tasks, then set to finish and send a message to the rest of workers
			if( subTasksWaitingForMemory.size() == 0 )
				if( subTasksWaitingForReadItems.size() == 0 )
					if( subTasksWaitingForProcess.size() == 0 )
				    {				
						// Send a close message to all the workers
						sendCloseMessages();
						
						status = waiting;
					}
		}

		if( status == waiting )
		{
			if( num_finished_workers == num_workers )
			{
				// FLush to emit all the pending buffers to a file
				flush();
				
				// Now the status is waiting for the all the workers confirm ( me included ) finish generating data
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
			if( error )
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
				if( i->error )
					setError(i->error_message);
				
				check();
				
			}
				break;
				
			case WORKER_TASK_COMPONENT_DATA_BUFFER_PROCESS:
			{
				DataBufferProcessItem* tmp = (DataBufferProcessItem*) i;
				
				processWriteItems.erase(tmp);
				
				// New file to be saved
				std::string queue_name = tmp->bv->queue->name();
				std::string fileName = newFileName( queue_name );
				
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
		output << "ID:" << task_id << " OP:" << operation << ") ";

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
			// It is suppoused to be a NetworkHeader
			NetworkHeader * header = (( NetworkHeader *) buffer->getData());
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
		confirmation->set_task_id( task_id );
		
		if( error )
		{
			confirmation->set_type( network::WorkerTaskConfirmation::error );
			confirmation->set_error_message( error_message );
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
		confirmation->set_task_id( task_id );
		
		if( error )
		{
			confirmation->set_type( network::WorkerTaskConfirmation::error );
			confirmation->set_error_message( error_message );
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
			FileHeader * header = (FileHeader*) ( b->getData() );
			info->set_size( header->info.size);
			info->set_kvs(header->info.kvs);
		}
		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_type( network::WorkerTaskConfirmation::new_file );
		confirmation->add_file()->CopyFrom( qf );
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
		
		
	}
	
	
		
#pragma mark FileName
	
	
	std::string WorkerTask::newFileName( std::string queue )
	{
		std::ostringstream fileName;
		fileName << SamsonSetup::shared()->dataDirectory << "/" << "file_" << queue << "_" << rand()%10000 << rand()%10000 << rand()%10000;
		return fileName.str();
	}
	
	
}
