
#include "DelilahDownloadDataProcess.h"
#include "Delilah.h"				// ss::Delilah

namespace ss {

	void* runDelilahDownloadDataProcessThread( void* p)
	{
		// Run the thread process
		((DelilahDownloadDataProcess*)p)->runThread();
		return NULL;
	}
	
	DelilahDownloadDataProcess::DelilahDownloadDataProcess( std::string _queue , std::string _fileName , bool _show_on_screen ) : DelilahComponent(DelilahComponent::load) , stopLock(&lock) 
	{
		queue = _queue;
		fileName = _fileName;

		show_on_screen = _show_on_screen;
		
		// Initial state until we receive information about queue
		num_files_to_download = -1;
		num_files_downloaded = 0;
		
		total_size = 0;
		
		file = fopen( fileName.c_str() , "w" );
		
		if( !file )
		{
			status = finish_with_error;
			error.set("Not possible to open local file");
			delilah->downloadDataConfirmation(this);
			component_finished = true;
			return;
		}

		// Pending to send the message to the controller
		status = uninitialized;
		
		// Received message from the controller initially NULL
		download_data_init_response = NULL;
		
	}
	
	std::string DelilahDownloadDataProcess::getDescription()
	{
		std::ostringstream o;
		o << "Downloading from queue " << queue << " to local file " << fileName;
		return o.str();
	}


	void DelilahDownloadDataProcess::run()
	{
		
		if( status != uninitialized )
			LM_X(1,("Unexpected status in an download data process"));

		status = waiting_controller_init_response;
		
		// Send the message to the controller
		Packet *p = new Packet(Message::DownloadDataInit);
		p->message->set_delilah_id( id );
		ss::network::DownloadDataInit *download_data_init = p->message->mutable_download_data_init();
		download_data_init->set_queue( queue );	// Set the queue we want to download
		delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), p);
		
		// Init the thread to process data in background
		pthread_create(&t, 0, runDelilahDownloadDataProcessThread, this);
	}
	
	void DelilahDownloadDataProcess::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		
		if (msgCode == Message::DownloadDataInitResponse )
		{
			// Reponse from the controller
			if( status != waiting_controller_init_response)
				LM_X(1,("Unexpected status in an download data process"));

			// Copy the message received from the controller
			download_data_init_response = new ss::network::DownloadDataInitResponse();
			download_data_init_response->CopyFrom( packet->message->download_data_init_response() );

			// Chage the status to download files
			status = downloading_files_from_workers;
			

			if( download_data_init_response->has_error() )
			{
				error.set( download_data_init_response->error().message() );
				status = finish_with_error;
				delilah->downloadDataConfirmation(this);
				component_finished = true;
				return;
			}
			
			// Set the number of files to download
			num_files_to_download = download_data_init_response->queue().file_size();

			status = downloading_files_from_workers;
			
			// Request for all the files
            size_t offset = 0;
            
			for ( int i = 0 ; i < download_data_init_response->queue().file_size() ; i++)
			{
				Packet *p = new Packet( Message::DownloadDataFile);
				p->message->set_delilah_id( id );
				ss::network::DownloadDataFile *download_data_file = p->message->mutable_download_data_file();

                // Order in witch the files will be downloaded
                download_data_file->set_file_id( i );
                
				// Copy the file information
				download_data_file->mutable_file()->CopyFrom( download_data_init_response->queue().file(i) );
				
				// Send to the rigth worker
				int worker = download_data_init_response->queue().file(i).worker();
                
				delilah->network->send(delilah, delilah->network->workerGetIdentifier(worker) , p);
                
                offset_per_file.push_back( offset );
                offset += download_data_init_response->queue().file(i).info().size();
			}
			
			
			
		}
		else if (msgCode == Message::DownloadDataFileResponse)
		{
			
			lock.lock();

			num_files_downloaded ++;
			
			if ( num_files_downloaded == num_files_to_download)
				status = waiting_file_downloads_confirmations;

            packet->buffer->tag =  (int) packet->message->download_data_file_response().query().file_id();
			buffers.push_back( packet->buffer );
			
			lock.unlock();

			
			
			// Wake up the backgroun thread ( if sleept to save this buffer to disk )
			lock.wakeUpStopLock( &stopLock );
			
		}
		
	}
	
	void DelilahDownloadDataProcess::runThread()
	{
		
		while ( true )
		{
			if ( ( status == finish_with_error ) || ( status == finish ) )
				return;
			
			lock.lock();
			
            engine::Buffer *buffer = NULL;
			
			if( buffers.size() > 0)
			{
				buffer = buffers.front();
				buffers.pop_front();
			}
						
			lock.unlock();
			
			if( buffer )
			{
				// Write stuff to disk
				std::ostringstream out;

				int file_id = buffer->tag;
                
                if( ( file_id < 0 ) || (file_id >= (int)offset_per_file.size() ) )
                    LM_X(1,("Wrong file id while downloading data from SAMSON"));
                
                fseek( file, offset_per_file[file_id], SEEK_SET );  // Position in the rigth place
				fwrite( buffer->getData(), buffer->getSize() , 1 , file );
				total_size += buffer->getSize();
                
                engine::MemoryManager::shared()->destroyBuffer(buffer);
                buffer = NULL;
				
			}
			else
			{
				if( status == waiting_file_downloads_confirmations )
				{
					// No buffer and all files downloaded
					
					fclose( file );	// Close the file
					delilah->showTrace("Download completed");
					
					if( show_on_screen )
					{
						if( total_size > 10000)
						{
							delilah->showMessage("It is not allowed to show files with more than 10Kbyes on screen");
						}
						else
						{
							char *fileBuffer = (char*)malloc( total_size+1);
							fileBuffer[total_size] = '\0';
							
							FILE*  file = fopen(fileName.c_str(), "r");
							size_t nb;

							nb = fread(fileBuffer, 1, total_size, file);
							if (nb != total_size)
								LM_W(("read only %d bytes (wanted to read %d)", nb, total_size));
							
							delilah->showMessage(fileBuffer);
							fclose( file );
							
							free( fileBuffer );
						}
					}
					
					// Set the finish state
					status = finish;
					delilah->downloadDataConfirmation(this);
					
					// Mark as ready to be cleared from the list
					component_finished = true;
					return;
				}				
				
				lock.lock();
				lock.unlock_waiting_in_stopLock( &stopLock , 1 );
			}
		}
	}
	
	std::string DelilahDownloadDataProcess::getStatus()
	{
		std::ostringstream output;
		output << "[ "<< id << " ] Downloading queue " << queue << " to " << fileName << " ( STATE: ";
		
		switch (status) {
			case uninitialized:
				output << "Uninitialized";
				break;
			case waiting_controller_init_response:
				output << "Waiting controller response to our init message";
				break;
			case downloading_files_from_workers:
				output << "Downloading data";
				break;
			case waiting_file_downloads_confirmations:
				output << "Waiting confirmation of downloaded data";
				break;
			case waiting_controller_finish_response:
				output << "Waiting final message confirmation from controller";
				break;
			case finish_with_error:
				output << "ERROR: " << error.getMessage();
				break;
			case finish:
				output << "Finished correctly";
				break;
		}
		output << " )";
		
		
		output << "\n\tTotal files: " << num_files_downloaded << "/" << num_files_to_download;
		output << " Buffers: " << buffers.size() << "\n";
		return output.str();
	}	
}
