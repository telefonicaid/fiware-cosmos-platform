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
#ifndef REST_H
#define REST_H

/* ****************************************************************************
*
* FILE                  rest.h -  
*
*
*
*
*/
#include "Format.h"                     // Format (XML, JSON)



/* ****************************************************************************
*
* allow - string that describes what operations are allowed for a REST request
*/
extern std::string allow;



/* ****************************************************************************
*
* restReply - 
*/
extern bool restReply(int fd, Format format, int httpCode, const char* key, const char* value);



/* ****************************************************************************
*
* restReplySend - 
*/
extern bool restReplySend(int fd, Format format, int httpCode, std::string data);



/* ****************************************************************************
*
* restServe - 
*/
extern int restServe(int fd);

#endif
