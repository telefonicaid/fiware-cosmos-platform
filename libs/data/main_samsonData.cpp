
#include <stdio.h>
#include <iostream>			// std::cerr
#include "data.pb.h"		// data::Command
#include <fstream>			// std::ifstream
#include <cstring>			// size_t
#include "DataManager.h"	// ss::LogFile

int main( int argc , char *argv[] )
{
	/*
	std::string fileName;
	
	if( argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " path_to_log_file" << std::endl;
		return 0;
	}
	

	if( !file.openToRead( argv[1] ) )
	{
		std::cerr << "Could not open file: " << argv[0] << std::endl;
		return 0;
	}
	
	fileName = argv[1];

	*/
	
	std::string fileName =  "/var/samson/log/log_worker_0";
	
	
	ss::LogFile file;
	file.openToRead( fileName );
	
	ss::data::Command command;
	
	while( file.read( command ) )
	{
		
		std::cout << "Task " << command.task_id();
		switch (command.status()) {
			case ss::data::Command_Status_Begin:
				std::cout << " [B]: ";
				break;
			case ss::data::Command_Status_Operation:
				std::cout << " [O]: ";
				break;
			case ss::data::Command_Status_Finish:
				std::cout << " [F]: ";
				break;
		} 
		
		
		std::cout << command.command() << std::endl;
	}
	
	
}
