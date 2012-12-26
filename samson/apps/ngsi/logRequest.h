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
#ifndef LOG_REQUEST_H
#define LOG_REQUEST_H

/* ****************************************************************************
*
* FILE                logRequest.h - 
*
*
*
*
*/
#include <string>                           // std::string

#include "Format.h"                         // Format (XML, JSON)
#include "Verb.h"                           // Verb



/* ****************************************************************************
*
* logRequest - 
*/
extern int logRequest(int fd, Verb verb, Format format, int components, std::string* component);

#endif
