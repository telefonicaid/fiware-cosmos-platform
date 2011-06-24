#include "engine/DiskOperation.h"		// Own interface
#include "logMsg/logMsg.h"                     // LM_T
#include "logMsg/traceLevels.h"                // LmtDisk
#include "engine/Engine.h"				// engine::Engine
#include "engine/DiskManager.h"         // engine::DiskManager
#include "engine/ReadFile.h"            // engine::ReadFile

namespace engine {
	
	
	void* diskOperationRunInBackGround(void * _diskOperation)
	{
		
		DiskOperation *diskOperation = (DiskOperation*) _diskOperation;
		diskOperation->run();
		
		return NULL;
	}
	
#pragma mark ----

	
	
	DiskOperation::DiskOperation( )
	{
		// Some default values
		buffer = NULL;
		read_buffer = NULL;
		
	}

    DiskOperation::~DiskOperation()
    {
    }
	
    
	DiskOperation* DiskOperation::newReadOperation( char *data , std::string fileName , size_t offset , size_t size , size_t _listenerId  )
	{
		
		DiskOperation *o = new DiskOperation();
		
		o->fileName = fileName;
		o->type = DiskOperation::read;
		o->read_buffer = data;
		o->size = size;
		o->offset = offset;
		o->setDevice();
        o->listenerId = _listenerId;
		
		return o;
	}
	
	DiskOperation * DiskOperation::newReadOperation( std::string fileName , size_t offset , size_t size ,  SimpleBuffer simpleBuffer , size_t _listenerId)
	{
		
		DiskOperation *o = new DiskOperation();
		
		o->fileName = fileName;
		o->type = DiskOperation::read;
		o->read_buffer = simpleBuffer.getData();
		o->size = size;
		o->offset = offset;
		o->setDevice();
        o->listenerId = _listenerId;
		
		return o;
	}
	
	
	
