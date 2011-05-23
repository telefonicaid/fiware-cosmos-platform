#include <stdio.h>
#include <iostream>				// std::cerr

#include "samsonDirectories.h"	// SAMSON_SETUP_FILE
#include "au/CommandLine.h"		// au::CommandLine
#include "SamsonSetup.h"		// Own interface
#include <errno.h>
#include "au/Format.h"             // au::Format

#include "logMsg.h"				// LM_X


#define SETUP_num_io_threads_per_device					"num_io_threads_per_device"
#define SETUP_DEFAULT_num_io_threads_per_device			1

#define SETUP_max_file_size								"max_file_size"
#define SETUP_DEFAULT_max_file_size						104857600	// 100Mb

#define SETUP_num_processes								"num_processes"
#define SETUP_DEFAULT_num_processes						2

#define SETUP_memory									"memory"
#ifdef __LP64__
#define SETUP_DEFAULT_memory							2147483648 // 2Gb
#else
#define SETUP_DEFAULT_memory							0x6fffffff // 1Gb
#endif

#define SETUP_shared_memory_size_per_buffer						"shm_size_per_buffer"
#define SETUP_DEFAULT_shared_memory_size_per_buffer				67108864	// 64 Mb

#define SETUP_DEFAULT_load_buffer_size							67108864	// 64 Mb

#define SETUP_num_paralell_outputs                              "num_paralell_outputs"
#define SETUP_DEFAULT_num_paralell_outputs                      2

#define SETUP_timeout_secs_isolatedProcess						"timeout_secs_isolatedProcess"
#define SETUP_DEFAULT_timeout_secs_isolatedProcess				300

namespace ss
{
	static SamsonSetup *samsonSetup = NULL;
	
	SamsonSetup *SamsonSetup::shared()
	{
		if( !samsonSetup)
			LM_X(1,("SamsonSetup not loaded. It should be initizalized at the begining of the main process"));
		
		return samsonSetup;
	}
	
    void destroy_samson_setup()
    {
        if( samsonSetup )
        {
            delete samsonSetup;
            samsonSetup = NULL;
        }
        
    }
    
	void SamsonSetup::load()
	{
		samsonSetup = new SamsonSetup( );
	}
	
	void SamsonSetup::load( std::string workingDirectory )
	{
		samsonSetup = new SamsonSetup( workingDirectory );
        
        atexit(destroy_samson_setup);
	}

	
	void SamsonSetup::createDirectory( std::string path )
	{
		
		if( mkdir(path.c_str()	, 0755) == -1 )
		{
			if( errno != EEXIST )
				LM_X(1,("Error creating directory %s", path.c_str()));
		}
	}
												   

	SamsonSetup::SamsonSetup( )
	{
		std::map<std::string,std::string> items;
		init( items );
		
	}
	
	SamsonSetup::SamsonSetup( std::string workingDirectory )
	{

		baseDirectory = workingDirectory;

		controllerDirectory = workingDirectory + "/controller";
		dataDirectory		= workingDirectory + "/data";
		modulesDirectory	= workingDirectory + "/modules";
		setupDirectory		= workingDirectory + "/etc";
		setupFile			= setupDirectory   + "/setup.txt";
		configDirectory			= workingDirectory + "/config";
				
		// Create directories if necessary
		createDirectory( workingDirectory );
		createDirectory( controllerDirectory );
		createDirectory( dataDirectory );
        createDirectory( modulesDirectory );        
		createDirectory( setupDirectory );			
		createDirectory( configDirectory );			
		
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
		memory
		   = getUInt64(
		   items, 
		   SETUP_memory ,
		   SETUP_DEFAULT_memory);

		shared_memory_size_per_buffer	= getUInt64( items, SETUP_shared_memory_size_per_buffer , SETUP_DEFAULT_shared_memory_size_per_buffer );
		
		// IsolatedProcess timeout
		timeout_secs_isolatedProcess	= getInt( items, SETUP_timeout_secs_isolatedProcess , SETUP_DEFAULT_timeout_secs_isolatedProcess );
		
		// Default value for other fields
		load_buffer_size = SETUP_DEFAULT_load_buffer_size;
		
		// Derived parameters
		num_paralell_outputs = getInt( items, SETUP_num_paralell_outputs , SETUP_DEFAULT_num_paralell_outputs );
		
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
		
		int max_num_paralell_outputs =  ssize_t( ssize_t(memory) - ssize_t(num_processes*shared_memory_size_per_buffer) ) / ssize_t(2*max_file_size);
		if( num_paralell_outputs > max_num_paralell_outputs )
		{
			LM_W(("Num of maximum paralell outputs is too high to the current memory setup. Review num_paralell_outputs in setup.txt file. Current value %d Max value %d (memory(%lu) - num_processes(%d)*shared_memory_size_per_buffer(%lu) ) / (2*max_file_size(%lu))", num_paralell_outputs , max_num_paralell_outputs, memory, num_processes, shared_memory_size_per_buffer, max_file_size ));
			return false;
		}
		else
		{
			LM_W(("Num of maximum paralell outputs is OK. Current value %d Max value %d (memory(%lu) - num_processes(%d)*shared_memory_size_per_buffer(%lu) ) / (2*max_file_size(%lu))", num_paralell_outputs , max_num_paralell_outputs, memory, num_processes, shared_memory_size_per_buffer, max_file_size ));
		}

		
		if ( num_paralell_outputs < 2 )
		{
			std::cerr << "Error in the memory setup. Please, review setup since the maximum number of outputs has to be at least 2\n";
			std::cerr << "Memory: " << au::Format::string( memory , "B" ) << "\n";
		    std::cerr << "Shared memory: " << au::Format::string( num_processes*shared_memory_size_per_buffer , "B" ) << "\n";
			std::cerr << "Max file size: " << au::Format::string( max_file_size , "B" ) << "\n";
			return false;
		}
		
		return true;
	}

    std::string SamsonSetup::dataFile( std::string filename )
    {
        return samsonSetup->dataDirectory + "/" + filename;
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
