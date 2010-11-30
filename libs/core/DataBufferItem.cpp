
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// Own interface
#include "BufferVector.h"			// ss::BufferVector
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate
#include "DataBuffer.h"				// ss::DataBuffer
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet

namespace ss {
	
	DataBufferItem::DataBufferItem( DataBuffer *_dataBuffer , size_t _task_id , int _myWorkerId, int _num_workers )
	{
		dataBuffer = _dataBuffer;
		task_id = _task_id;

		num_finished_workers = 0;
		
		num_workers = _num_workers;
		myWorkerId = _myWorkerId;
		
		finished = false;	// Flag
		completed = false;
		
		
	}	
	
	void DataBufferItem::addBuffer( network::Queue queue , Buffer *buffer )
	{
		std::string name = queue.name();
		QueueuBufferVector* bv = findInMap( name );
		
		if( !bv )
		{
			bv = new QueueuBufferVector( queue );
			insertInMap( name , bv  );
		}
		
		if( buffer->getSize() + bv->getSize() > KV_MAX_FILE_SIZE )
		{
			Buffer *b = bv->getFileBufferFromNetworkBuffers( KVFormat( queue.format().keyformat() , queue.format().valueformat() ) );
			std::string fileName = newFileName( name );
			saveBufferToDisk( b , fileName , bv->queue );
		}
		
		bv->addBuffer( buffer );
		
	}
	
	void DataBufferItem::finishWorker()
	{
		num_finished_workers++;
		
		if( num_finished_workers == num_workers )
		{
			// Set the finish flag to indicate that this task operation is finished
			finished = true;
		
			// Flush to disk the rest of the buffer
			for ( std::map<std::string , QueueuBufferVector* >::iterator i = begin() ; i != end() ; i++)
			{
				if( i->second->getSize() > 0)
				{
					QueueuBufferVector *bv = i->second;
					network::Queue queue = bv->queue;
					Buffer *b = bv->getFileBufferFromNetworkBuffers( KVFormat( queue.format().keyformat() , queue.format().valueformat() ) );
					std::string fileName = newFileName( i->first );
					saveBufferToDisk( b , fileName , bv->queue );
				}
			}
			
			
			
			// Send a message to the controller to notify that task is "finish" but not "complete"
			Packet *p = new Packet();
			network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
			confirmation->set_task_id( task_id );
			confirmation->set_finish( true );
			confirmation->set_completed( false );
			confirmation->set_error( false );
			dataBuffer->worker->network->send(dataBuffer->worker, dataBuffer->worker->network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
			
			// Just in case, there is nothing else to save
			if( ids_files.size() == 0 )
			{
				completed = true;
				dataBuffer->worker->taskManager.completeTask( task_id );	// Notify to the task manager that this is completed
				
			}
		}		
	}	
	
	
	
	void DataBufferItem::saveBufferToDisk( Buffer* b , std::string fileName , network::Queue queue )
	{
		
		// Notify the controller that a file has been created ( update )

		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();

		confirmation->set_task_id( task_id );
		confirmation->set_finish( false );
		confirmation->set_completed( false );
		confirmation->set_error( false );
		
		network::QueueFile *qf = confirmation->add_file();
		qf->set_queue( queue.name() );
		network::File *file = qf->mutable_file();
		file->set_name( fileName );
		file->set_worker( myWorkerId );
		network::KVInfo *info = file->mutable_info();
		
		// this is suppoused to be a file
		FileKVInfo * _info = (FileKVInfo*) ( b->getData() + sizeof(FileHeader) );
		info->set_size(_info->size);
		info->set_kvs(_info->kvs);
		

		// Send the message
		NetworkInterface *network = dataBuffer->worker->network;
		network->send(dataBuffer->worker, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation , p);
		
		// Schedule at the File Manager
		size_t id = FileManager::shared()->addItemToWrite( new FileManagerWriteItem( fileName,b , dataBuffer ) ) ;
		
		dataBuffer->id_relation.insert( std::pair<size_t,size_t>( id, task_id) );
		
		// Keep ids to control if files are saved on disk
		ids_files.insert( id );
		
	}
	
	void DataBufferItem::diskManagerNotifyFinish(size_t id, bool success)
	{
		
		if( success )
			ids_files_saved.insert( id );
		else
		{
			assert( false );
		}
		
		if( !completed && ( ids_files.size() == ids_files_saved.size() ) && finished )
		{
			completed = true;

			// Notify to the task manager that this is completed
			dataBuffer->worker->taskManager.completeTask( task_id );
		}
		
		
	}	
	
	std::string DataBufferItem::newFileName( std::string queue)
	{
		std::ostringstream fileName;
		fileName << "/tmp/" << queue << rand()%1000 << rand()%1000;
		return fileName.str();
	}
	
	std::string DataBufferItem::getStatus()
	{
		std::ostringstream output;
		if( completed )
			output << "[COMPLETED]";
		else if( finished )
			output << "[FINISHED] ";
		
		output << "[Closed " << num_finished_workers << " of " << num_workers << " workers ] ";
		output << "[Files ids " << ids_files_saved.size() << " / " << ids_files.size() << " files ] ";
		output << "[Created " << ids_files.size() << " files ] ";
		
		std::map<std::string , QueueuBufferVector* >::iterator i;
		for (i = begin() ; i != end() ; i++)
			output << "[ Queue: " << i->first << " " << i->second->getInfo().str() << " ]";
		
		return output.str();
	}	
	
	
}
