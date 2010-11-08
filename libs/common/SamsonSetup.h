#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll

#define SETUP_max_open_files_per_device		"max_open_files_per_device"

#define SETUP_num_workers					"num_workers"	

#define SETUP_shm_size_per_process			"shm_size_per_core"
#define SETUP_num_processes					"num_process"
#define SETUP_memory						"memory"

namespace ss {
	
	class SamsonSetup
	{
		
		SamsonSetup();

		std::map<std::string,std::string> items;
		
	public:

		static SamsonSetup *shared();

		void setDefaultValues()
		{
			// Hard coded default values
			set( SETUP_max_open_files_per_device , "50" );

			set( SETUP_num_processes , "2" );
			set( SETUP_memory , "2147483648" );
			set( SETUP_shm_size_per_process , "536870912" );
		}
		
		void set(std::string name , std::string value)
		{
			std::map<std::string,std::string>::iterator i = items.find( name );
			if( i!= items.end() )
				items.erase( i );
			
			
			items.insert( std::pair<std::string , std::string>( name ,value ) );
		}
		
		size_t getUInt64( std::string name , size_t defaultValue )
		{
			std::multimap<std::string,std::string>::iterator i =  items.find( name );
			if( i == items.end() )
				return defaultValue;
			else
				return atoll(i->second.c_str());
			
		}

		size_t getInt( std::string name , int defaultValue )
		{
			std::multimap<std::string,std::string>::iterator i =  items.find( name );
			if( i == items.end() )
				return defaultValue;
			else
				return atoi(i->second.c_str());
		}
		
	};

}

#endif
