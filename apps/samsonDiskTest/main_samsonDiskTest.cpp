
#include <iostream>
#include "Console.h"				// au::Console
#include "DiskManager.h"			// ss::DiskManager
#include "DiskOperationsManager.h"	// ss::DiskOperationsManager
#include <set>						// std::set
#include "Lock.h"					// au::Lock
#include "CommandLine.h"			// au::CommandLine
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager

//#define DIR1  "/Volumes/UNTITLED 1/"
//#define DIR2  "/tmp/"

int main( int arg , const char *argv[] )
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
	
	
	
	
	
}
