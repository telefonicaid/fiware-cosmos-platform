#ifndef PA_IS_OPTION_H
#define PA_IS_OPTION_H

/* ****************************************************************************
*
* FILE                  paIsOption.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "parseArgs/parseArgs.h"          /* PaArgument, ...                           */



/* ****************************************************************************
*
* paIsOption - is the string 'string' an option?
*/
extern bool paIsOption(PaiArgument* paList, char* string);

#endif
