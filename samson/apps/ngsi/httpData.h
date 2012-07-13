#ifndef HTTP_DATA_H
#define HTTP_DATA_H

#include <string>                           // std::string



/* ****************************************************************************
*
* HTTP Header and Data variables
*/
extern char* contentType;
extern char* contentLength;
extern char* host;
extern char* userAgent;
extern char* accepts;

extern char*        httpData[100];
extern int          httpDataLines;
extern std::string  httpDataString;



/* ****************************************************************************
*
* httpDataParse - 
*/
extern bool httpDataParse(char** input, int lines, bool continued = false);

#endif

