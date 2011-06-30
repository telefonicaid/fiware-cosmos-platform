/* ****************************************************************************
 *
 * FILE                     Buffer.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_BUFFER
#define _H_BUFFER

#include <cstring>			// size_t
#include <cstdlib>			// malloc, ...
#include <fstream>			// std::ifstream
#include "engine/SimpleBuffer.h"        // engine::SimpleBuffer
#include "engine/Object.h"  // engine::EngineNotificationObject

namespace engine {
	
	/**
	 Buffer class to hold data managed by MemoryManager
	 */
	
	class Buffer 
	{

	public:
		
        // Tag identifying the type of buffer ( for better control )
        int tag;        
        
	private:
		
		char * _data;			// Buffer of data
		size_t _max_size;		// Maximum size of this buffer
		std::string _name;		// Internal name for debugging
		
		/**
		 Current size used in this buffer 
		 This is the one that should be used when transmitting the buffer across the network
		 This variable is updated with calls to "write"
		 */
		
		size_t _size;			

		/**
		 Internal variable used for the read process
		 */
		
		size_t _offset;
				
	private:
		
		// Private constructor/destructors since it can be only access by MemoryManager
		
		Buffer( std::string name ,   size_t max_size, int _tag );
		~Buffer();
		
		void free();

		friend class MemoryManager;
		
	public:
		
		// Main functions
		// ------------------------------------------
		
		// Get the maximum size of the buffer
		size_t getMaxSize();
		
		// Get used size of this buffer ( not necessary the maximum )
		size_t getSize();
		
		// Get a description of the buffer
		std::string str();
		
		// Writing content to the buffer
		// ------------------------------------------
		
		/**
		 Function to write content updating the size variable coherently
		 If it is not possible to write the entire block, it return false
		 So, it never try to write less than input_size bytes
		 */
		
		bool write( char * input_buffer , size_t input_size );
		
		// Skip some space without writing anything
		bool skipWrite( size_t size );
		
		// Write on the buffer the maximum possible ammount of data
		void write( std::ifstream &inputStream );
		
		// Get available space to write with "write call"
		size_t getAvailableWrite();
		
		// Reading content from the buffer
		// ------------------------------------------
		
		// Skip some space without reading
		size_t skipRead( size_t size);
		
		// Read command
		size_t read( char *output_buffer, size_t output_size);
		
		// Get pending bytes to be read
		size_t getSizePendingRead();
		
		// Manual manipulation of data
		// ------------------------------------------
		
		// Get a pointer to the data space
		char *getData();
		
		// Set used size manually
		void setSize( size_t size );

		// Interface with SimpleBuffer
		// ------------------------------------------
		
		SimpleBuffer getSimpleBuffer();
		SimpleBuffer getSimpleBufferAtOffset(size_t offset);
		
		// Spetial functions
		// ------------------------------------------
		
		// Remove the last characters of an unfinished line and put them in buffer.
		size_t removeLastUnfinishedLine( char * buffer );

		
    public:
        
        
        // Public identifiers to be removed
        
        int worker;     // Identifier of the worker
        int hg_set;     // Identifier of ther hash_group-set
        bool finish;    // Flag ot the finish hash-group-set
        
	};

}

#endif
