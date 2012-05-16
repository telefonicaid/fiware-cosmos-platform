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
