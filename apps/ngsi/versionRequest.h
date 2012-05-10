#ifndef VERSION_REQUEST_H
#define VERSION_REQUEST_H

/* ****************************************************************************
*
* FILE                versionRequest.h - 
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
* versionRequest - 
*/
extern int versionRequest(int fd, Verb verb, Format format, int components, std::string* component);

#endif

