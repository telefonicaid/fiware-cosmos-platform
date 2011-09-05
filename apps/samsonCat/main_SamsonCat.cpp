
#include <iostream>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "au/CommandLine.h"                         // au::CommandLine

#include "samson/common/coding.h"					// samson::FormatHeader
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup

#include "samson/module/KVFormat.h"                 // samson::KVFormat
#include "samson/module/ModulesManager.h"			// samson::ModulesManager

#include "samson/delilah/SamsonFile.h"              // samson::SamsonFile
#include "samson/delilah/SamsonDataSet.h"           // samson::SamsonDataSet

int logFd = -1;

//char* progName = (char*) "samsonCat";



int main(int argc, const char *argv[])
{
    
    
	au::CommandLine cmdLine;
	cmdLine.set_flag_boolean("header");
	cmdLine.set_flag_boolean("hg");
	cmdLine.set_flag_boolean("debug");
	cmdLine.set_flag_uint64("limit" , 100 );
	cmdLine.set_flag_string("working",SAMSON_DEFAULT_WORKING_DIRECTORY);
	cmdLine.parse(argc , argv);

	samson::SamsonSetup::init(  );
    samson::SamsonSetup::shared()->setWorkingDirectory(cmdLine.get_flag_string("working"));
    samson::ModulesManager::init();
	
	size_t limit = cmdLine.get_flag_uint64("limit");
	
	
	if( cmdLine.get_num_arguments() < 2 ) 
	{
		std::cerr << "Usage: samsonCat file_or_directory [-header] [-hg]\n";
		exit(0);
	}

	std::string fileName = cmdLine.get_argument(1);

    
    struct stat filestatus;
    stat( fileName.c_str() , &filestatus );
    
    if ( S_ISREG( filestatus.st_mode ) )
    {
        
        // Open a single file
        samson::SamsonFile samsonFile( fileName );
        
        if( samsonFile.hasError() )
        {
            std::cerr << samsonFile.getErrorMessage();    
            exit(0);
        }
        
        
        if( cmdLine.get_flag_bool("header") )
        {
            std::cout << samsonFile.header.str() << " " << " [ " << samsonFile.header.info.kvs << " kvs in " << samsonFile.header.info.size << "bytes\n";            
            exit(0);
        }
        
        if( cmdLine.get_flag_bool("hg" ) )
        {
            std::cout << samsonFile.getHashGroups();
            exit(0);
        }
        
        samsonFile.printContent( limit );
        
    }
    else if( S_ISDIR( filestatus.st_mode ) )
    {
        samson::SamsonDataSet samsonDataSet( fileName );
        
        if( samsonDataSet.error.isActivated() )
        {
            std::cerr << samsonDataSet.error.getMessage();
            exit(0);
        };
        
        if( cmdLine.get_flag_bool("header") )
        {
            samsonDataSet.printHeaders();
            exit(0);
        }
        
        if( cmdLine.get_flag_bool("hg" ) )
        {
            samsonDataSet.printHashGroups();
            exit(0);
        }
        
        samsonDataSet.printContent( limit );
        
    } 
    else
    {
        std::cerr << "Error:"  << fileName << " is not a file or a directory.\n";
    }
    
    
	
	
	
}
