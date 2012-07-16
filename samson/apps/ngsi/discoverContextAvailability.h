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
