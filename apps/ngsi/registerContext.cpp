#include <libxml/tree.h>
#include <string>

#include "ws.h"                             // whitespace functions
#include "registerContext.h"                // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
static int          entityIdIx  = -1;
static int          cralMetaIx  = -1;
static int          rMetaIx     = -1;



/* ****************************************************************************
*
* registerContextRequestNode - 
*/
static void registerContextRequestNode(RegisterContextRequest* rcReqP, xmlNodePtr nodeP, int level)
{
    std::string  path    = "";
    xmlNodePtr   current = nodeP;

    //
    // Fix the path
    //
    while (current != NULL)
    {
        if (current->name != NULL)
            path = (path == "")? std::string((char*) current->name) : std::string((char*) current->name) + std::string(".") + path;
        current = current->parent;
    }

    char*        contentP = (char*) xmlNodeGetContent(nodeP);
    std::string  content  = "emptycontent";

    contentP = wsClean(contentP);
    if (contentP != NULL)
        content  = std::string(contentP);

    if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId.id")
        rcReqP->entityV[entityIdIx]->id = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId.type")
        rcReqP->entityV[entityIdIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId.isPattern")
        rcReqP->entityV[entityIdIx]->isPattern = (content == "false")? false : true;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.name")
       rcReqP->attributeList.name = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.type")
       rcReqP->attributeList.type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.isDomain")
       rcReqP->attributeList.isDomain = (content == "true")? true : false;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.name")
       rcReqP->attributeList.metadataV[cralMetaIx]->name = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.type")
       rcReqP->attributeList.metadataV[cralMetaIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.value")
       rcReqP->attributeList.metadataV[cralMetaIx]->value = content;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata.name")
       rcReqP->registrationMetadataV[rMetaIx]->name = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata.type")
       rcReqP->registrationMetadataV[rMetaIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata.value")
       rcReqP->registrationMetadataV[rMetaIx]->value = content;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.providingApplication")
       rcReqP->providingApplication = content;
    else if (path == "registerContextRequest.duration")
       rcReqP->duration = content;
    else if (path == "registerContextRequest.registrationId")
       rcReqP->registrationId = content;


    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId")
    {
        rcReqP->entityV.push_back(new Entity());
        ++entityIdIx;
    }
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata")
    {
       rcReqP->attributeList.metadataV.push_back(new Metadata());
       ++cralMetaIx;
    }
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata")
    {
       rcReqP->registrationMetadataV.push_back(new Metadata());
       ++rMetaIx;
    }
    else
    {
//        if (contentP != NULL)
//            printf("%d:%s NOT TREATED (content: '%s')\n", level, path.c_str(), content.c_str());
    }
}



/* ****************************************************************************
*
* registerContextRequest - 
*/
static RegisterContextRequest* registerContextRequest(RegisterContextRequest* rcReqP, xmlNodePtr nodeP, int level)
{
    xmlNodePtr firstChild;

    while (nodeP)
    {
        registerContextRequestNode(rcReqP, nodeP, level);
        firstChild = nodeP->children;

        if (firstChild != NULL)
            registerContextRequest(rcReqP, firstChild, level + 1);

        nodeP = nodeP->next;
    }

    return rcReqP;
}



/* ****************************************************************************
*
* registerContextRequestParse - 
*/
RegisterContextRequest* registerContextRequestParse(xmlNodePtr nodeP)
{
    RegisterContextRequest* rcReqP = new RegisterContextRequest();

    entityIdIx  = -1;
    cralMetaIx  = -1;
    rMetaIx     = -1;

    registerContextRequest(rcReqP, nodeP, 0);

    return rcReqP;
}



/* ****************************************************************************
*
* registerContextRequestPresent - 
*/
void registerContextRequestPresent(RegisterContextRequest* rcrP)
{
    unsigned int ix;

    printf("%d entities:\n", (int) rcrP->entityV.size());
    for (ix = 0; ix < rcrP->entityV.size(); ix++)
    {
        printf("  Entity[%d].id:        '%s'\n", ix, rcrP->entityV[ix]->id.c_str());
        printf("  Entity[%d].type:      '%s'\n", ix, rcrP->entityV[ix]->type.c_str());
        printf("  Entity[%d].isPattern: %s\n", ix, (rcrP->entityV[ix]->isPattern == true)? "true" : "false");
    }
    printf("\n");
    
    printf("Attribute List:\n");
    printf("  name:     '%s'\n", rcrP->attributeList.name.c_str());
    printf("  type:     '%s'\n", rcrP->attributeList.type.c_str());
    printf("  isDomain:  %s\n", (rcrP->attributeList.isDomain == true)? "true" : "false");
    printf("  %d metadatas:\n", (int) rcrP->attributeList.metadataV.size());

    for (ix = 0; ix < rcrP->attributeList.metadataV.size(); ix++)
    {
        printf("    Metadata[%d].name:     '%s'\n", ix, rcrP->attributeList.metadataV[ix]->name.c_str());
        printf("    Metadata[%d].type:     '%s'\n", ix, rcrP->attributeList.metadataV[ix]->type.c_str());
        printf("    Metadata[%d].value:    '%s'\n", ix, rcrP->attributeList.metadataV[ix]->value.c_str());
    }
    printf("\n");

    printf("%d Registration Metadata:\n", (int) rcrP->registrationMetadataV.size());
    for (ix = 0; ix < rcrP->registrationMetadataV.size(); ix++)
    {
        printf("    Metadata[%d].name:     '%s'\n", ix, rcrP->registrationMetadataV[ix]->name.c_str());
        printf("    Metadata[%d].type:     '%s'\n", ix, rcrP->registrationMetadataV[ix]->type.c_str());
        printf("    Metadata[%d].value:    '%s'\n", ix, rcrP->registrationMetadataV[ix]->value.c_str());
    }
    printf("\n");

    printf("Providing Application:   '%s'\n", rcrP->providingApplication.c_str());
    printf("Duration:                '%s'\n", rcrP->duration.c_str());
    printf("Registration Id:         '%s'\n", rcrP->registrationId.c_str());
}
