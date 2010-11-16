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
extern char* paFromName(PaArgument* aP, const char* out);



/* ****************************************************************************
*
* paFrom - from where did the value come?
*/
extern char* paFrom(PaArgument* paList, char* option);

#endif
