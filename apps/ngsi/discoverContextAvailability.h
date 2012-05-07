#ifndef DISCOVER_CONTEXT_AVAILABILITY_h
#define DISCOVER_CONTEXT_AVAILABILITY_h

#include <string>
#include <vector>

#include "Entity.h"
#include "Scope.h"



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
* discoverContextAvailabilityRequestParse - 
*/
extern DiscoverContextAvailabilityRequest* discoverContextAvailabilityRequestParse(xmlNodePtr nodeP);



/* ****************************************************************************
*
* discoverContextAvailabilityRequestPresent - 
*/
extern void discoverContextAvailabilityRequestPresent(DiscoverContextAvailabilityRequest* dcarP);

#endif
