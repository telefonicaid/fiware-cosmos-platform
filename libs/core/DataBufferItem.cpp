
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
#include "SamsonSetup.h"			// ss::SamsonSetup

namespace ss {
	
	DataBufferItem::DataBufferItem( DataBuffer *_dataBuffer , size_t _task_id ,int _num_workers )
	{
		dataBuffer = _dataBuffer;
		task_id = _task_id;

		num_finished_workers = 0;
		num_workers = _num_workers;
		
		pending_files_to_be_created=0;
		pending_files_to_be_saved=0;

		
		finish_task_notified = false;
		to_be_removed = false;
		
	}	
	
	bool DataBufferItem::addBuffer( network::Queue queue , Buffer *buffer ,bool txt )
	{
		std::string queue_name = queue.name();
		QueueuBufferVector* bv = findInMap( queue_name );
		
		bool added_new_buffer_vectors = false;
		
		if( !bv )
		{
			bv = new QueueuBufferVector(  task_id , queue , txt );
			insertInMap( queue_name , bv  );
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
			QueueuBufferVector* bv = extractFromMap( queue_name );
			assert( bv );
			dataBuffer->pendingBufferVectors.push_back( bv );
			pending_files_to_be_created++;	// Mark as pending to be created by background thread
			pending_files_to_be_saved++;
			
			// To return "true" to DataBuffer
			added_new_buffer_vectors = true;
			
			
			// Create a new one to store the new one
			bv = new QueueuBufferVector(  task_id , queue , txt );
			insertInMap( queue_name , bv  );
			
		}

		bv->addBuffer( buffer );
		
		
		return added_new_buffer_vectors;
		
	}
	
	bool DataBufferItem::finishWorker()
	{
		
		bool added_new_buffer_vectors = false;
		
		num_finished_workers++;
		
		if( num_finished_workers == num_workers )
		{
			
			// Flush to disk the rest of the buffer vector to disk
			for ( std::map<std::string , QueueuBufferVector* >::iterator i = begin() ; i != end() ; i++)
			{
				if( i->second->size > 0)
				{
					QueueuBufferVector* bv = i->second;
					assert( bv );
					dataBuffer->pendingBufferVectors.push_back( bv );
					pending_files_to_be_created++;	// Mark as pending to be created by background thread
					pending_files_to_be_saved++;
					
					added_new_buffer_vectors = true;	// Return true to DataBuffer
				}
			}
			
			// Remove all the elements in the map
			clear();
			
		}		
		
		// Check what to do
		check();
		
		return added_new_buffer_vectors;
	}	
	
	
	void DataBufferItem::newFileCreated()
	{
		pending_files_to_be_created--;
		check();
	}
	
	// common function to check everything in a single function
	void DataBufferItem::check()
	{
		
		if( (num_finished_workers == num_workers) && (pending_files_to_be_created==0) )
		{
			// The task is finished and files are scheduled to be saved to disk
			if( !finish_task_notified )
			{
				finish_task_notified = true;
				// Notify the task manager about the "finish" fo this task ( only once )
				dataBuffer->worker->taskManager.finishTask( task_id );
			}
			
			
		}
		
		if( (num_finished_workers == num_workers) && ( pending_files_to_be_saved == 0 ) && ( pending_files_to_be_created==0 ) )
		{
			// Notify to the task manager that this is completed
			dataBuffer->worker->taskManager.completeTask( task_id );	
			to_be_removed =  true;	// Flag set  to be eliminated by DataBuffer
		}
		
		
	}
	
	

	
	void DataBufferItem::fileManagerNotifyFinish(size_t id, bool success)
	{
		
		if( success )
			pending_files_to_be_saved--;
		else
			assert( false );

		check();
		
	}	
	
	void DataBufferItem::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		// Nothing to do here... to be removed
	}	
	
	std::string DataBufferItem::getStatus()
	{
		// Protected by lock of DataBuffer...
		
		std::ostringstream output;
		
		output << "Task: " << task_id;
		output	 << " ";
		
		std::map<std::string , QueueuBufferVector*>::iterator iter;
		for ( iter = this->begin() ; iter != this->end() ; iter++)
			output << "<Q:" << iter->second->buffer.size() << ">";
		output << " ";
		if( num_workers == num_finished_workers )
		{
			if( pending_files_to_be_created == 0)
			{
				if( pending_files_to_be_saved == 0 )
					output << " (C) ";
				else
					output << " (Sa:"<< pending_files_to_be_saved <<") ";
			}
			else
				output << " (Cr:"<< pending_files_to_be_created <<") ";
		}
		else
			output << " (W:"<< num_finished_workers	<< "/" << num_workers << ") ";

		return output.str();
		
	}


	
	
}
