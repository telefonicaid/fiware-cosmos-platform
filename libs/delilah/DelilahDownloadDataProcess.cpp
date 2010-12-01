
#include "DelilahDownloadDataProcess.h"
#include "Delilah.h"				// ss::Delilah

namespace ss {

	
	void* runDelilahDownloadDataProcessThread( void* p)
	{
		// Run the thread process
		((DelilahDownloadDataProcess*)p)->runThread();
		return NULL;
	}
	
	
	
	DelilahDownloadDataProcess::DelilahDownloadDataProcess( std::string _queue , std::string _fileName ) : stopLock(&lock)
	{
		queue = _queue;
		fileName = _fileName;
	
		// Initial state until we receive information about queue
		num_files_to_download = -1;
		num_files_downloaded = -1;;
		
		
		file = fopen( fileName.c_str() , "w" );
		
		if( file )
		{
			error = false;	// By default no error ;)
			
		}
		else
		{
			error = true;
			error_message = "Not possible to open local file";
		}
	}

	void DelilahDownloadDataProcess::run()
	{
		Packet *p = new Packet();
		network::Help *help = p->message.mutable_help();
		help->set_queues(true);
		help->set_name( queue ); 
		p->message.set_delilah_id( id );
		
		delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Help , p);
		
	}
	
	void DelilahDownloadDataProcess::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		
		if( msgCode == Message::HelpResponse )
		{
			
			if( packet->message.help_response().queue_size() == 0)
			{
				error = true;
				error_message = "Queue does not exist\n";
				return;
			}
			
			// For each file, send a message to the rigth worker
			for ( int i = 0 ; i < packet->message.help_response().queue(0).file_size() ;i++)
			{
				Packet *p = new Packet();
				network::DownloadData *downloadData = p->message.mutable_download_data();
				downloadData->set_file_id( i );
				
				network::File *file = downloadData->mutable_file();
				file->CopyFrom( packet->message.help_response().queue(0).file(i)  );
				p->message.set_delilah_id( id );
				
				int worker = packet->message.help_response().queue(0).file(i).worker();
				
				delilah->network->send(delilah, delilah->network->workerGetIdentifier(worker) , Message::DownloadData , p);
				
				num_files_to_download++;
			}
			
			// Run the backgorund thread to save buffers received with the answers
			pthread_create(&t, NULL, runDelilahDownloadDataProcessThread, this);
			
			
		}
		
		if (msgCode == Message::DownloadDataResponse)
		{
			num_files_downloaded ++;
			
			lock.lock();
			buffers.push_back( packet->buffer );
			lock.unlock();
			
			// Wake up the backgroun thread ( if sleept to save this buffer to disk )
			lock.wakeUpStopLock( &stopLock );
			
		}
		
		/*
		if( num_files_downloaded == num_files_to_download)
		{
			component_finished =  true;
			delilah->client->showMessage("Download complete");
		}
		 */
		
	}
	
	void DelilahDownloadDataProcess::runThread()
	{
		
		while ( !component_finished )
		{
			lock.lock();
			
			Buffer *buffer = NULL;
			
			if( buffers.size() > 0)
			{
				buffer = buffers.front();
				buffers.pop_front();
			}
			
			if( ( num_files_to_download == num_files_downloaded ) && ( buffers.size() == 0) )
				component_finished =  true;
			
			lock.unlock();
			
			if( buffer )
			{
				// Write stuff to disk
				std::ostringstream out;
				
				fwrite( buffer->getData(), buffer->getSize() , 1 , file );
				MemoryManager::shared()->destroyBuffer( buffer );
				
				if( component_finished )
				{
					fclose( file );
					delilah->client->showMessage("Download completed");
					
					// Delete the component from delilah
					DelilahComponent *component = delilah->components.extractFromMap( id );
					delilah->client->notifyFinishOperation(id);
					delete component;
					
					return;
				}
				
				
				
			}
			else
			{
				lock.lock();
				lock.unlock_waiting_in_stopLock( &stopLock );
			}
		}
		
		
	}
	
}