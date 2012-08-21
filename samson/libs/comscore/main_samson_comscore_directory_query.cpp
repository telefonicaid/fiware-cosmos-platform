/* ****************************************************************************
 *
 * FILE                     main_samson_comscore_directory_create.cpp
 *
 * AUTHOR                   Andreu Urrueka
 *
 * CREATION DATE            2012
 *
 */

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/parseArgs.h"

#include <signal.h>


#include "au/ThreadManager.h"
#include "au/mutex/LockDebugger.h"            // au::LockDebugger

#include "comscore/SamsonComscoreDictionary.h"
#include "comscore/common.h"

typedef  samson::comscore::uint   uint;

/* ****************************************************************************
 *
 * Option variables
 */



/* ****************************************************************************
 *
 * parse arguments
 */

char dictionary_file_name[1024];

#define DEFAULT_DICTIONARY_LOCATION "/var/comscore/samson_comscore_dictionary.bin"

PaArgument paArgs[] =
{
  { "-dictionary", dictionary_file_name, "",   PaString,   PaOpt, _i DEFAULT_DICTIONARY_LOCATION, PaNL, PaNL,  "Binary Comscore dictionary"   },
  PA_END_OF_ARGS
};


/* ****************************************************************************
 *
 * global variables
 */

int logFd             = -1;


/* ****************************************************************************
 *
 * main -
 */
int main(int argC, const char *argV[]) {
  paConfig("builtin prefix",                (void *)"SS_WORKER_");
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)true);
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("log to file",                   (void *)true);

  paParse(paArgs, argC, (char **)argV, 1, false);


  // Load dictionary
  samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;
  samson_comscore_dictionary.read(dictionary_file_name);

  // const char * url_to_test = "yahoo.com";
  // const char * url_to_test = "m.yahoo.com/aux/yservices/1.0/image/fwpEyWUNOZdh7HgM46ViCA--/resource:/brand/yahoouk/web/1.0.17/image/A/icon/everything/featured/featured_mail";

  char nativeURL[1024];
  while (true) {
    printf("Enter URL> ");
    fflush(stdout);
    if (scanf("%s", nativeURL) == 0)
      LM_W(("Error reading url"));  // Get categories for this URL
    std::vector<uint> categories = samson_comscore_dictionary.getCategories(nativeURL);

    if (categories.size() == 0) {
      std::cout << nativeURL << " has not being identifier inside this comscore dictionary\n";
      continue;
    }

    for (size_t i = 0; i < categories.size(); i++) {
      std::string description = samson_comscore_dictionary.getCategoryName(categories[i]);
      std::cout << nativeURL << " -> : " << categories[i] << " " << description << "\n";
    }
  }
}

