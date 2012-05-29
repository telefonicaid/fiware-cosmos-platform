#ifndef CONTEXT_REGISTRATION_ATTRIBUTE_H
#define CONTEXT_REGISTRATION_ATTRIBUTE_H

/* ****************************************************************************
*
* FILE                  
*
*
*
*
*/
#include <string>
#include <vector>

#include "Entity.h"
#include "ContextRegistrationAttributeList.h"
#include "Metadata.h"

using namespace std;



/* ****************************************************************************
*
* RegisterContextRequest - 
*/
typedef struct RegisterContextRequest
{
    vector<Entity*>                  entityV;
    ContextRegistrationAttributeList attributeList;
    vector<Metadata*>                registrationMetadataV;
    std::string                      providingApplication;
    std::string                      duration;
    std::string                      registrationId;
} RegisterContextRequest;

#endif
