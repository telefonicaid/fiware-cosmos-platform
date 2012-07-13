#ifndef PA_ITERATE_H
#define PA_ITERATE_H

/* ****************************************************************************
*
* FILE                  paIterate.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
*
* paIterateInit - 
*/
extern void paIterateInit(void);



/* ****************************************************************************
*
* paIterateNext - 
*/
extern PaiArgument* paIterateNext(PaiArgument* paList);



/* ****************************************************************************
*
* paNoOfOptions - 
*/
extern int paNoOfOptions(PaArgument* paList);



/* ****************************************************************************
*
* paOptionLookupByIx - 
*/
extern PaArgument* paOptionLookupByIx(PaArgument* paList, int ix);

#endif
