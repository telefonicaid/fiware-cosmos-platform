

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
		
		// First the header
		BufferHeader *header = (BufferHeader *)b->getData();
		b->skipWrite(sizeof(BufferHeader) );
		
		if( finish )
		{
			return;	// Just in case
		}
		
		// Write the previous characters here
		bool ans = b->write( previousBuffer, previousBufferSize );
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
				{
					// Information in the header
					header->init( 0 );	// Non compressed header
					header->original_size = b->getSize() - sizeof( BufferHeader );
					header->compressed_size = b->getSize() - sizeof( BufferHeader );
					return;
				}
			}
		}
		
		// Full buffer
		// Remove the last chars until a complete line and keep for the next read
		previousBufferSize =  b->removeLastUnfinishedLine( previousBuffer );

		// Information in the header
		header->init( 0 );	// Non compressed header
		header->original_size = b->getSize() - sizeof( BufferHeader );
		header->compressed_size = b->getSize() - sizeof( BufferHeader );
		
		
	}
	
#pragma mark -
	
	
	DelilahUploadDataProcess::DelilahUploadDataProcess( std::vector<std::string> &fileNames , std::string _queue , bool _compression, int _max_num_threads ) : fileSet( fileNames )
	{
		
		// Get initial time
		gettimeofday(&init_time, NULL);
		
		// Queue name 
		queue = _queue;

		// Information about the compression	
		compression = _compression;
		
		uploadedSize = 0;
		totalSize = 0;	
		
		num_threads = 0;
		max_num_threads = _max_num_threads;
		
		// Compute the total size for all the files
		for ( size_t i =  0 ; i < fileNames.size() ; i++)
			totalSize += au::Format::sizeOfFile( fileNames[i] );

		// Initial worker to sent data
		worker = 0; // rand()%num_workers;		// Random worker to start

		// Init counters of created and confirmed files
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

	
	void DelilahUploadDataProcess::run()
	{
		// Set the number of workers
		num_workers = delilah->network->getNumWorkers();
		
		// Create the thread for this load process
		pthread_create(&t, NULL, runThreadDelilahLoadDataProcess, this);
	}
	
	struct UploadPacketData {
		Packet *p;
		int worker;
		Delilah* delilah;
		DelilahUploadDataProcess* uploadDataProcess;
	};
	
	void* processUploadPacketData( void *p )
	{
		UploadPacketData * pd = (UploadPacketData*)p;
		
		// Compress the buffer
		if ( pd->uploadDataProcess->compression )
		{
			size_t original_size = pd->p->buffer->getSize();
			
			Buffer *buffer = Packet::compressBuffer( pd->p->buffer );
			MemoryManager::shared()->destroyBuffer( pd->p->buffer );
			pd->p->buffer = buffer;

			size_t compress_size = pd->p->buffer->getSize();
			
			std::ostringstream message;
			message << "Sending compressed buffer ( Size: " << au::Format::string( compress_size ) << " Original Size: " << au::Format::string(original_size) << "\n";
			pd->delilah->client->showMessage( message.str() );
			
		}
		
		// Notify that the thread finished
		pd->uploadDataProcess->finishCompressionThread();
		
		// Send the packet
		pd->delilah->network->send(pd->delilah, pd->delilah->network->workerGetIdentifier(pd->worker), Message::UploadData, pd->p);

		// Free allocated input parameter
		free( p );
		
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

			// Insert into the list of pending elements
			size_t file_id = num_files++;
			
			// Activate the finish flag if necessary before sending the packet ( so at the reception the flag is activated )
			if( fileSet.isFinish() )
			{
				lock.lock();
				finish =  true;
				lock.unlock();
			}
			
			// Send to the rigth worker
			Packet *p = new Packet();
			p->buffer = b;	// Add the buffer to the packet

			// Set message fields
			network::UploadData *loadData = p->message.mutable_upload_data();	
			loadData->set_file_id( file_id );				// File id
			loadData->set_original_size( b->getSize() );	// Size of the file we are uploading
			p->message.set_delilah_id( id );				// Global id of delilah jobs

			
			// Send the packet
			std::ostringstream message;
			message << getStatus() << " Created buffer " << au::Format::string( b->getSize() , "B" );
			delilah->client->showMessage(message.str());


			lock.lock();

			// Create a thread to compress this message and send it
			
			UploadPacketData *data  = (UploadPacketData*)malloc( sizeof(UploadPacketData) );
			
			data->p = p;
			data->delilah = delilah;
			data->worker = worker++;
			if ( worker >= num_workers )
				worker = 0;
			data->uploadDataProcess = this;
			
			pthread_t t;
			pthread_create(&t, NULL, processUploadPacketData, (void*) data);
			num_threads++;
			
			lock.unlock();
			
			// Wait if memory is not released
			while( ( MemoryManager::shared()->getMemoryUsage() > 0.7 ) || ( num_threads >= max_num_threads ) )
				sleep(1);
		}
	}
	

	
	void DelilahUploadDataProcess::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		lock.lock();
		
		if (msgCode == Message::UploadDataResponse )
		{
			//size_t file_id = packet->message.upload_data_response().upload_data().file_id();
			
			
			error				= packet->message.upload_data_response().error();
			error_message		= packet->message.upload_data_response().error_message();
			
			network::File file	= packet->message.upload_data_response().file();
			
			// update the uploaded data
			uploadedSize +=  packet->message.upload_data_response().upload_data().original_size();
			//uploadedSize += file.info().size();
			
			created_files.push_back(file);
			num_confirmed_files++;
			
			if( finish )
				if ( num_files == num_confirmed_files)
					completed = true;
			
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

			delilah->client->showMessage(getStatus());
			
			
		} else if (msgCode == Message::UploadDataConfirmationResponse )
		{
			assert( finish );
			assert( completed );
			
			network::UploadDataConfirmationResponse confirmation = packet->message.upload_data_confirmation_response();
			error			= confirmation.error();
			error_message	= confirmation.error_message();
			
			// Notify to the client to show on scren the result of this load process
			delilah->client->loadDataConfirmation( this );
			
			// mark the component as finished to be removed by Delilah component
			component_finished = true;
			
			delilah->client->showMessage(getStatus());
			
			
		}
		else
		{
			//??
		}

		lock.unlock();		
		
	}

	
	std::string DelilahUploadDataProcess::getStatus()
	{
		
		std::ostringstream output;
		
		int seconds = au::Format::ellapsedSeconds(&init_time);
		
		output << "[ Upload "<< id << "] Queue: " << queue << ": ";
		
		output << "[" << au::Format::time_string(seconds) << "] ";

		
		if( completed )
			output << " FINISH AND CONFIRMATED ";
		else if( finish )
			output << " FINISH ";

		{
			if( uploadedSize > 0)
			{
				size_t pending_secs =  ( totalSize - uploadedSize ) * seconds / uploadedSize;
				output << "[ETA " << au::Format::time_string( pending_secs ) << "] ";
			}
		}
		

		int p;
		if( totalSize > 0)
			p = ( (double) uploadedSize / (double) totalSize ) * 100;
		else
			p = 0;
		
		size_t r;
		
		if ( seconds > 0 )
			r = ((double) uploadedSize * 8.0 / (double) seconds);
		else
			r = 0;
		
		size_t r2 = r / num_workers;
		output << " [ Size " << au::Format::string( uploadedSize ) << " / " << au::Format::string( totalSize ) << " " << p << "%" << " ]";
		output << "[ Rate: " << au::Format::string( r , "bps" ) << " - " << au::Format::string( r2 , "bps" ) << " per worker ]";

		return output.str();
	}
	
}
