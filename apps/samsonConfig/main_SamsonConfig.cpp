
#include <iostream>
#include "au/CommandLine.h"			// au::CommandLine
#include "samson/common/coding.h"					// samson::FormatHeader
#include "samson/module/KVFormat.h"		// samson::KVFormat
#include "samson/module/ModulesManager.h"			// samson::ModulesManager
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup

int logFd = -1;

//char* progName = (char*) "samsonCat";


int main(int argc, const char *argv[])
{
	au::CommandLine cmdLine;
	cmdLine.set_flag_string("working",SAMSON_DEFAULT_WORKING_DIRECTORY);
    cmdLine.set_flag_boolean("check");
    cmdLine.set_flag_boolean("edit");
    cmdLine.set_flag_boolean("help");
    cmdLine.set_flag_boolean("-help");
	cmdLine.parse(argc , argv);

	samson::SamsonSetup::init(  );
    samson::SamsonSetup::shared()->setWorkingDirectory(cmdLine.get_flag_string("working"));

    
    if( cmdLine.get_flag_bool("check") )
    {
        std::cout << "Checking setup...";
        
        // Check everything looks ok
        au::ErrorManager errorManager;
        samson::SamsonSetup::shared()->check(&errorManager);
        
        if( errorManager.isActivated() ) 
            std::cout << "Error checking setup: " << errorManager.getMessage();
        else
            std::cout << "OK!\n";
        
        return 0;
    }

    
    if( cmdLine.get_flag_bool("edit") )
    {
        // Edit mode
        
        samson::SamsonSetup::shared()->edit();  // Enter in edit mode from command line
        samson::SamsonSetup::shared()->save();  // Save a new file with the current setup
        
        std::cout << "File " << samson::SamsonSetup::shared()->setupFile << " has been successfully generated\n\n";
        
        return 0;
    }

    
    if( cmdLine.get_flag_bool("help") || cmdLine.get_flag_bool("-help") )
    {
        std::cout << "samsonConfig [-check][-edit] : Samson config tool. Use -edit to create a new setup file\n\n";
        return 0;
    }
    
    
    
    
    // Show samson setup
    std::cout << samson::SamsonSetup::shared()->str();
	
}
