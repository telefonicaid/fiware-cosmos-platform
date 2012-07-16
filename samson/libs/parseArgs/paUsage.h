#ifndef PA_USAGE_H
#define PA_USAGE_H

/* ****************************************************************************
*
* FILE                  paUsage.h
*
* AUTHOR                Ken Zangelin
*
*/
#include <stdio.h>                        /* FILE                            */

#include "parseArgs/parseArgs.h"          /* PaArgument                      */



/* ****************************************************************************
*
* paUsage - print synopsis
*/
void paUsage(void);



/* ****************************************************************************
*
* paVersionPrint - print synopsis
*/
void paVersionPrint(void);



/* ****************************************************************************
*
* paExtendedUsage - print extended synopsis
*/
void paExtendedUsage(void);



/* ****************************************************************************
*
* paHelp - print help text
*/
void paHelp(void);

#endif
