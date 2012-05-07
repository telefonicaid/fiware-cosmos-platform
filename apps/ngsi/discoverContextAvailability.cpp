#include <libxml/tree.h>
#include <string>

#include "ws.h"                             // whitespace functions
#include "Scope.h"                          // Scope
#include "discoverContextAvailability.h"    // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
static int          entityIdIx  = -1;
static int          attributeIx = -1;
static int          scopeIx     = -1;



/* ****************************************************************************
*
* discoverContextAvailabilityRequestNode - 
*/
static void discoverContextAvailabilityRequestNode(DiscoverContextAvailabilityRequest* dcarP, xmlNodePtr nodeP, int level)
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

    if (path == "discoverContextAvailabilityRequest.entityIdList.entityId.id")
        dcarP->entityV[entityIdIx]->id = content;
    else if (path == "discoverContextAvailabilityRequest.entityIdList.entityId.type")
        dcarP->entityV[entityIdIx]->type = content;
    else if (path == "discoverContextAvailabilityRequest.entityIdList.entityId.isPattern")
        dcarP->entityV[entityIdIx]->isPattern = (content == "false")? false : true;
    else if (path == "discoverContextAvailabilityRequest.attributeList.attribute")
        dcarP->attributeV.push_back(content);
    else if (path == "discoverContextAvailabilityRequest.restriction.scope.operationScope.scopeType")
        dcarP->scopeV[scopeIx]->type = content;
    else if (path == "discoverContextAvailabilityRequest.restriction.scope.operationScope.scopeValue")
        dcarP->scopeV[scopeIx]->value = content;
    else if (path == "discoverContextAvailabilityRequest.restriction.attributeExpression")
        dcarP->attributeExpression = content;
    else if (path == "discoverContextAvailabilityRequest.entityIdList.entityId")
    {
        dcarP->entityV.push_back(new Entity());
        ++entityIdIx;
    }
    else if (path == "discoverContextAvailabilityRequest.restriction.scope.operationScope")
    {
        dcarP->scopeV.push_back(new Scope());
        ++scopeIx;
    }
    else
    {
//        if (contentP != NULL)
//            printf("%d:%s NOT TREATED (content: '%s')\n", level, path.c_str(), content.c_str());
    }
}



/* ****************************************************************************
*
* discoverContextAvailabilityRequest - 
*/
static DiscoverContextAvailabilityRequest* discoverContextAvailabilityRequest(DiscoverContextAvailabilityRequest* dcarP, xmlNodePtr nodeP, int level)
{
    xmlNodePtr firstChild;

    while (nodeP)
    {
        discoverContextAvailabilityRequestNode(dcarP, nodeP, level);
        firstChild = nodeP->children;

        if (firstChild != NULL)
            discoverContextAvailabilityRequest(dcarP, firstChild, level + 1);

        nodeP = nodeP->next;
    }

    return dcarP;
}



/* ****************************************************************************
*
* discoverContextAvailabilityRequestParse - 
*/
DiscoverContextAvailabilityRequest* discoverContextAvailabilityRequestParse(xmlNodePtr nodeP)
{
    DiscoverContextAvailabilityRequest* dcarP = new DiscoverContextAvailabilityRequest();

    entityIdIx  = -1;
    attributeIx = -1;
    scopeIx     = -1;

    discoverContextAvailabilityRequest(dcarP, nodeP, 0);

    return dcarP;
}



/* ****************************************************************************
*
* discoverContextAvailabilityRequestPresent - 
*/
void discoverContextAvailabilityRequestPresent(DiscoverContextAvailabilityRequest* dcarP)
{
    unsigned int ix;

    printf("%d entities:\n", (int) dcarP->entityV.size());
    for (ix = 0; ix < dcarP->entityV.size(); ix++)
    {
        printf("  Entity[%d].id:        '%s'\n", ix, dcarP->entityV[ix]->id.c_str());
        printf("  Entity[%d].type:      '%s'\n", ix, dcarP->entityV[ix]->type.c_str());
        printf("  Entity[%d].isPattern: %s\n", ix, (dcarP->entityV[ix]->isPattern == true)? "true" : "false");
    }
    printf("\n");
    
    printf("Attributes:\n");
    for (ix = 0; ix < dcarP->attributeV.size(); ix++)
        printf("  %s\n", dcarP->attributeV[ix].c_str());
    printf("\n");

    printf("attributeExpression:  '%s'\n", dcarP->attributeExpression.c_str());
    printf("\n");

    printf("%d scopes:\n", (int) dcarP->scopeV.size());
    for (ix = 0; ix < dcarP->scopeV.size(); ix++)
    {
        printf("  scopeType[%d]:  '%s'\n", ix, dcarP->scopeV[ix]->type.c_str());
        printf("  scopeValue[%d]: '%s'\n", ix, dcarP->scopeV[ix]->value.c_str());
    }
}



