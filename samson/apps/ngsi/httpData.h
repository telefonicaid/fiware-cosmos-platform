/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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

