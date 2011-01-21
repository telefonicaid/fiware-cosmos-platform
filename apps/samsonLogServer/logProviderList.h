#ifndef LOG_PROVIDER_LIST_H
#define LOG_PROVIDER_LIST_H

/* ****************************************************************************
*
* FILE                     logProviderList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 20 2011
*
*/
#include "LogProvider.h"        // LogProvider



/* ****************************************************************************
*
* logProviderListInit - 
*/
extern void logProviderListInit(unsigned int lpMax);



/* ****************************************************************************
*
* logProviderAdd - 
*/
extern void logProviderAdd(const char* name, const char* host, int fd);



/* ****************************************************************************
*
* logProviderRemove - 
*/
extern void logProviderRemove(LogProvider* lpP);



/* ****************************************************************************
*
* logProviderLookup - 
*/
extern LogProvider* logProviderLookup(char* name, char* host);



/* ****************************************************************************
*
* logProviderLookup - 
*/
extern LogProvider* logProviderLookup(unsigned int ix);



/* ****************************************************************************
*
* logProviderListGet - 
*/
extern LogProvider** logProviderListGet(void);



/* ****************************************************************************
*
* logProviderMaxGet - 
*/
extern unsigned int logProviderMaxGet(void);



/* ****************************************************************************
*
* logProviderListShow - 
*/
extern void logProviderListShow(const char* why);



/* ****************************************************************************
*
* logProviderMsgTreat - 
*/
extern void logProviderMsgTreat(LogProvider* lpP);

#endif
