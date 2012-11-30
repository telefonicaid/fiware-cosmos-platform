/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * FILE                     main_SamsonMemCheck.cpp
 *
 * DESCRIPTION              Main routine for samsonMemCheck executable
 *
 * AUTHOR                   Grant Croker
 *
 * CREATION DATE            Mar 5 2012
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>                           // PAGE_SIZE
#include <unistd.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/parseArgs.h"

#include "au/CommandLine.h"                     // au::CommandLine
#include "au/console/Console.h"                 // au::console::Console

#include "samson/common/MemoryCheck.h"
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/coding.h"               // samson::FormatHeader
#include "samson/common/ports.h"
#include "samson/common/samsonVars.h"           // SAMSON_ARG_VARS SAMSON_ARGS
#include "samson/common/samsonVersion.h"
#include "samson/module/KVFormat.h"             // samson::KVFormat
#include "samson/module/ModulesManager.h"       // samson::ModulesManager




/* ****************************************************************************
 *
 * Option variables
 */

SAMSON_ARG_VARS;


/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] = {
  SAMSON_ARGS,
  PA_END_OF_ARGS
};



int main(int argc, const char *argv[]) {
  paConfig("usage and exit on any warning", reinterpret_cast<void *>(true));
  paConfig("log to screen", reinterpret_cast<void *>(true));
  paConfig("log file line format",
           reinterpret_cast<void *>(const_cast<char *>("TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT")));
  paConfig("screen line format", reinterpret_cast<void *>(const_cast<char *>("TYPE@TIME  EXEC: TEXT")));
  paConfig("log to file", reinterpret_cast<void *>(true));

  paParse(paArgs, argc, (char **)argv, 1, false);

  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);            // Load setup and create default directories

  // Check to see if the current memory configuration is ok or not
  if (samson::MemoryCheck() == false) {
    LOG_X(1, ("Insufficient memory configured. Check %s/samsonWorkerLog for more information."));
  } else {
    LOG_SM(("samsonMemCheck ok"));
  }
}

