#include <stdio.h>
#include <iostream>				// std::cerr

#include "samsonDirectories.h"	// SAMSON_SETUP_FILE
#include "CommandLine.h"		// au::CommandLine
#include "SamsonSetup.h"		// Own interface
#include <assert.h>				// assert(.)
#include <errno.h>

#define SETUP_num_io_threads_per_device					"num_io_threads_per_device"
#define SETUP_DEFAULT_num_io_threads_per_device			1

#define SETUP_max_file_size								"max_file_size"
#define SETUP_DEFAULT_max_file_size						209715200	// 200Mb

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
		{
			std::cerr << "Error: Setup not loaded\n";
			assert(false);
		}
		
		return samsonSetup;
	}
	
	void SamsonSetup::load()
	{
		samsonSetup = new SamsonSetup( );
	}
	
	void SamsonSetup::load( std::string workingDirectory )
	{
		samsonSetup = new SamsonSetup( workingDirectory );
	}

	
	void SamsonSetup::createDirectory( std::string path )
	{
		
		if( mkdir(path.c_str()	, 0755) == -1 )
		{
			if( errno != EEXIST )
			{
				std::cout << "Error creating directory " << path << "\n";
				assert(false);
			}
		}
	}
												   

	SamsonSetup::SamsonSetup(  )
	{
		std::map<std::string,std::string> items;
		init( items );
		
	}

	
	
	SamsonSetup::SamsonSetup( std::string workingDirectory )
	{

		baseDirectory = workingDirectory;
		// TODO: Remove the last / if necessary

		controllerDirectory = workingDirectory + "/controller";
		dataDirectory		= workingDirectory + "/data";
		modulesDirectory	= workingDirectory + "/modules";
		setupDirectory		= workingDirectory + "/etc";
		setupFile			= setupDirectory + "/setup.txt";
				
		// Create directories if necessary
		
		createDirectory( workingDirectory );
		createDirectory(controllerDirectory);
		createDirectory(dataDirectory);
        createDirectory(modulesDirectory);        
		createDirectory(setupDirectory);			
		

		
		// Load values from file ( if exist )
		
		std::map<std::string,std::string> items;
		
		FILE *file = fopen( setupFile.c_str()  ,"r");
		if (!file)
		{
			std::cerr << "Warning: Setup file "<< setupFile <<" not found\n";
		}
		else
		{
			
			char line[2000];
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
		
			fclose(file);
		}
		
		init( items );
		
	}
	
	void SamsonSetup::init( std::map<std::string,std::string> &items )
	{
		
		// General setup
		num_processes	= getInt( items, SETUP_num_processes , SETUP_DEFAULT_num_processes );
		
		// Disk management
		num_io_threads_per_device = getInt( items, SETUP_num_io_threads_per_device , SETUP_DEFAULT_num_io_threads_per_device );
		max_file_size = getUInt64( items, SETUP_max_file_size, SETUP_DEFAULT_max_file_size);
		
		//  Memory - System
		memory							= getUInt64( items, SETUP_memory , SETUP_DEFAULT_memory );
		shared_memory_size_per_buffer	= getUInt64( items, SETUP_shared_memory_size_per_buffer , SETUP_DEFAULT_shared_memory_size_per_buffer );
		shared_memory_num_buffers		= getInt( items, SETUP_shared_memory_num_buffers , SETUP_DEFAULT_shared_memory_num_buffers );
		
		if( !check() )
		{
			std::cerr << "Error in setup file " << setupFile << "\n";
			exit(0);
		}
		
	}

	
	bool SamsonSetup::check()
	{
		/*
		if ( memory < 1024*1024*1024 )
		{
			std::cerr << "Memory should be at least 1Gb\n";
			return false;
		}
		 */
		
		if( ( shared_memory_num_buffers * shared_memory_size_per_buffer ) > (0.5 *memory) )
		{
			std::cerr << "Memory should be at least double of total shared Memory\n";
			return false;
		}

#if 0
		if ( shared_memory_size_per_buffer < 64*1024*1024)
		{
			std::cerr << "Shared Memory Size should be at least 64Mb\n";
			return false;
		}
#endif	
		if( shared_memory_num_buffers < 2*num_processes )
		{
			std::cerr << "Number of shared memory buffers should be at least 2 times the number of process.\n";
			std::cerr << "Recomended value: num_buffers = 3 * num_process\n";
			return false;
		}
		if ( num_io_threads_per_device < 1)
		{
			std::cerr << "Minimum number of threads per device 1.\n";
			return false;
		}
		if ( num_io_threads_per_device > 10)
		{
			std::cerr << "Maximum number of threads per device 10.\n";
			return false;
		}
		
		return true;
	}


	
	size_t SamsonSetup::getUInt64(std::map<std::string,std::string> &items, std::string name , size_t defaultValue )
	{
		std::map<std::string,std::string>::iterator i =  items.find( name );
		if( i == items.end() )
			return defaultValue;
		else
			return atoll(i->second.c_str());
		
	}
	
	int SamsonSetup::getInt(std::map<std::string,std::string> &items, std::string name , int defaultValue )
	{
		std::map<std::string,std::string>::iterator i =  items.find( name );
		if( i == items.end() )
			return defaultValue;
		else
			return atoi(i->second.c_str());
	}	
	
}
