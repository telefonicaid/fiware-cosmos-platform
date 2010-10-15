
#include <iostream>
#include "MemoryManager.h"
#include "Console.h"			// au::Console
#include "Buffer.h"				// ss:BUffer
#include "DiskManager.H"		// ss::DiskManager

class TestConsole : public au::Console
{
public:
	TestConsole() : au::Console( false )
	{
	}
	
	void evalCommand( std::string command ) 
	{
		au::CommandLine c;
		c.parse(command);
		
		if( c.get_num_arguments() == 0)
			return;
		
		
		if ( c.get_argument(0) == "set" )
		{
			if( c.get_num_arguments() < 2)
				return;
			
			ss::SharedMemoryItem *info = ss::MemoryManager::shared()->getSharedMemory( 0 );
			
			int value = atoi( c.get_argument(1).c_str() );;
			*((int*)info->data) = value;
			std::cout << "Set value OK";

		}

		if ( c.get_argument(0) == "get" )
		{
			
			ss::SharedMemoryItem *info = ss::MemoryManager::shared()->getSharedMemory( 0 );
			int v = *((int*)info->data);

			std::cout << "value " << v << "\n";
			
		}
		
		if ( c.get_argument(0) == "quit" )
		{
			exit(0);
		}
		
	}
	
		
};

int main( int arg , char *argv[] )
{
	ss::MemoryManager::shared();

	TestConsole c;
	c.run();
}
