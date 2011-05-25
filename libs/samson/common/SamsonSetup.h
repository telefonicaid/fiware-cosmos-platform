#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll
#include <sys/stat.h>		// mkdir

#define SAMSON_CONTROLLER_DEFAULT_PORT		1234
#define SAMSON_WORKER_DEFAULT_PORT			1235
#define SAMSON_DEFAULT_WORKING_DIRECTORY	"/opt/samson"

namespace samson {
	
	class SamsonSetup
	{
		
		SamsonSetup( );
		SamsonSetup( std::string workingDirectory );
		
	public:

		static SamsonSetup *shared();
		static void load(  );
		static void load( std::string workingDirectory );
		static void createDirectory( std::string path);

		// Directories
		std::string baseDirectory;
		std::string controllerDirectory;
		std::string dataDirectory;
		std::string blocksDirectory;
		std::string modulesDirectory;
		std::string setupDirectory;
		std::string setupFile;
		std::string configDirectory;
		
		// General setup
		int num_processes;
		
		// Disk management
		int num_io_threads_per_device;

		//  Memory - System
		size_t memory;
		size_t shared_memory_size_per_buffer;
		
		// Max size of kv files
		size_t max_file_size;
		
		// Load Data suze
		size_t load_buffer_size;
		
		// IsolatedProcess timeout
		int timeout_secs_isolatedProcess;
		
		// DERIVED PARAMETERS
		// --------------------------------------------
		
		int num_paralell_outputs;	// Maximum number of outputs for all the working operations
        
        static std::string dataFile( std::string filename );
		
	private:
		
		static size_t getUInt64(std::map<std::string,std::string> &items, std::string name , size_t defaultValue );
		static int getInt(std::map<std::string,std::string> &items, std::string name , int defaultValue );
		
		// Check if everything is ok. Exit if not
		bool check();

		// Common function to init variables
		void init( std::map<std::string,std::string> &items );
		
	};

}

#endif
