#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll
#include <sys/stat.h>		// mkdir

namespace ss {
	
	class SamsonSetup
	{
		
		SamsonSetup();
		
	public:

		static SamsonSetup *shared();

		// General setup
		int num_processes;
		
		// Disk management
		int num_io_threads_per_device;

		//  Memory - System
		size_t memory;
		size_t shared_memory_size_per_buffer;
		int shared_memory_num_buffers;
		
		// Max size of kv files
		size_t max_file_size;
		
	private:
		
		static size_t getUInt64(std::map<std::string,std::string> &items, std::string name , size_t defaultValue );
		static int getInt(std::map<std::string,std::string> &items, std::string name , int defaultValue );
		
		// Check if everything is ok. Exit if not
		bool check();
		
	};

}

#endif
