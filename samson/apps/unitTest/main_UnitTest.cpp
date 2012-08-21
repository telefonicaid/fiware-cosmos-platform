/* ****************************************************************************
 *
 * FILE            main_UnitTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Main file for the automatic unit testing application
 *
 */

#include "au/log/LogToServer.h"
#include "au/log/log_server_common.h"
#include "gtest/gtest.h"
#include "logMsg/logMsg.h"
#include "parseArgs/paBuiltin.h"        // paLsHost, paLsPort
#include "parseArgs/paConfig.h"         // paConfig()
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"
#include "samson/common/SamsonSetup.h"
#include <stdio.h>


extern bool lmAssertAtExit;

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
int logFd             = -1;

/* ****************************************************************************
 *
 * main -
 */

int main(int argC, char **argV) {
  paConfig("usage and exit on any warning", (void *)true);
//  paConfig("log to screen",                 (void*) "only errors");
  paConfig("log to screen",                 (void *)false);
  paConfig("log to file",                   (void *)true);
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("default value", "-logDir",      (void *)"/var/log/samson");

  paConfig("man author",                    "Samson team");


  // Goyo. trying to get traces
  if (( argC > 2 ) &&  (strcmp(argV[1], "-t") == 0))
    paParse(paArgs, 3, (char **)argV, 3, false); else
    // Avoid parsing any argument
    paParse(paArgs, 1, (char **)argV, 1, false);  // Set assert flag to true ro force asserts instead of exits
  lmAssertAtExit = true;

  // Start login to server ( this avoids warning )
  au::start_log_to_server("localhost", AU_LOG_SERVER_PORT, "/tmp/smson_unitTest");

  // Run all tests
  ::testing::InitGoogleTest(&argC, argV);
  return RUN_ALL_TESTS();
}

