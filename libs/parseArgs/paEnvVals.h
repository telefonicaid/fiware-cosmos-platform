#ifndef PA_ENV_VALS_H
#define PA_ENV_VALS_H

/* ****************************************************************************
*
* FILE                  paEnvVals.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
*
* paEnvName - get real name of variable (environment or RC-file variable)
*/
extern char* paEnvName(PaArgument* aP, char* out, int index);



/* ****************************************************************************
*
* paEnvVals - 
*/
extern int paEnvVals(PaArgument* paList);

#endif
