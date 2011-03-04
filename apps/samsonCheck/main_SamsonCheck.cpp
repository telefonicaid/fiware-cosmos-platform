
#include <iostream>
#include "CommandLine.h"			// au::CommandLine
#include "coding.h"					// ss::FormatHeader
#include "samson/KVFormat.h"		// ss::KVFormat
#include "ModulesManager.h"			// ss::ModulesManager

#include "SamsonSetup.h"			// ss::SamsonSetup
#include "MemoryManager.h"			// ss::MemoryManager
#include "SamsonSetup.h"			// ss::SamsonSetup

int logFd = -1;

//char* progName = (char*) "samsonSetup";


int main(int argc, const char *argv[])
{
	
	au::CommandLine cmdLine;
	cmdLine.set_flag_string("working", SAMSON_DEFAULT_WORKING_DIRECTORY);
	cmdLine.set_flag_boolean("clean");		// Clean shared memory areas
	cmdLine.parse(argc, argv);
	
	ss::SamsonSetup::load( cmdLine.get_flag_string("working") );
	ss::SamsonSetup *s = ss::SamsonSetup::shared();	// Load setup file and create main directories
	
	if ( cmdLine.get_flag_bool("clean" ) )
	{
		// Spetial command to clena shared memory areas
		
		std::cout << "Cleaning shared memory areas\n";
		
		ss::MemoryManager *mm = ss::MemoryManager::shared();
		
		for (int i = 0 ; i < ss::SamsonSetup::shared()->num_processes ; i++)
		{
			std::cout << "Removing shared memory " << i << " / " << ss::SamsonSetup::shared()->num_processes << std::endl;
			mm->removeSharedMemory(i);
		}
		
		std::cout << "Done!\n";
		exit(0);
		
	}
	
	
	
	std::cout << "\n";
	std::cout << "----------------------------------------------------\n";
	std::cout << "Current setup analysis\n";
	std::cout << "Working directory: " << cmdLine.get_flag_string("working") << "\n";
	std::cout << "----------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "** General setup:\n";
	std::cout << "Num processes: " << s->num_processes << "\n";
	std::cout << "\n";
	std::cout << "** Memory-system setup:\n";
	std::cout << "Memory: " << au::Format::string( s->memory ) << std::endl;
	std::cout << "Shared memory: " << ss::SamsonSetup::shared()->num_processes << " x " << au::Format::string( s->shared_memory_size_per_buffer ) << std::endl;
	std::cout << "\n";
	std::cout << "** Disk setup:\n";
	std::cout << "Num threads per devide:    " << s->num_io_threads_per_device << "\n";
	
	std::cout << "\n\n\n";
	std::cout << "Testing shared memory...\n";

	ss::MemoryManager *mm = ss::MemoryManager::shared();
	
	for (int i = 0 ; i < ss::SamsonSetup::shared()->num_processes ; i++)
	{
		std::cout << "Geting shared memory " << i << " / " << ss::SamsonSetup::shared()->num_processes << std::endl;
		ss::SharedMemoryItem *item =  mm->getSharedMemory(i);
		
		if( item )
		{
			std::cout << "OK\n";
			mm->freeSharedMemory( item );
		}
		else
		{
			std::cout << "ERROR\n";
		}
		
		
		
	}
	
	
	
	
	
	
	
}
