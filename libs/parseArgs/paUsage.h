#ifndef PA_USAGE_H
#define PA_USAGE_H

/* ****************************************************************************
*
* FILE                  paUsage.h
*
* AUTHOR                Ken Zangelin
*
*/
#include <stdio.h>              /* FILE                                      */

#include "parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
*
* paUsage - print synopsis
*/
void paUsage(PaArgument* paList);



/* ****************************************************************************
*
* paExtendedUsage - print extended synopsis
*/
void paExtendedUsage(PaArgument* paList);



/* ****************************************************************************
*
* paHelp - print help text
*/
void paHelp(PaArgument* paList);

#endif
