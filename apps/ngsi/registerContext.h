#ifndef REGISTER_CONTEXT_H
#define REGISTER_CONTEXT_H

#include <string>
#include <vector>

#include "Entity.h"
#include "Metadata.h"

using namespace std;



/* ****************************************************************************
*
* ContextRegistrationAttributeList - 
*/
typedef struct ContextRegistrationAttributeList
{
    std::string         name;
    std::string         type;
    bool                isDomain;
    vector<Metadata*>   metadataV;
} ContextRegistrationAttributeList;



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



/* ****************************************************************************
*
* registerContextRequestParse - 
*/
extern RegisterContextRequest* registerContextOldRequestParse(xmlNodePtr node0P);
extern RegisterContextRequest* registerContextRequestParse(xmlNodePtr node0P);



/* ****************************************************************************
*
* registerContextRequestPresent - 
*/
extern void registerContextRequestPresent(RegisterContextRequest* rcrP);

#endif
