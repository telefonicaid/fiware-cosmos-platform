#ifndef PA_OPTIONS_PARSE_H
#define PA_OPTIONS_PARSE_H

/* ****************************************************************************
 *
 * FILE                  paOptionsParse.c -
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
 *
 * paOptionsParse -
 */
extern int paOptionsParse(PaiArgument * paList, char *argV[], int argC);

#endif
