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
