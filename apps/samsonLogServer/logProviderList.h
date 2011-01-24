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
#include "Endpoint.h"           // ss::Endpoint
#include "LogProvider.h"        // LogProvider



/* ****************************************************************************
*
* logProviderListInit - 
*/
extern void logProviderListInit(unsigned int lpMax);



/* ****************************************************************************
*
* logProviderNameSet - 
*/
extern void logProviderNameSet(LogProvider* lpP, const char* name, const char* ip);



/* ****************************************************************************
*
* logProviderStateSet - 
*/
extern void logProviderStateSet(LogProvider* lpP, const char* state);



/* ****************************************************************************
*
* logProviderAdd - 
*/
extern void logProviderAdd(ss::Endpoint* endpoint, const char* name, const char* host, int fd);



/* ****************************************************************************
*
* logProviderRemove - 
*/
extern void logProviderRemove(LogProvider* lpP);



/* ****************************************************************************
*
* logProviderLookup - 
*/
extern LogProvider* logProviderLookup(unsigned int ix);
extern LogProvider* logProviderLookup(char* name, char* host);
extern LogProvider* logProviderLookup(ss::Endpoint* ep);



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
