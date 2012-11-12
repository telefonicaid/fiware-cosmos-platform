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
#ifndef _H_TXT_FILE_SET
#define _H_TXT_FILE_SET

#include "au/file.h"

namespace samson {

	class Delilah;
	class Buffer;
	
    class DataSource
    {
        
    public:
        
        virtual ~DataSource(){}
        
		virtual bool isFinish()=0;
		virtual int fill( engine::Buffer *b )=0;
        virtual size_t getTotalSize()=0;
        
    };
    
    
    class BufferDataSource : public DataSource
    {
        char *data;
        size_t size;
        size_t pos;
        
    public:
        
        BufferDataSource( char *_data , size_t _size )
        {
            size = _size;
            
            // Alloc and copy data
            data = (char*) malloc( size );
            memcpy(data, _data, size);            
            
            pos = 0;
        }
        
        ~BufferDataSource()
        {
            freeBuffer();
        }
        
        void freeBuffer()
        {
            if( data )
                free( data );
            data = NULL;
        }
        
		bool isFinish()
        {
            return (pos>=size);
        }
		
        int fill( engine::Buffer *b )
        {
            size_t available_size = b->getAvailableWrite();
            
            if( available_size > (size-pos) )
            {
                b->write( &data[pos] , size-pos );
                pos = size;
                
                // We can already free this buffer
                freeBuffer();
                
                return 0;
            }
            else
            {
                b->write( &data[pos] , available_size );
                pos += available_size;
            }
            
            return 0;
            
        }
        
        size_t getTotalSize()
        {
            return size;
        }
        
        
    };
    
	
	class TXTFileSet : public DataSource
	{
		
		std::vector<std::string> fileNames;		// List of files to read
		std::ifstream inputStream;				// Stream of the file we are reading	
		
		int file;
		bool finish;
		
		char *previousBuffer;               // Part of the previous buffer waiting for the next read
		size_t previousBufferSize;
		
		std::vector<std::string> failedFiles;		// List of files that could not be uploaded		
        
	public:
		
		TXTFileSet( std::vector<std::string> &_fileNames )
		{
			fileNames = _fileNames;
			
			previousBufferSize = 0;
			previousBuffer = NULL;
			
			finish = false;
			file = 0;	

            
			openNextFile();
		}
		
		void openNextFile()
		{
			while ( !inputStream.is_open() ) 
			{
				
				if( file >= (int)fileNames.size() )
				{
					finish = true;
					return;
				}
				
				inputStream.open( fileNames[file].c_str() );
				if( !inputStream.is_open() )
					failedFiles.push_back( fileNames[file] );	// Add to the list of failed files
				file++;
				
			}
		}
		
		
		bool isFinish()
		{
			return finish;
		}
		
		// Read as much as possible breaking in lines
		int fill( engine::Buffer *b );
		
		std::vector<std::string> getFailedFiles()
		{
			return failedFiles;
		}
		
        
        std::string getStatus()
        {
            std::ostringstream output;
            output << "TXTFile: ";
            if( finish )
                output << " finished";
            else
                output << " running: " << fileNames.size() << " pending files and " << failedFiles.size() << " failed files";
            return  output.str();
        }
        
        
        size_t getTotalSize()
        {
            size_t totalSize = 0;
            
            // Compute the total size for all the files
            for ( size_t i =  0 ; i < fileNames.size() ; i++)
                totalSize += au::sizeOfFile( fileNames[i] );

            return totalSize;
        }
        
		
	};
	
	
}

#endif
