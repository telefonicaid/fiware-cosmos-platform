#ifndef PA_FROM_H
#define PA_FROM_H

/* ****************************************************************************
*
* FILE                  paFrom.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "baStd.h"              /* BA standard header file                   */

#include "parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
*
* paFromName - 
*/
extern char* paFromName(PaArgument* aP, char* out);



/* ****************************************************************************
*
* paFrom - from where did the value come?
*/
extern char* paFrom(PaArgument* paList, const char* option);

#endif
