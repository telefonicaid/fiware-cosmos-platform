

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
	
	
	DelilahUploadDataProcess::DelilahUploadDataProcess( std::vector<std::string> &fileNames , std::string _queue ) : fileSet( fileNames )
	{
		// Get initial time
		gettimeofday(&init_time, NULL);
		
		// Queue name 
		queue = _queue;
		
		uploadedSize = 0;
		totalSize = 0;	
		
		for ( size_t i =  0 ; i < fileNames.size() ; i++)
			totalSize += au::Format::sizeOfFile( fileNames[i] );
		
		worker = 0; // rand()%num_workers;		// Random worker to start
		
		num_files = 0;
		num_confirmed_files = 0;
		
		finish = false;
		completed = false;
	}	
	
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
			Buffer *b = MemoryManager::shared()->newBuffer( "Loading buffer" , 64*1024*1024 );
			
			// Fill the buffer with the contents from the file
			fileSet.fill( b );

			// Send to the rigth worker
			Packet *p = new Packet();
			p->buffer = b;	// Add the buffer to the packet

			// Insert into the list of pending elements
			size_t file_id = num_files++;

			// Set message fields
			network::UploadData *loadData = p->message.mutable_upload_data();	
			loadData->set_file_id( file_id );
			p->message.set_delilah_id( id );		// Global id of delilah jobs

			
			// Send the packet
			
			std::ostringstream message;
			message << getStatus() << " Sending buffer of " << au::Format::string( b->getSize() , "B" ) << " with id " << file_id;
			delilah->client->showMessage(message.str());
			
			delilah->network->send(delilah, delilah->network->workerGetIdentifier(worker), Message::UploadData, p);
			
			// Next worker
			if( ++worker == num_workers )
				worker = 0;
			
			// Wait if memory is not released
			while( MemoryManager::shared()->getMemoryUsage() > 0.7 )
				sleep(1);
			
		}
		
		lock.lock();
		
		finish =  true;
		
		if( num_files == num_confirmed_files )
		{
			completed = true;
			delilah->client->loadDataConfirmation(this);
		}
		
		lock.unlock();
			
	}
	
	void DelilahUploadDataProcess::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		lock.lock();
		
		if (msgCode == Message::UploadDataResponse )
		{
			size_t file_id = packet->message.upload_data_response().upload_data().file_id();
			
			std::ostringstream message;
			message << getStatus() << " Received confirmation with file_id " << file_id;
			delilah->client->showMessage(message.str());
			
			error			= packet->message.upload_data_response().error();
			error_message	= packet->message.upload_data_response().error_message();
			
			network::File file = packet->message.upload_data_response().file();
			
			// update the uploaded data
			uploadedSize += file.info().size();
			
			created_files.push_back(file);
			num_confirmed_files++;
			
			if( finish )
				if ( num_files == num_confirmed_files)
					completed =true;
			
			if ( completed )
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
			
		} else if (msgCode == Message::UploadDataConfirmationResponse )
		{
			network::UploadDataConfirmationResponse confirmation = packet->message.upload_data_confirmation_response();
			error			= confirmation.error();
			error_message	= confirmation.error_message();
			
			// Notify to the client to show on scren the result of this load process
			delilah->client->loadDataConfirmation( this );
			
			// mark the component as finished to be removed
			component_finished = true;
			
			std::ostringstream message;
			message << getStatus() << " Received an upload confirmation message ";
			delilah->client->showMessage(message.str());
			
			
		}
		else
		{
			std::ostringstream message;
			message << getStatus() << " Received strange message with code " << msgCode;
			delilah->client->showMessage(message.str());
		}

		lock.unlock();		
		
	}

	
	
	void DelilahUploadDataProcess::run()
	{
		// Set the number of workers
		num_workers = delilah->network->getNumWorkers();
		
		// Create the thread for this load process
		pthread_create(&t, NULL, runThreadDelilahLoadDataProcess, this);
	}
	
	void DelilahUploadDataProcess::_run();		
	
	std::string DelilahUploadDataProcess::getStatus()
	{
		
		std::ostringstream output;
		
		int seconds = au::Format::ellapsedSeconds(&init_time);
		
		output << "["<< id << "] Upload to " << queue << ": ";
		
		output << "[" << au::Format::time_string(seconds) << "] ";
		
		if( uploadedSize > 0)
		{
			size_t pending_secs =  ( totalSize - uploadedSize ) * seconds / uploadedSize;
			output << "[ETA " << au::Format::time_string( pending_secs ) << "] ";
		}
		

		int p = ( (double) uploadedSize / (double) totalSize ) * 100;
		size_t r = ((double) uploadedSize * 8.0 / (double) seconds);
		size_t r2 = r / num_workers;
		output << " [ Files " << num_confirmed_files << " / " << num_files << "]";
		output << " [ Size " << au::Format::string( uploadedSize ) << " / " << au::Format::string( totalSize ) << " " << p << "%" << " ]";
		output << "[ Rate: " << au::Format::string( r , "bps" ) << " - " << au::Format::string( r2 , "bps" ) << " per worker ]";

		return output.str();
	}
	
}
