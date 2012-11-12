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
#ifndef DISCOVER_CONTEXT_AVAILABILITY_h
#define DISCOVER_CONTEXT_AVAILABILITY_h

#include <string>
#include <vector>

#include "Format.h"                         // Format
#include "Verb.h"                           // Verb
#include "Entity.h"                         // Entity
#include "Scope.h"                          // Scope



/* ****************************************************************************
*
* DiscoverContextAvailabilityRequest - 
*/
typedef struct DiscoverContextAvailabilityRequest
{
    vector<Entity*>                  entityV;
    vector<std::string>              attributeV;
    std::string                      attributeExpression;
    vector<Scope*>                   scopeV;
} DiscoverContextAvailabilityRequest;



/* ****************************************************************************
*
* discoverContextAvailability - 
*/
extern bool discoverContextAvailability(int fd, Verb verb, Format format, char* data);

#endif
