
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "BufferVector.h"			// Own interface

namespace ss {

	QueueuBufferVector::QueueuBufferVector(size_t _task_id, const network::Queue &_queue , bool _txt )
	{
		// Init all the counter
		txt		= _txt;
		size	= 0;
		info.clear();
		
		task_id = _task_id;
		
		// Get a copy of the network information
		queue = new network::Queue();
		queue->CopyFrom( _queue );
		
		clear();
		
	}

	QueueuBufferVector::~QueueuBufferVector()
	{
		delete queue;
	}
	
	void QueueuBufferVector:: clear()
	{
		
		size = 0;
		info.clear();
		
		// Remove all buffers
		for (size_t i = 0 ; i < buffer.size() ; i++)
		{
			MemoryManager::shared()->destroyBuffer( buffer[i] );
			buffer[i] = NULL;
		}
		
		// Empty the vector of buffers
		buffer.clear();
		
	}
	
	void QueueuBufferVector::addBuffer( Buffer *b )
	{
		// Add the buffer to the vector
		buffer.push_back( b );
		
		if( txt )
			size += b->getSize(); 
		else
		{
			// Update the total ( size and number of kvs )
			NetworkHeader * header = (( NetworkHeader *) b->getData());
			
			// Assert magic number of incoming data packets
			assert( header->check() );	
			
			// Increase total information for this file
			info.kvs	+= header->info.kvs;
			info.size	+= header->info.size;

			// Increase the total size to monitorize the global file size
			size += header->info.size;
		}
	}
	
	Buffer* QueueuBufferVector::getJoinedBufferAndClear()
	{
		Buffer *outputBuffer;
		
		if( txt )
			outputBuffer = getTXTBufferFromBuffers();
		else
			outputBuffer = getFileBufferFromNetworkBuffers();
		
		clear();
		
		return outputBuffer;
	}
	
	/**
	 Buidl a TXT buffer with just the accumulation of the component buffers
	 */
	
	Buffer *QueueuBufferVector::getTXTBufferFromBuffers()
	{
		size_t file_size = 0;
		for (size_t i=0;i < buffer.size() ;i++)
			file_size += buffer[i]->getSize();
		
		// Crearte the buffer
		Buffer *b = MemoryManager::shared()->newBuffer( "Creating txt file from buffers" , file_size );
		
		for (size_t i=0;i < buffer.size() ;i++)
			b->write( buffer[i]->getData(), buffer[i]->getSize() );

		// Make sure buffer is correct
		assert( b->getSize() == b->getMaxSize() );
		return b;
	}
	
	
	/**
	 Build a file in the SAMSON format with the incomming network buffers
	 */
	
	Buffer* QueueuBufferVector::getFileBufferFromNetworkBuffers( )
	{
		KVFormat queue_format( queue->format().keyformat() , queue->format().valueformat() );
		
		
		// Check all network buffers to be correct
		size_t global_size = 0;
		
		for (size_t i=0; i < buffer.size() ;i++)
		{
			NetworkHeader *header = (NetworkHeader*) buffer[i]->getData(); 
			NetworkKVInfo *info	  = (NetworkKVInfo*)( buffer[i]->getData() + sizeof( NetworkHeader ) );
			
			size_t total_size=0;
			for (int hg = 0 ; hg < KV_NUM_HASHGROUPS ; hg++)
				total_size += info[hg].size;
			assert( total_size == header->info.size );
			global_size += total_size;
		}
		assert( global_size == info.size );
		
		
		// Get a buffer to be able to put all data in memory
		size_t file_size = FILE_TOTAL_HEADER_SIZE + info.size;	

		// Crearte the buffer
		Buffer *b = MemoryManager::shared()->newBuffer( "Creating file from network buffers" , file_size );
		
		// Global header of the file with magic number and format
		FileHeader fileHeader;
		fileHeader.init( );
		fileHeader.setInfo( info );	
		fileHeader.setFormat( queue_format );
		
		memcpy(b->getData(), &fileHeader, sizeof(FileHeader) );	

		// Vector with per-hash info
		FileKVInfo *file_info = (FileKVInfo*) (b->getData() + sizeof(fileHeader));
		
		// Global data and offset in the resulting buffer
		char *data = b->getData();
		size_t offset = FILE_TOTAL_HEADER_SIZE;	// Initial offset at the file outptu
		
		// Init the offset in each file
		for (size_t i = 0 ; i < buffer.size() ; i++)
			buffer[i]->skipRead( NETWORK_TOTAL_HEADER_SIZE );
				
		for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
		{
			file_info[i].kvs = 0;
			file_info[i].size = 0;
			
			// Fill data and sizes from all buffers
			for (size_t j = 0 ; j <  buffer.size() ; j++)
			{
				NetworkKVInfo *_sub_info = (NetworkKVInfo*) ( buffer[j]->getData() + sizeof( NetworkHeader ) );
				NetworkKVInfo sub_info = _sub_info[i];

				size_t read_size = buffer[j]->read( data + offset , sub_info.size );
				assert( read_size == sub_info.size);
				
				offset += sub_info.size;
				
				// Update information about this hash-group in the file buffer
				file_info[i].kvs += sub_info.kvs;
				file_info[i].size += sub_info.size;
			}
		}
		
		assert( file_size == offset );
		
		// Set the global size
		b->setSize(offset);
		
		// Return the new buffer with the content reordered
		return b;
		
	}
	
	
	std::string QueueuBufferVector::getStatus()
	{
		std::ostringstream o;
		o << "<" << buffer.size() << "/" << au::Format::string( size , "B" ) << ">";
		return o.str();
	}
	
	
}
