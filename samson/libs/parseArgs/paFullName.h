#ifndef PA_FULL_NAME_H
#define PA_FULL_NAME_H

/* ****************************************************************************
*
* FILE                  paFullName.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
*
* paFullName - fill in name and description in string string
*/
extern char* paFullName(char* string, size_t max_length, PaiArgument* aP);

#endif
