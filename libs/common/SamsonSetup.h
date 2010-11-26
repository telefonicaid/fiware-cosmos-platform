#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll

namespace ss {
	
	class SamsonSetup
	{
		
		SamsonSetup();
		
	public:

		static SamsonSetup *shared();

		// General setup
		int num_workers;
		int num_processes;
		
		// Disk management
		int max_open_files_per_device;

		//  Memory - System
		size_t memory;
		size_t shared_memory_size_per_buffer;
		int shared_memory_num_buffers;
		
		
	private:
		
		
		static size_t getUInt64(std::map<std::string,std::string> &items, std::string name , size_t defaultValue )
		{
			std::map<std::string,std::string>::iterator i =  items.find( name );
			if( i == items.end() )
				return defaultValue;
			else
				return atoll(i->second.c_str());
			
		}

		static int getInt(std::map<std::string,std::string> &items, std::string name , int defaultValue )
		{
			std::map<std::string,std::string>::iterator i =  items.find( name );
			if( i == items.end() )
				return defaultValue;
			else
				return atoi(i->second.c_str());
		}
		
	};

}

#endif
