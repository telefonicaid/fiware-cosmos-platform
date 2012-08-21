#ifndef MEMORY_CHECK_H
#define MEMORY_CHECK_H

/*****************************************************************************
*
* FILE                     MemoryCheck.h
*
* DESCRIPTION              Methods to check the platform has the correct memory configuration
*
* AUTHOR                   Grant Croker
*
* CREATION DATE            Mar 5 2012
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>                             // getpagesize()

#include "logMsg/logMsg.h"

#include "samson/common/MemoryCheck.h"          // samson::MemoryCheck
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/samsonDirectories.h"
#include "samson/common/samsonVars.h"           // SAMSON_ARG_VARS SAMSON_ARGS
#include "samson/common/samsonVersion.h"

namespace samson {
void sysctl_value(char *param_name, long int *param_value);
bool MemoryCheck();
}
#endif  // ifndef MEMORY_CHECK_H
