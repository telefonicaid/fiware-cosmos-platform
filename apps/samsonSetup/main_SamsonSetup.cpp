
#include <iostream>
#include "CommandLine.h"			// au::CommandLine
#include "coding.h"					// ss::FormatHeader
#include "samson/KVFormat.h"		// ss::KVFormat
#include "ModulesManager.h"			// ss::ModulesManager

#include "SamsonSetup.h"			// ss::SamsonSetup
#include "MemoryManager.h"			// ss::MemoryManager

int logFd = -1;

char* progName = (char*) "samsonSetup";


int main(int argc, const char *argv[])
{
	ss::SamsonSetup *s = ss::SamsonSetup::shared();	// Load setup file and create main directories
	
	std::cout << "\n";
	std::cout << "--------------------------------\n";
	std::cout << "Current setup analysis\n";
	std::cout << "--------------------------------\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "** General setup:\n";
	std::cout << "Num workers: " << s->num_workers << "\n";
	std::cout << "Num processes: " << s->num_processes << "\n";
	std::cout << "\n";
	std::cout << "** Memory-system setup:\n";
	std::cout << "Memory: " << au::Format::string( s->memory ) << std::endl;
	std::cout << "Shared memory: " << s->shared_memory_num_buffers << " x " << au::Format::string( s->shared_memory_size_per_buffer ) << std::endl;
	std::cout << "\n";
	std::cout << "** Disk setup:\n";
	std::cout << "Max open files per devide:    " << s->max_open_files_per_device << "\n";
	
	

	std::cout << "\n\n\n";
	std::cout << "Testing shared memory...\n";

	ss::MemoryManager *mm = ss::MemoryManager::shared();
	
	for (int i = 0 ; i < s->shared_memory_num_buffers ; i++)
	{
		std::cout << "Geting shared memory " << i << " / " << s->shared_memory_num_buffers << std::endl;
		ss::SharedMemoryItem *item =  mm->getSharedMemory(i);
		
		if( item )
			std::cout << "OK\n";
		else
		{
			std::cout << "ERROR\n";
		}
		
	}
	
	
	
	
	
	
	
}