	DiskOperation* DiskOperation::newWriteOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId )
	{
		DiskOperation *o = new DiskOperation();
		
		o->fileName = fileName;
		o->type = DiskOperation::write;
		o->buffer = buffer;
		o->size = buffer->getSize();
		o->offset = 0;
		o->setDevice();
        o->listenerId = _listenerId;
		
		return o;
	}

	
	DiskOperation* DiskOperation::newAppendOperation( Buffer* buffer ,  std::string fileName, size_t _listenerId )
	{
		DiskOperation *o = new DiskOperation();
		
		o->fileName = fileName;
		o->type = DiskOperation::append;
		o->buffer = buffer;
		o->size = buffer->getSize();
		o->offset = 0;
		o->setDevice();
        o->listenerId = _listenerId;
		
		return o;
	}	
    
	DiskOperation* DiskOperation::newRemoveOperation( std::string fileName , size_t _listenerId )
	{
		DiskOperation *o = new DiskOperation();
		
		o->fileName = fileName;
		o->type = DiskOperation::remove;
		o->setDevice();
        o->size = 0;
        o->listenerId = _listenerId;
        
		return o;
	}
	
	
	std::string DiskOperation::directoryPath( std::string path )
	{
		size_t pos = path.find_last_of("/");
		
		if ( pos == std::string::npos)
			return ".";
		else
			return path.substr( 0 , pos );
	}
	
	bool DiskOperation::setDevice( )
	{
		// Make sure we only use one device
		st_dev = 0;
		return true;
		
		
		struct ::stat info;
		stat(fileName.c_str(), &info);
		
		if( S_ISREG(info.st_mode) )
		{
			st_dev = info.st_dev;	// Get the devide of the inode 
			return true;
		}
		
		// File currently does not exist ( it wil be writed )
		if( type != DiskOperation::write )
			return false;
		
		// Get the path of the directory
		std::string directory = directoryPath( fileName );
		stat(directory.c_str(), &info);
		
		if( S_ISDIR(info.st_mode) )
		{
			st_dev = info.st_dev;	// Get the devide of the inode 
			st_dev = 0;// Force all files to be process as the same inode
			return true;
		}
		
		// Something wrong...
		return false;
	}
	
	
	std::string DiskOperation::getDescription()
	{
		std::ostringstream o;
		
		switch (type) {
			case write:
				o << "Write to file: '" << fileName << "' Size:" << au::Format::string(size,"B");
				break;
			case append:
				o << "Append to file: '" << fileName << "' Size:" << au::Format::string(size,"B");
				break;
			case read:
				o << "Read from file: '" << fileName << "' Size:" << au::Format::string(size,"B") << " ["<< size << "B] Offset:" << offset;
				break;
			case remove:
				o << "Remove file: '" << fileName << "'";
				break;
		}
		
		
		return o.str();		
	}

	std::string DiskOperation::getShortDescription()
	{
		std::ostringstream o;
		
		switch (type) {
			case write:
				o << "W:" << au::Format::string( size );
				break;
			case append:
				o << "A:" << au::Format::string( size );
				break;
			case read:
				o << "R:" << au::Format::string( size );
				break;
			case remove:
				o << "X";
				break;
		}
		
		
		return o.str();		
	}
	
	void DiskOperation::setError( std::string message )
	{
		
		std::ostringstream o;
		o << message << " ( " << getDescription() << " )";
		error.set( o.str() );
	}
	
	

	void DiskOperation::runInBackGround()
	{
		pthread_create(&t, 0, diskOperationRunInBackGround, this);
	}

	
	void DiskOperation::run(  )
	{
		
		LM_T( LmtDisk , ("DiskManager: Running operation %s", getDescription().c_str() ));
		
		struct timeval start,stop;
		gettimeofday(&start, NULL);
		
		if( type == DiskOperation::write )
		{
			// Create a new file
			
			
			LM_T( LmtDisk , ("DiskManager: Opening file %s to write", fileName.c_str() ));
			FILE *file = fopen( fileName.c_str() , "w" );
			if ( !file )
				setError("Error opening file");
			else
			{
				if( size > 0 )
                {
                    if( fwrite(buffer->getData(), size, 1 , file) == 1 )
                    {
                        fflush(file);
                        gettimeofday(&stop, NULL);
                        operation_time = DiskStatistics::timevaldiff( &start , &stop);
                    }
                    else
                        setError("Error writing data to the file");
                }
                else
                    operation_time = 0;

			}
			
			fclose(file);
		}

		if( type == DiskOperation::append )
		{
			// Create a new file
			
			
			LM_T( LmtDisk , ("DiskManager: Opening file %s to write", fileName.c_str() ));
			FILE *file = fopen( fileName.c_str() , "a" );
			if ( !file )
				setError("Error opening file");
			else
			{
				if( size > 0 )
                {
                    if( fwrite(buffer->getData(), size, 1 , file) == 1 )
                    {
                        fflush(file);
                        gettimeofday(&stop, NULL);
                        operation_time = DiskStatistics::timevaldiff( &start , &stop);
                    }
                    else
                        setError("Error writing data to the file");
                }
                else
                    operation_time = 0;
                
			}
			
			fclose(file);
		}
        
        
		if( type == DiskOperation::read )
		{

			// Get the Read file from the Manager
            ReadFile *rf = diskManager->fileManager.getReadFile( fileName );
            
            LM_T( LmtDisk , ("DiskManager: Opening file %s to read", fileName.c_str() ));
			
			if( !rf->isValid() )
				setError( "Internal error: Not valid read file" );
			else
			{
                if( rf->seek( offset ) )
                    setError( au::Format::string("Error while seeking data from file %s" , fileName.c_str()));

                
                if( rf->read(read_buffer, size ) )
                    setError( au::Format::string("Error while reading data from file %s" , fileName.c_str()));
                
			}
            
			/*
			LM_T( LmtDisk , ("DiskManager: Opening file %s to read", fileName.c_str() ));
			
			FILE *file = fopen(fileName.c_str() , "r" );
			if( !file )
				setError("Error opening file");
			else
			{
				if( fseek(file, offset, SEEK_SET) != 0)
					setError("Error in fseek operation");
				else
				{
                    if(size > 0 )
                    {
                        if ( fread(read_buffer, size, 1, file) == 1 )
                        {
                            gettimeofday(&stop, NULL);
                            LM_TODO(("Fix statistics using Engine"));
                            operation_time = DiskStatistics::timevaldiff( &start , &stop);
                        }
                        else
                            setError("Error while reading data from file");
                    }
                    else
                        operation_time = 0;
				}
				
				fclose(file);
			}
             */
		}
		
		if( type == DiskOperation::remove)
		{
			
			LM_T( LmtDisk , ("DiskManager: Removing file %s to read", fileName.c_str() ));
			
			// Remove the file
			int c = ::remove( fileName.c_str() );
			if( c != 0 )
				setError("Error while removing file");
		}
        

		// If write operation, destroy the buffer ( only in write and append operations )
        if( environment.get( destroy_buffer_after_write , "yes") == "yes" )
            destroyBuffer();
		
		// Notify to the engine
		diskManager->finishDiskOperation( this );
        
	}	
	
	void DiskOperation::destroyBuffer()
	{
        if( buffer )
        {
            engine::MemoryManager::shared()->destroyBuffer(buffer);
            buffer = NULL;
        }
	}
	
    bool DiskOperation::compare( DiskOperation *operation )
    {
        // Priority to append and write operations
        if( ( type == append ) && ( type == write ) )
            if( ( operation->type != append ) && ( operation->type != write ) )
                return true;
        
        return false;
    }

	
}
