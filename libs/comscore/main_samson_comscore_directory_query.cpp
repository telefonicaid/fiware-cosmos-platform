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

#include "comscore/common.h"
#include "comscore/SamsonComscoreDictionary.h"

typedef  samson::comscore::uint uint;

/* ****************************************************************************
 *
 * Option variables
 */



/* ****************************************************************************
 *
 * parse arguments
 */

int number;

PaArgument paArgs[] =
{
	{ "",         &number,           "",       PaInt,  PaOpt,      0,    0,  10000000, "Number of test over a static url"     },
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
    

	const char * url_to_test = "yahoo.com";
	//const char * url_to_test = "m.yahoo.com/aux/yservices/1.0/image/fwpEyWUNOZdh7HgM46ViCA--/resource:/brand/yahoouk/web/1.0.17/image/A/icon/everything/featured/featured_mail";
    
    if( number > 0 )
    {
        au::Cronometer cronometer;
        
        for ( int j = 0 ; j < number ; j++ )
        {
            
            // Get categories for this URL
            std::vector<uint> categories = samson_comscore_dictionary.getCategories( url_to_test );
            
            for( size_t i = 0 ; i < categories.size() ; i++ )
                std::string description = samson_comscore_dictionary.getCategoryName( categories[i] );
    
            if( (j%10000) == 0 )
                LM_M(("Tested %d times in %s",j , au::time_string(cronometer.diffTimeInSeconds()).c_str() ));
            
        }
        
        LM_M(("Tested %d times in %s",number , au::time_string(cronometer.diffTimeInSeconds()).c_str() ));
        
        return 0;
    }
    

    char nativeURL[1024];
    while( true )
    {
        printf("Enter URL> ");
        fflush(stdout);
        if ( scanf("%s" , nativeURL ) == 0 )
            LM_W(("Error reading url"));

        // Get categories for this URL
        std::vector<uint> categories = samson_comscore_dictionary.getCategories( nativeURL );
        
        if( categories.size() == 0 )
        {
            std::cout << nativeURL << " has not being identifier inside this comscore dictionary\n";
            continue;
        }
        
        for( size_t i = 0 ; i < categories.size() ; i++ )
        {
            std::string description = samson_comscore_dictionary.getCategoryName( categories[i] );
            std::cout << nativeURL << " -> : " << categories[i] << " " << description << "\n";
        }
        
    }
    
    
    
}
