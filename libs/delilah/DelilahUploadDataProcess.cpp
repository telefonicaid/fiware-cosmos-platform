

#include "DelilahUploadDataProcess.h"			// Own interface
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
		DelilahUploadDataProcess *d = ((DelilahUploadDataProcess*)p);
		d->_run();
		return NULL;
	}
	
	
	void DelilahUploadDataProcess::_run()
	{
		
		while( !fileSet.isFinish() )
		{
			// Create a buffer
			delilah->client->showMessage("Creating buffer to load data");
			Buffer *b = MemoryManager::shared()->newBuffer( "Loading buffer" , 64*1024*1024 );
			
			// Fill the buffer
			fileSet.fill( b );
			
			uploadedSize += b->getSize();
			
			// Send to the rigth worker
			Packet *p = new Packet();
			p->buffer = b;	// Add the buffer to the packet

			// Get a new id for this packet
			size_t file_id = id_counter++;
			lock.lock();
			pending_ids.insert( file_id );
			lock.unlock();

			// Set message fields
			network::UploadData *loadData = p->message.mutable_upload_data();	
			loadData->set_file_id( file_id );
			p->message.set_delilah_id( id );		// Global id of delilah jobs
			
			// Send the packet
			delilah->network->send(delilah, delilah->network->workerGetIdentifier(worker), Message::UploadData, p);
			
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
	
	void DelilahUploadDataProcess::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		
		if (msgCode == Message::UploadDataResponse )
		{
			size_t		file_id			= packet->message.upload_data_response().upload_data().file_id();
			
			error			= packet->message.upload_data_response().error();
			error_message	= packet->message.upload_data_response().error_message();
			
			network::File file = packet->message.upload_data_response().file();
			
			created_files.push_back(file);
			pending_ids.erase( file_id );
			if( finish )
				if ( pending_ids.size() == 0)
					completed =true;
			
			if ( isUploadFinish() )
			{
				// Send the final packet to the controller notifying about the loading process
				Packet *p = new Packet();
				network::UploadDataConfirmation *confirmation	= p->message.mutable_upload_data_confirmation();
				confirmation->set_queue( queue );
				p->message.set_delilah_id( id );
				
				for (size_t i = 0 ; i < created_files.size() ; i++)
				{
					network::File *file = confirmation->add_file();
					file->CopyFrom(created_files[i]);
				}
				
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::UploadDataConfirmation, p);
			}
		}
		
		if (msgCode == Message::UploadDataConfirmationResponse )
		{
			network::UploadDataConfirmationResponse confirmation = packet->message.upload_data_confirmation_response();
			error			= confirmation.error();
			error_message	= confirmation.error_message();
			
			// Notify to the client to show on scren the result of this load process
			delilah->client->loadDataConfirmation( this );
			
			// mark the component as finished to be removed
			component_finished = true;
			
		}
		
	}

	DelilahUploadDataProcess::DelilahUploadDataProcess( std::vector<std::string> &fileNames , std::string _queue ) : fileSet( fileNames )
	{
		
		// Queue name 
		queue = _queue;
		
		uploadedSize = 0;
		
		worker = 0; // rand()%num_workers;		// Random worker to start
		
		id_counter = 0;	// Init counter for loading files to the workers
		
		finish = false;
		completed = false;
	}
	
	
	bool DelilahUploadDataProcess::isUploadFinish()
	{
		return completed;
	}
	
	void DelilahUploadDataProcess::run()
	{
		// Set the number of workers
		num_workers = delilah->network->getNumWorkers();
		
		
		// Create the thread for this load process
		pthread_create(&t, NULL, runThreadDelilahLoadDataProcess, this);
	}
	
	void DelilahUploadDataProcess::_run();		
	
	size_t DelilahUploadDataProcess::getId()
	{
		return id;
	}
	
	size_t DelilahUploadDataProcess::getUploadedSize()
	{
		return uploadedSize;
	}
	
	std::vector<std::string> DelilahUploadDataProcess::getFailedFiles()
	{
		return fileSet.getFailedFiles();
	}
	
	std::vector<network::File> DelilahUploadDataProcess::getCreatedFile()
	{
		return created_files;
	}
	
	
	std::string DelilahUploadDataProcess::getStatus()
	{
		std::ostringstream output;
		
		output << "["<< id << "]Uploading to queue " << queue << " : ";
		output << "Pending files: " << pending_ids.size();
		output << " Created files: " << created_files.size() << std::endl;
		
		return output.str();
	}
	
}
