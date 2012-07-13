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
