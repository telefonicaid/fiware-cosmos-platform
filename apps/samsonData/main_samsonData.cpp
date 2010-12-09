#include <stdio.h>
#include <iostream>			// std::cerr
#include <fstream>			// std::ifstream
#include <cstring>			// size_t

#include "data.pb.h"		// data::Command
#include "DataManager.h"	// ss::LogFile



char* progName = (char*) "samsonData";



std::string stringForAction( ss::data::Command_Action action )
{
	switch (action) {
		case ss::data::Command_Action_Begin:
			return "[Begin     ]";
			break;
		case ss::data::Command_Action_Operation:
			return "[Operation ]";
			break;
		case ss::data::Command_Action_Finish:
			return "[Finish    ]";
			break;
		case ss::data::Command_Action_Cancel:
			return "[Cancel    ]";
			break;
		case ss::data::Command_Action_Comment:
			return "[Comment   ]";
			break;	
		case ss::data::Command_Action_Session:
			return "[Session   ]";
			break;	
	}
	
	return "[Unknown   ]";
}


bool processFile( std::string fileName )
{
	ss::LogFile file(fileName);
	if( !file.openToRead( ) )
		return false;
	
	ss::data::Command command;
	while( file.read( command ) )
	{
		size_t task_id = command.task_id();
		ss::data::Command_Action action = command.action();
		std::string txt = command.command();
		
		if( action == ss::data::Command_Action_Session)
			std::cout << "*** " << stringForAction( action ) << " " << txt << std::endl;
		else
			std::cout << "[" << task_id  << "] " << stringForAction( action ) << " " << txt << std::endl;
	}
	
	return true;
}

int main( int argc , char *argv[] )
{
	std::string fileName;
	
	if( argc < 2 )
	{
		if( processFile("/opt/samson/controller/log_controller") )
			return 0;	// Return without error
		
		std::cerr << "Usage: " << argv[0] << " path_to_log_file" << std::endl;
		return 0;
	}
	
	fileName = argv[1];

	if( !processFile( fileName ) )
	{
		std::cerr << "Error opening " << fileName << std::endl; 
		exit(1);
	}

	
}
