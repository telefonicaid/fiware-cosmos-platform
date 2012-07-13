#include <libxml/tree.h>
#include <string.h>
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
static std::string  nodename[10];



/* ****************************************************************************
*
* registerContextOldRequestNode - 
*/
void registerContextOldRequestNode(RegisterContextRequest* rqReqP, xmlNodePtr node, int level)
{
	char*        contentP;
    std::string  content;
	char         tab[182];
	int          ix;
	std::string  path;

	for (ix = 0; ix < (level + 1) * 2; ix++)
		tab[ix] = ' ';
	tab[ix] = 0;

	path = "";
	for (ix = 0; ix <= level; ix++)
	{
		if (path != "")
			path += ".";
		path = path + nodename[ix];
	}

	if (node->children == NULL)
	{
		contentP = (char*) xmlNodeGetContent(node);
        contentP = wsClean(contentP);
		if (contentP == NULL)
            return;

        content = std::string(contentP);
        if ((level == 6) && (nodename[4] == "entityId"))
        {
            if      (nodename[5] == "id")              rqReqP->entityV[entityIdIx]->id = content;
            else if (nodename[5] == "type")            rqReqP->entityV[entityIdIx]->type = content;
            else if (nodename[5] == "isPattern")       rqReqP->entityV[entityIdIx]->isPattern = (content == "true")? true : false;
            else
                    printf("Nothing handled 1 for %s (level %d)\n", path.c_str(), level);
        }
        else if ((level == 7) && (nodename[3] == "contextRegistrationAttributeList") && (nodename[5] == "contextMetadata"))
        {
            if      (nodename[6] == "name")  rqReqP->attributeList.metadataV[cralMetaIx]->name  = content;
            else if (nodename[6] == "type")  rqReqP->attributeList.metadataV[cralMetaIx]->type  = content;
            else if (nodename[6] == "value") rqReqP->attributeList.metadataV[cralMetaIx]->value = content;
            else printf("Nothing handled 2 for %s (level %d)\n", path.c_str(), level);
        }
        else if ((level == 5) && (nodename[3] == "contextRegistrationAttributeList"))
        {
            if      (nodename[4] == "name")       rqReqP->attributeList.name     = content;
            else if (nodename[4] == "type")       rqReqP->attributeList.type     = content;
            else if (nodename[4] == "isDomain")   rqReqP->attributeList.isDomain = (content == "true")? true : false;
            else printf("Nothing handled 3 for %s (level %d)\n", path.c_str(), level);
        }
        else if ((level == 6) && (nodename[3] == "registrationMetaData") && (nodename[4] == "contextMetadata"))
        {
            if      (nodename[5] == "name")  rqReqP->registrationMetadataV[rMetaIx]->name  = content;
            else if (nodename[5] == "type")  rqReqP->registrationMetadataV[rMetaIx]->type  = content;
            else if (nodename[5] == "value") rqReqP->registrationMetadataV[rMetaIx]->value = content;
            else printf("Nothing handled for %s (level %d)\n", path.c_str(), level);
        }
        else if ((level == 4) && (nodename[3] == "providingApplication"))
            rqReqP->providingApplication = content;
        else if ((level == 2) && (nodename[1] == "duration"))
            rqReqP->duration = content;
        else if ((level == 2) && (nodename[1] == "registrationId"))
            rqReqP->registrationId = content;
        else
            printf("Nothing handled for %s (level %d)\n", path.c_str(), level);
	}
	else
	{
		if (path == "registerContextOldRequest.contextRegistrationList.contextRegistration.entityIdList.entityId")
		{
            rqReqP->entityV.push_back(new Entity());
			++entityIdIx;
		}
		else if ((strcmp((char*) node->name, "contextMetadata") == 0) && (nodename[3] == "contextRegistrationAttributeList"))
		{
            rqReqP->attributeList.metadataV.push_back(new Metadata());
            ++cralMetaIx;
        }
		else if ((strcmp((char*) node->name, "contextMetadata") == 0) && (nodename[3] == "registrationMetaData"))
        {
            rqReqP->registrationMetadataV.push_back(new Metadata());
			++rMetaIx;
        }
	}
}



/* ****************************************************************************
*
* registerContextOldRequestParse - 
*/
RegisterContextRequest* registerContextOldRequestParse(xmlNodePtr node0P)
{
    xmlNodePtr node1P;
    xmlNodePtr node2P;
    xmlNodePtr node3P;
    xmlNodePtr node4P;
    xmlNodePtr node5P;
    xmlNodePtr node6P;
    xmlNodePtr node7P;
    RegisterContextRequest* rqReqP = new RegisterContextRequest();

    entityIdIx  = -1;
    cralMetaIx  = -1;
    rMetaIx     = -1;

    while (node0P)
    {
        nodename[0] = (char*) node0P->name;
	   
        registerContextOldRequestNode(rqReqP, node0P, 0);

        for (node1P = node0P->children; node1P != NULL; node1P = node1P->next)
        {
            nodename[1] = (char*) node1P->name;
            registerContextOldRequestNode(rqReqP, node1P, 1);

            for (node2P = node1P->children; node2P != NULL; node2P = node2P->next)
            {
                nodename[2] = (char*) node2P->name;
                registerContextOldRequestNode(rqReqP, node2P, 2);

                for (node3P = node2P->children; node3P != NULL; node3P = node3P->next)
                {
                    nodename[3] = (char*) node3P->name;
                    registerContextOldRequestNode(rqReqP, node3P, 3);
				   
                    for (node4P = node3P->children; node4P != NULL; node4P = node4P->next)
                    {
                        nodename[4] = (char*) node4P->name;
                        registerContextOldRequestNode(rqReqP, node4P, 4);

                        for (node5P = node4P->children; node5P != NULL; node5P = node5P->next)
                        {
                            nodename[5] = (char*) node5P->name;
                            registerContextOldRequestNode(rqReqP, node5P, 5);

                            for (node6P = node5P->children; node6P != NULL; node6P = node6P->next)
                            {
                                nodename[6] = (char*) node6P->name;
                                registerContextOldRequestNode(rqReqP, node6P, 6);

                                for (node7P = node6P->children; node7P != NULL; node7P = node7P->next)
                                {
                                    nodename[7] = (char*) node7P->name;
                                    registerContextOldRequestNode(rqReqP, node7P, 7);
                                }
                            }
                        }
                    }
                }
            }
        }

        node0P = node0P->next;
    }

    return rqReqP;
}
