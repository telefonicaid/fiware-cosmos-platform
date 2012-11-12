/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
 *
 * FILE            Buffer.h
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Buffer is a memory region allocated and controlled by MemoryManager
 * It can be requested and used by any element in the application
 * It can only be destroyed calling "destroyBuffer" at MemoryMamager
 * It comes with some usufull function to interact with its contect
 *
 * ****************************************************************************/

#ifndef _H_BUFFER
#define _H_BUFFER

#include <cstring>			// size_t
#include <cstdlib>			// malloc, ...
#include <fstream>			// std::ifstream

#include "au/mutex/Token.h"
#include "au/Object.h"
#include "engine/SimpleBuffer.h"        // engine::SimpleBuffer
#include "engine/Object.h"  // engine::EngineNotificationObject


namespace engine 
{
    
    /**
     Buffer class to hold data managed by MemoryManager
     */
    
    class Buffer : public au::Object
    {
        
        char * _data;			// Buffer of data
        size_t _max_size;		// Maximum size of this buffer
        
        std::string _name;		// Internal name for debugging
        std::string _type;      // Identifier of groups of blocks
        
        /**
         Current size used in this buffer 
         This is the one that should be used when transmitting the buffer across the network
         This variable is updated with calls to "write" or set manually in setSize
         */
        
        size_t _size;			
        
        /**
         Internal variable used for the read process
         */
        
        size_t _offset;
        
        // Private constructor/destructors since it can be only access by MemoryManager
        Buffer( std::string name , std::string type , size_t max_size );
        ~Buffer();
        
        // Free internal buffer
        void free();
        
        
        // Managed from Memory manager class
        friend class MemoryManager;
        
    public:

        // au::Object implementtion
        void self_destroy();

        // Get the maximum size of the buffer
        size_t getMaxSize();
        
        // Get used size of this buffer ( not necessary the maximum )
        size_t getSize();
        
        // Get internal name for debuggin
        std::string getName();
        
        // Get internal type for debuggin
        std::string getType();
        
        // Get a description of the buffer ( debugging )
        std::string str();
        
        // Set internal name and type for debuggin
        void setNameAndType( std::string name , std::string type );
        
        // Add string to the internal name
        void addToName( std::string description );
                
        // Write content to this buffer
        bool write( char * input_buffer , size_t input_size );
        
        // Skip some space without writing anything ( usefull to fix-size headers )
        bool skipWrite( size_t size );
        
        // Write on the buffer the maximum possible ammount of data
        void write( std::ifstream &inputStream );
        
        // Get available space to write with "write call"
        size_t getAvailableWrite();
        
        // Reading content from the buffer
        // ------------------------------------------
        
        // Skip some space without reading
        size_t skipRead( size_t size );
        
        // Read command
        size_t read( char *output_buffer, size_t output_size);
        
        // Get pending bytes to be read
        size_t getSizePendingRead();
        
        // Manual manipulation of data
        // ------------------------------------------
        
        // Get a pointer to the data full space ( not related with write calls )
        char *getData();
        
        // Set used size manually
        void setSize( size_t size );
        
        // Interface with SimpleBuffer ( simplified view of the internal buffer )
        // ------------------------------------------
        
        SimpleBuffer getSimpleBuffer();
        SimpleBuffer getSimpleBufferAtOffset(size_t offset);
        
        // Spetial functions
        // ------------------------------------------
        
        // Remove the last characters of an unfinished line and put them in buffer.
        int removeLastUnfinishedLine( char *& buffer , size_t& buffer_size);
        
        
        // Public identifiers to be removed
        
        int worker;     // Identifier of the worker
        int hg_set;     // Identifier of ther hash_group-set
        bool finish;    // Flag ot the finish hash-group-set
        
    };
    
    
}

#endif
