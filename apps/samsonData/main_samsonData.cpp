#include <stdio.h>
#include <iostream>			// std::cerr
#include <fstream>			// std::ifstream
#include <cstring>			// size_t

#include "samson/data/data.pb.h"		// data::Command
#include "samson/data/DataManager.h"	// samson::LogFile



//char* progName = (char*) "samsonData";



std::string stringForAction( samson::data::Command_Action action )
{
	switch (action) {
		case samson::data::Command_Action_Begin:
			return "[Begin     ]";
			break;
		case samson::data::Command_Action_Operation:
			return "[Operation ]";
			break;
		case samson::data::Command_Action_Finish:
			return "[Finish    ]";
			break;
		case samson::data::Command_Action_Cancel:
			return "[Cancel    ]";
			break;
		case samson::data::Command_Action_Comment:
			return "[Comment   ]";
			break;	
		case samson::data::Command_Action_Session:
			return "[Session   ]";
			break;	
	}
	
	return "[Unknown   ]";
}


bool processFile( std::string fileName )
{
	samson::LogFile file(fileName);
	if( !file.openToRead( ) )
		return false;
	
	samson::data::Command command;
	while( file.read( command ) )
	{
		size_t task_id = command.task_id();
		samson::data::Command_Action action = command.action();
		std::string txt = command.command();

		// Time in a particular format
		char buffer_time[100];
		time_t t = (time_t) command.time();
		struct tm timeinfo;

		// Convert to local version
		localtime_r ( &t , &timeinfo );
		
		strftime (buffer_time,100,"%d/%m/%Y (%X)",&timeinfo);
		
		if( action == samson::data::Command_Action_Session)
			std::cout << "*** " << stringForAction( action ) << " " << txt << std::endl;
		else
			std::cout << "<" << buffer_time << ">" << " [" << task_id  << "] " << stringForAction( action ) << " " << txt << std::endl;
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
		LM_X(1,("Error opening data file %s ",fileName.c_str() ));
	}

	
}
