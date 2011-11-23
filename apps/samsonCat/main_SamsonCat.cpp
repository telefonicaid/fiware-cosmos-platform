
#include <iostream>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "au/CommandLine.h"                         // au::CommandLine

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "parseArgs/parseArgs.h"

#include "au/LockDebugger.h"            // au::LockDebugger

#include "samson/common/coding.h"					// samson::FormatHeader
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup
#include "samson/common/samsonVars.h"               // SAMSON_ARG_VARS

#include "samson/module/KVFormat.h"                 // samson::KVFormat
#include "samson/module/ModulesManager.h"			// samson::ModulesManager

#include "samson/delilah/SamsonFile.h"              // samson::SamsonFile
#include "samson/delilah/SamsonDataSet.h"           // samson::SamsonDataSet


/* ****************************************************************************
 *
 * Option variables
 */

bool show_header;
bool show_hg;
size_t show_limit;
char file_name[1024];

SAMSON_ARG_VARS;

#define S01 (long int) "samson01:1234"
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
    SAMSON_ARGS,
	{ "-limit",       &show_limit,         "",     PaInt,     PaOpt,     0,      0,    100000,      "Limit in the number of records"   },
	{ "-header",      &show_header,        "",     PaBool,    PaOpt,    false,  false,   true,   "Show only header"          },
	{ "-hg",          &show_hg,            "",     PaBool,    PaOpt,    false,  false,   true,   "Show only hash-group information" },
	{ " ",            file_name,           "",     PaString,  PaReq,    (long)  "null",   PaNL,      PaNL,  "name of the file or directory to scan"   },
	PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;




int main(int argC, const char *argV[])
{
    
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] (p.PID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);
	
	paParse(paArgs, argC, (char**) argV, 1, false);// No more pid in the log file name
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
    
	samson::SamsonSetup::init( samsonHome , samsonWorking );
    samson::ModulesManager::init();
	
    
    struct stat filestatus;
    stat( file_name , &filestatus );
    
    if ( S_ISREG( filestatus.st_mode ) )
    {
        
        // Open a single file
        samson::SamsonFile samsonFile( file_name );
        
        if( samsonFile.hasError() )
            LM_X(1,("%s", samsonFile.getErrorMessage().c_str() ));    
        
        if( show_header )
        {
            std::cout << samsonFile.header.str() << " " << " [ " << samsonFile.header.info.kvs << " kvs in " << samsonFile.header.info.size << "bytes\n";            
            exit(0);
        }
        
        if( show_hg )
        {
            std::cout << samsonFile.getHashGroups();
            exit(0);
        }
        
        samsonFile.printContent( show_limit );
        
    }
    else if( S_ISDIR( filestatus.st_mode ) )
    {
        
        samson::SamsonDataSet samsonDataSet( file_name );
        
        if( samsonDataSet.error.isActivated() )
            LM_X(1,("%s", samsonDataSet.error.getMessage().c_str() ));    
        
        if( show_header )
        {
            samsonDataSet.printHeaders();
            exit(0);
        }
        
        if( show_hg )
        {
            samsonDataSet.printHashGroups();
            exit(0);
        }
        
        samsonDataSet.printContent( show_limit );
        
    } 
    else
    {
        LM_E(("%s is not a file or a directory",file_name));
    }
    
    
	
	
	
}
