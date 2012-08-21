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
  { "-dictionary", dictionary_file_name, "", PaString, PaOpt, _i DEFAULT_DICTIONARY_LOCATION,   PaNL,   PaNL,
    "Binary Comscore dictionary"    },
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

  size_t num = samson_comscore_dictionary.getNumEntries();

  printf("Comscore dictionary %s\n", dictionary_file_name);
  printf("--------------------------------------\n");

  for (size_t i = 0; i < num; i++) {
    printf("PATTERN:    %lu\n", samson_comscore_dictionary.getPatternIdForEnty(i));
    printf("DOMAIN:     %s\n", samson_comscore_dictionary.getDomainForEntry(i));
    printf("PRE-DOMAIN: %s\n", samson_comscore_dictionary.getPreDomainPatternForEntry(i));
    printf("PATH:       %s\n", samson_comscore_dictionary.getPathPatternForEntry(i));
    printf("\n");
  }
}

