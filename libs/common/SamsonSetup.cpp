#include <stdio.h>
#include <iostream>				// std::cerr

#include "samsonDirectories.h"	// SAMSON_SETUP_FILE
#include "CommandLine.h"		// au::CommandLine
#include "SamsonSetup.h"		// Own interface


#define SETUP_max_open_files_per_device					"max_open_files_per_device"
#define SETUP_DEFAULT_max_open_files_per_device			100

#define SETUP_num_workers								"num_workers"	
#define SETUP_DEFAULT_num_workers						2	

#define SETUP_num_processes								"num_processes"
#define SETUP_DEFAULT_num_processes						2

#define SETUP_memory									"memory"
#define SETUP_DEFAULT_memory							2147483648	// 2Gb

#define SETUP_shared_memory_size_per_buffer						"shm_size_per_buffer"
#define SETUP_DEFAULT_shared_memory_size_per_buffer				67108864	// 64 Mb

#define SETUP_shared_memory_num_buffers							"shm_num_buffers"
#define SETUP_DEFAULT_shared_memory_num_buffers					12


namespace ss
{
	static SamsonSetup *samsonSetup = NULL;
	
	SamsonSetup *SamsonSetup::shared()
	{
		if( !samsonSetup)
			samsonSetup = new SamsonSetup();
		return samsonSetup;
	}
	
	SamsonSetup::SamsonSetup()
	{
		
		FILE *file = fopen( SAMSON_SETUP_FILE  ,"r");
		if (!file)
		{
			std::cerr << "Config file "<< SAMSON_SETUP_FILE <<" not found\n";
			return;
		}

		char line[2000];
		std::map<std::string,std::string> items;
		while( fgets(line, sizeof(line), file))
		{
			au::CommandLine c;
			c.parse(line);
			
			if( c.get_num_arguments() == 0 )
				continue;
			
			// Skip comments
			std::string mainCommand = c.get_argument(0);
			if( mainCommand[0] == '#' )
				continue;
			
			if (c.get_num_arguments() >= 2)	
			{
				std::string name = c.get_argument(0);
				std::string value =  c.get_argument(1);
				
				std::map<std::string,std::string>::iterator i = items.find( name );
				if( i!= items.end() )
					items.erase( i );
				items.insert( std::pair<std::string , std::string>( name ,value ) );
				
			}

		}
		
		// General setup
		num_workers		= getInt( items, SETUP_num_workers , SETUP_DEFAULT_num_workers );
		num_processes	= getInt( items, SETUP_num_processes , SETUP_DEFAULT_num_processes );
		
		// Disk management
		max_open_files_per_device = getInt( items, SETUP_max_open_files_per_device , SETUP_DEFAULT_max_open_files_per_device );
		
		//  Memory - System
		memory							= getUInt64( items, SETUP_memory , SETUP_DEFAULT_max_open_files_per_device );
		shared_memory_size_per_buffer	= getUInt64( items, SETUP_shared_memory_size_per_buffer , SETUP_DEFAULT_shared_memory_size_per_buffer );
		shared_memory_num_buffers		= getInt( items, SETUP_shared_memory_num_buffers , SETUP_DEFAULT_shared_memory_num_buffers );
		
	}
	
	
}
