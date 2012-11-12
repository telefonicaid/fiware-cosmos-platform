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
#ifndef VERB_H
#define VERB_H

/* ****************************************************************************
*
* FILE                  Verb.h 
*
*
*
*
*/



/* ****************************************************************************
*
* Verb - 
*/ 
typedef enum Verb
{
    GET = 1,
    POST,
    PUT,
    DELETE
} Verb;



/* ****************************************************************************
*
* verbName - 
*/
extern const char* verbName(Verb verb);

#endif
