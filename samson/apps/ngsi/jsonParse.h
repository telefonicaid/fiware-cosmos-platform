#ifndef JSON_PARSE_H
#define JSON_PARSE_H

/* ****************************************************************************
*
* FILE                      jsonParse.h -  
*/
#include <sys/types.h>            // system types ...

#include "json.h"                 // json_type



/* ****************************************************************************
*
* Forward declarations
*/
struct Entity;



/* ****************************************************************************
*
* jsonTypeName - 
*/
extern const char* jsonTypeName(json_type type);



/* ****************************************************************************
*
* jsonParse - 
*/
extern int jsonParse(struct Entity* entityP, char* in);

#endif
