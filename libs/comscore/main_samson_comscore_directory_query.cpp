/* ****************************************************************************
 *
 * FILE                     main_samson_comscore_directory_create.cpp
 *
 * AUTHOR                   Andreu Urrueka
 *
 * CREATION DATE            2012
 *
 */

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include <signal.h>


#include "au/LockDebugger.h"            // au::LockDebugger
#include "au/ThreadManager.h"

#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/samsonVars.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/daemonize.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/isolated/SharedMemoryManager.h"
#include "samson/stream/BlockManager.h"
#include "samson/module/ModulesManager.h"


#include "comscore/common.h"
#include "comscore/SamsonComscoreDictionary.h"


/* ****************************************************************************
 *
 * Option variables
 */



/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	PA_END_OF_ARGS
};



/* ****************************************************************************
 *
 * global variables
 */

int                   logFd             = -1;


/* ****************************************************************************
 *
 * main - 
 */
int main(int argC, const char *argV[])
{
    
	paConfig("builtin prefix",                (void*) "SS_WORKER_");
	paConfig("usage and exit on any warning", (void*) true);
    
	paConfig("log to screen",                 (void*) true);
    
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);
    
	paParse(paArgs, argC, (char**) argV, 1, false);
    
    
    const char* dictionary_file_name = "/var/comscore/samson_comscore_dictionary.bin";
    
    // Load dictionary
    samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;
    samson_comscore_dictionary.read(dictionary_file_name);
    
    
    samson_comscore_dictionary.getCategories("seat.es");

    char nativeURL[1024];
    while( true )
    {
        
        printf("Enter URL> ");
        fflush(stdout);
        scanf("%s" , nativeURL );
        
        
        //samson_comscore_dictionary.findURLPattern("a.b.seat.es/path/y");
        // Apply
        // -------------------------------------------------
        
        std::vector<uint> categories = samson_comscore_dictionary.getCategories( nativeURL );
        
        if( categories.size() == 0 )
        {
            std::cout << nativeURL << " has not being identifier inside this comscore dictionary\n";
            continue;
        }
        
        for( size_t i = 0 ; i < categories.size() ; i++ )
            std::cout << nativeURL << " -> : " << categories[i] << " " << samson_comscore_dictionary.getCategoryName( categories[i] ) << "\n";
        
/*
 //std::vector<size_t> categories = map_pattern_category.getCategoriesFromPatterm( lPatternID );
        
        {
            std::cout << "URL: " << nativeURL << " fits in pattern " << lPatternID << "\n";
            
            for ( size_t i = 0 ; i < categories.size() ; i++ )
            {
                std::string description = map_categrory_description.getDescriptionForCategory( categories[i] );
                std::cout << "\tCategory: " << description << "\n";
            }
        }
 */       
    }
    
    
    
}