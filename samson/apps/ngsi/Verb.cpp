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
/* ****************************************************************************
*
*  FILE                 Verb.cpp -  
*
*
*
*
*/
#include <string>

#include "Verb.h"                       // Own interface

using namespace std;



/* ****************************************************************************
*
* verbName - 
*/
const char* verbName(Verb verb)
{
    switch (verb)
    {
    case GET:    return "GET";
    case POST:   return "POST";
    case PUT:    return "PUT";
    case DELETE: return "DELETE";
    }

    return "VERB";
}
