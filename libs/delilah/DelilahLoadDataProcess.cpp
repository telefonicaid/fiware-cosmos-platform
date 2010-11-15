

#include "DelilahLoadDataProcess.h"			// Own interface
#include "MemoryManager.h"					// ss::MemoryManager
#include "Buffer.h"							// ss::Buffer
#include "Packet.h"							// ss::Packet
#include "Message.h"						// ss::Message
#include "Delilah.h"						// ss::Delilah
#include "samson.pb.h"						// network::...
#include "DelilahClient.h"					// ss::DelilahClient

namespace ss
{
	
	
#pragma mark -
	
	void TXTFileSet::fill( Buffer *b )
	{
		
		if( finish )
		{
			return;	// Just in case
		}
		
		// Write the previous characters here
		bool ans = b->write(previousBuffer, previousBufferSize);
		assert( ans );	// There have to space for the previous buffer
		
		while( b->getAvailableWrite() > 0 )	// While there is space to fill
		{
			
			b->write( inputStream );
			
			// Open the next file if necessary
			if( inputStream.eof() )
			{
				inputStream.close();
				openNextFile();
				if( finish )
					return;
			}
		}
		
		// Full buffer
		// Remove the last chars until a complete line and keep for the next read
		previousBufferSize =  b->removeLastUnfinishedLine( previousBuffer );
		
	}
	
#pragma mark -
	
	void* runThreadDelilahLoadDataProcess(void *p)
	{
		DelilahLoadDataProcess *d = ((DelilahLoadDataProcess*)p);
		d->_run();
		return NULL;
	}
	
	
	void DelilahLoadDataProcess::_run()
	{
		
		while( !fileSet.isFinish() )
		{
			// Create a buffer
			Buffer *b = MemoryManager::shared()->newBuffer( 64*1024*1024 );
			
			// Fill the buffer
			fileSet.fill( b );
			
			uploadedSize += b->getSize();
			
			// Send to the rigth worker
			Packet p;
			p.buffer = b;	// Add the buffer to the packet

			// Get a new id for this packet
			size_t file_id = id_counter++;
			lock.lock();
			pending_ids.insert( file_id );
			lock.unlock();

			// Set message fields
			network::LoadData *loadData = p.message.mutable_load_data();	
			loadData->set_file_id( file_id );
			loadData->set_process_id( id );
			
			// Send the packet
			delilah->network->send(delilah, delilah->network->workerGetIdentifier(worker), Message::LoadData, &p);
			
			// Next worker
			if( ++worker == num_workers )
				worker = 0;
		}
		
		lock.lock();
		
		finish =  true;
		
		if( pending_ids.size() == 0)
		{
			completed = true;
			delilah->client->loadDataConfirmation(this);
		}
		
		lock.unlock();
			
	}
	
	bool DelilahLoadDataProcess::notifyDataLoad( size_t file_id , network::File file , bool error )
	{
		
		lock.lock();
		
		created_files.push_back(file);
		
		pending_ids.erase( file_id );
		
		if( finish )
			if ( pending_ids.size() == 0)
				completed =true;
		
		lock.unlock();
		
		return completed;
	}
	
	void DelilahLoadDataProcess::fillLoadDataConfirmationMessage( network::LoadDataConfirmation *confirmation )
	{
		lock.lock();
		
		confirmation->set_process_id( id );
		confirmation->set_queue( queue );

		for (size_t i = 0 ; i < created_files.size() ; i++)
		{
			network::File *file = confirmation->add_file();
			file->CopyFrom(created_files[i]);
		}
		
		lock.unlock();
	}
	
	

	
}