#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

/* ****************************************************************************
*
* FILE                     configFile.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 17 2011
*
*/



/* ****************************************************************************
*
* configFileInit
*/
extern void configFileInit(const char* path);



/* ****************************************************************************
*
* configFileParse - 
*/
extern int configFileParse(const char* processHost, const char* processName, int* argCP, char** argV);



/* ****************************************************************************
*
* configFileParseByAlias - 
*/
extern int configFileParseByAlias(const char* alias, char* processHost, char* processName, int* argCP, char** argV);

#endif

