#include <stdio.h>
#include <iostream>			// std::cerr
#include <fstream>			// std::ifstream
#include <cstring>			// size_t

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "samson/data/data.pb.h"		// data::Command
#include "samson/data/DataManager.h"	// samson::LogFile

#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup
#include "samson/common/samsonVars.h"               // SAMSON_ARG_VARS

SAMSON_ARG_VARS;

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


char file_name[1024];



PaArgument paArgs[] =
{
    SAMSON_ARGS,
    { " ",   file_name,  "DATA_FILE_NAME",   PaString,  PaOpt,  (long) "null",   PaNL,   PaNL,  "file to read"  },
    PA_END_OF_ARGS
};

const char* manShortDescription = "samsonData is a debug tool used to scan the log file of controller in a SAMSON system";

int logFd = -1;

int main( int argC , char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);

    paConfig("log to screen",                 (void*) "only errors");
    paConfig("screen line format",            (void*) "TYPE: TEXT");
    paConfig("log to file",                   (void*) true);
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] (p.PID) FUNC: TEXT");
    paConfig("log to stderr",                 (void*) true);
    paConfig("man shortdescription",          (void*) manShortDescription);

    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    lmAux((char*) "father");
    logFd = lmFirstDiskFileDescriptor();

    samson::SamsonSetup::init( samsonHome , samsonWorking );

    if (!strcmp(file_name, "null"))
    {
        sprintf(file_name, "%s/log/log_controller", samsonWorking);
    }

    if( !processFile( file_name ) )
    {
        LM_X(1,("Error opening data file %s ", file_name ));
    }


}
