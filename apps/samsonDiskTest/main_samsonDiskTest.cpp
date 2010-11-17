
#include <iostream>
#include "Console.h"				// au::Console
#include "DiskManager.h"			// ss::DiskManager
#include "DiskOperationsManager.h"	// ss::DiskOperationsManager
#include <set>						// std::set
#include "Lock.h"					// au::Lock
#include "CommandLine.h"			// au::CommandLine
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "Console.h"				// au::Console
#include "DataBuffer.h"				// ss::DataBuffer
#include <cstdlib>					// atoll
#include "samson/DataInstance.h"	// ss::systen::UInt
#include "DataBuffer.h"
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate



char* progName = (char*) "samsonDiskTest";



class DiskTest : public au::Console , public ss::DataBufferItemDelegate
{
public:
	
	ss::DataBuffer dataBuffer;
	
	DiskTest() : au::Console(false)
	{
	}

	virtual void evalCommand( std::string command )
	{
		
		au::CommandLine commandLine;
		
		commandLine.parse(command);
		
		if( commandLine.get_num_arguments() == 0)
			return;

		std::string mainCommand = commandLine.get_argument(0);
		
		if( mainCommand == "quit")
		{
			quit();
			return;
		}
		else if (mainCommand == "help" )
		{
			writeBlockOnConsole("Help\n Commands:\n\tnew_task task_id queues\n\tadd_buffer task_id queue size\n\tfinish_task task_id\n\tremove_task task_id\n\tmemory\n\tdataBuffer");
			return;
		}
		else if (mainCommand == "memory" )
		{
			writeBlockOnConsole( ss::MemoryManager::shared()->str() );
			return;
		}
		else if (mainCommand == "dataBuffer" )
		{
			writeBlockOnConsole( dataBuffer.str() );
			return;
		}
		else if (mainCommand == "finish_task" )
		{
			if( commandLine.get_num_arguments() < 2 )
			{
				writeErrorOnConsole("finish_task task_id");
				return;
			}
			
			size_t task_id = atoll( commandLine.get_argument(1).c_str() );

			dataBuffer.finishTask( task_id );
			
			writeOnConsole("OK");
			return;
			
		}
		else if (mainCommand == "remove_task" )
		{
			if( commandLine.get_num_arguments() < 2 )
			{
				writeErrorOnConsole("remove_task task_id");
				return;
			}
			
			size_t task_id = atoll( commandLine.get_argument(1).c_str() );
			
			dataBuffer.removeTask( task_id );
			
			writeOnConsole("OK");
			return;
			
		}
		
		writeOnConsole("Not implemented...");
	
	}
	
	// DataBufferItemDelegate
	
	void addFile(size_t task_id, std::string fileName , std::string queue , ss::FileKVInfo info)
	{
		std::ostringstream o;
		o << "Notification of a new file " << fileName << " for task " << task_id << " and queue " << queue << " " << info.str(); 
		writeOnConsole(o.str());
	}
	
	void finishDataBufferTask(size_t task_id)
	{
		std::ostringstream o;
		o << "Notification of finish task " << task_id;
		writeOnConsole(o.str());
	}
	
};


/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



int main( int arg , const char *argv[] )
{
	DiskTest test;
	test.run();
	return 0;
}



// Old demo to save static files to disk


int _main( int arg , const char *argv[] )
{
	// Access the disk manager

	au::CommandLine cmdLine;
	cmdLine.set_flag_uint64("size", 10000000);	// By default 100 Mbytes
	cmdLine.set_flag_int("files", 100);			// By default 100 files
	cmdLine.parse(arg , argv);
	
	// get the size of the individual files
	size_t size = cmdLine.get_flag_uint64("size");
	int files = cmdLine.get_flag_int("files");

	if( cmdLine.get_num_arguments() == 1 )
		std::cout << "Please specify one or more directories where to write files. /tmpt/ is selected by default\n";
	
	// Vector with all directories to test
	std::vector<std::string> directories;
	if( cmdLine.get_num_arguments() > 1)
	{
		for (int i =  1 ; i < cmdLine.get_num_arguments() ; i++)
			directories.push_back( cmdLine.get_argument(i) );
	}
	else
		directories.push_back( "/tmp/" );
	
	
	// Create a buffer with the rigth size
	ss::Buffer *buffer  = ss::MemoryManager::shared()->newBuffer( size );
	ss::Buffer *buffer2 = ss::MemoryManager::shared()->newBuffer( size );
	
	// fill data...
	char *data  = buffer->getData();
	for (size_t i = 0 ;  i < size ; i++)
		data[i] = rand()%100;

	// Manager to run multiple io operations
	ss::DiskOperationsManager operationsManager;
	
	// Add some operations for testing writing...
	for (size_t f = 0 ; f < directories.size() ; f++)
	{
		for (int i = 0 ; i < files ; i++)
		{
			std::ostringstream fileName;
			fileName << directories[f] << "file_" << i;
			
			operationsManager.write( buffer ,  fileName.str() );
		}
	}

	operationsManager.waitUntilComplete();
	
	// Add some operations for testing reading...
	for (size_t f = 0 ; f < directories.size() ; f++)
	{
		for (int i = 0 ; i < files ; i++)
		{
			std::ostringstream fileName;
			fileName << directories[f] << "file_" << i;
			
			operationsManager.read( buffer2, fileName.str() , 0 ,  size );
		}
	}
	operationsManager.waitUntilComplete();
	
	return 0;
}
