#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/user.h>                           // PAGE_SIZE

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/CommandLine.h"                     // au::CommandLine
#include "au/console/Console.h"                         // au::Console

#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup
#include "samson/common/ports.h"
#include "samson/common/coding.h"				// samson::FormatHeader
#include "samson/common/samsonVars.h"           // SAMSON_ARG_VARS SAMSON_ARGS
#include "samson/module/KVFormat.h"             // samson::KVFormat
#include "samson/module/ModulesManager.h"		// samson::ModulesManager
#include "samson/common/samsonVersion.h"
#include "samson/common/MemoryCheck.h"

#ifdef LINUX
#define KERNEL_SHMMAX "/proc/sys/kernel/shmmax"
#define KERNEL_SHMALL "/proc/sys/kernel/shmall"
#endif

#ifdef OSX
#define KERNEL_SHMMAX "kern.sysv.shmmax"
#define KERNEL_SHMALL "kern.sysv.shmall"
#endif



/* ****************************************************************************
 *
 * Option variables
 */

SAMSON_ARG_VARS;
bool check;


/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] =
{
	SAMSON_ARGS,
	PA_END_OF_ARGS
};



int main(int argC, const char *argV[])
{
    paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

    paParse(paArgs, argC, (char**) argV, 1, false);


    samson::SamsonSetup::init(samsonHome, samsonWorking);          // Load setup and create default directories

    // Check to see if the current memory configuration is ok or not
    if (samson::MemoryCheck() == false)
        LM_X(1,("Insufficient memory configured. Check %s/samsonWorkerLog for more information."));


}
