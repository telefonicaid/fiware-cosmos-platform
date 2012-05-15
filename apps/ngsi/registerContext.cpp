#include <string>
#include <libxml/tree.h>

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "globals.h"                        // TF
#include "ws.h"                             // whitespace functions
#include "Format.h"                         // Format
#include "Verb.h"                           // Verb
#include "rest.h"                           // allow, restReply
#include "httpData.h"                       // httpDataLines, httpData
#include "duration.h"                       // durationStringToSeconds
#include "RegisterContextRequest.h"         // RegisterContextRequest
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
       rcReqP->attributeList.attributeV[0]->name = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.type")
       rcReqP->attributeList.attributeV[0]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.isDomain")
       rcReqP->attributeList.attributeV[0]->isDomain = (content == "true")? true : false;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.name")
       rcReqP->attributeList.attributeV[0]->metadataV[cralMetaIx]->name = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.type")
       rcReqP->attributeList.attributeV[0]->metadataV[cralMetaIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.value")
       rcReqP->attributeList.attributeV[0]->metadataV[cralMetaIx]->value = content;

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
       rcReqP->attributeList.attribute[0]->metadataV.push_back(new Metadata());
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

	rcReqP->attributeList.attributeV.push_back(new ContextRegistrationAttribute());
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
static RegisterContextRequest* registerContextRequestParse(xmlNodePtr nodeP)
{
    RegisterContextRequest* rcReqP = new RegisterContextRequest();

	LM_T(LmtRegisterContext, ("parsing XML data for a context registration"));

    entityIdIx  = -1;
    cralMetaIx  = -1;
    rMetaIx     = -1;

    registerContextRequest(rcReqP, nodeP, 0);

    return rcReqP;
}



/* ****************************************************************************
*
* registerContextRequestTreat - add/update context entities
*
* RETURN VALUE
*  o true     means that a REST Response has been sent, not that the request was correctly treated
*
* It's either ADD or UPDATE, no mix.
* If an 'update' request comes in with one entity that exists and another that doesn't,
* nothing is done and an error is flagged
* 
*/
static bool registerContextRequestTreat(RegisterContextRequest* rcrP)
{
	unsigned int  ix;
	std::string   registrationId;
	int           duration = 0;
	bool          isUpdate = false;

	if (rcrP->registrationId == "")
	{
		char s[64];
		
		registrationId = std::string(registrationIdGet(s, sizeof(s)));
	}
	else
	{
		LM_M(("Got a registerContextRequest with a registrationId - treat like update ..."));
		isUpdate       = true;
		registrationId = rcrP->registrationId;

		registrationP = registrationLookup(registrationId);
		if (registrationP == NULL)
		{
			restReply(fd, format, 474, "error", "registration id not found");
			return true;
		}
	}
		
	if (rcrP->duration != "")
	{
		duration = durationStringToSeconds(rcrP->duration);
	}


	//
	// if 'update' - check that all entities exist AND that they have the correct registration id
	//
	if (isUpdate)
	{
		for (ix = 0; ix < rcrP->entityV.size(); ix++)
		{
			Entity* entityP;

			if ((entityP = entityLookup(rcrP->entityV[ix]->id, rcrP->entityV[ix]->type)) == NULL)
			{
				restReply(fd, format, 404, "error", "entity not found");
				LM_RE(true, ("Cannot find entity with id '%s and type '%s'", rcrP->entityV[ix]->id, rcrP->entityV[ix]->type));
			}

			if (entityP->registrationId != registrationId)
			{
				restReply(fd, format, 404, "error", "bad registration id");
				LM_RE(true, ("Entity with id '%s and type '%s' has registration id '%s'. Incoming registration id was: '%s'", entityP->id.c_str(), entityP->type.c_str(), entityP->registrationId.c_str(), registrationId.c_str()));
			}
		}
	}


	//
	// if NOT 'update' - check that no entities exist
	//
	if (!isUpdate)
	{
		for (ix = 0; ix < rcrP->entityV.size(); ix++)
		{
			if (entityLookup(rcrP->entityV[ix]->id, rcrP->entityV[ix]->type) != NULL)
			{
				restReply(fd, format, 404, "error", "entity already exists and no registration id supplied");
				LM_RE(true, ("entity with id '%s and type '%s' does already exist", rcrP->entityV[ix]->id, rcrP->entityV[ix]->type));
			}
		}
	}


	//
	// Now treat the request
	//
	for (ix = 0; ix < rcrP->entityV.size(); ix++)
	{
		LM_T(LmtEntity, ("Adding entity '%s'", rcrP->entityV[ix]->id.c_str()));
		if (isUpdate)
			entityUpdate(rcrP->entityV[ix]->id, rcrP->entityV[ix]->type, rcrP->entityV[ix]->isPattern, rcrP->providingApplication, duration, registrationId, &rcrP->attributeList);
		else
			entityAdd(rcrP->entityV[ix]->id,    rcrP->entityV[ix]->type, rcrP->entityV[ix]->isPattern, rcrP->providingApplication, duration, registrationId, &rcrP->attributeList);
	}
		

	//
	// Save the registration info
	//
	registrationAdd(registrationId, rcrP->registrationMetadataV);

	restReply(fd, format, 200, "status", "OK");
	return true;
}



#if 0
/* ****************************************************************************
*
* registerContextRequestPresent - 
*/
static void registerContextRequestPresent(RegisterContextRequest* rcrP)
{
    unsigned int ix;

    printf("%d entities:\n", (int) rcrP->entityV.size());
    for (ix = 0; ix < rcrP->entityV.size(); ix++)
    {
        printf("  Entity[%d]->id:        '%s'\n", ix, rcrP->entityV[ix]->id.c_str());
        printf("  Entity[%d]->type:      '%s'\n", ix, rcrP->entityV[ix]->type.c_str());
        printf("  Entity[%d]->isPattern: %s\n", ix, (rcrP->entityV[ix]->isPattern == true)? "true" : "false");
    }
    printf("\n");
    
    printf("Attribute List:\n");
    printf("  name:     '%s'\n", rcrP->attributeList.name.c_str());
    printf("  type:     '%s'\n", rcrP->attributeList.type.c_str());
    printf("  isDomain:  %s\n", (rcrP->attributeList.isDomain == true)? "true" : "false");
    printf("  %d metadatas:\n", (int) rcrP->attributeList.metadataV.size());

    for (ix = 0; ix < rcrP->attributeList.metadataV.size(); ix++)
    {
        printf("    Metadata[%d]->name:     '%s'\n", ix, rcrP->attributeList.metadataV[ix]->name.c_str());
        printf("    Metadata[%d]->type:     '%s'\n", ix, rcrP->attributeList.metadataV[ix]->type.c_str());
        printf("    Metadata[%d]->value:    '%s'\n", ix, rcrP->attributeList.metadataV[ix]->value.c_str());
    }
    printf("\n");

    printf("%d Registration Metadata:\n", (int) rcrP->registrationMetadataV.size());
    for (ix = 0; ix < rcrP->registrationMetadataV.size(); ix++)
    {
        printf("    Metadata[%d]->name:     '%s'\n", ix, rcrP->registrationMetadataV[ix]->name.c_str());
        printf("    Metadata[%d]->type:     '%s'\n", ix, rcrP->registrationMetadataV[ix]->type.c_str());
        printf("    Metadata[%d]->value:    '%s'\n", ix, rcrP->registrationMetadataV[ix]->value.c_str());
    }
    printf("\n");

    printf("Providing Application:   '%s'\n", rcrP->providingApplication.c_str());
    printf("Duration:                '%s'\n", rcrP->duration.c_str());
    printf("Registration Id:         '%s'\n", rcrP->registrationId.c_str());
}
#endif



/* ****************************************************************************
*
* registerContextXmlDataParse - 
*/
static bool registerContextXmlDataParse(int fd, Format format, std::string buf)
{
    xmlDocPtr                doc;
    xmlNodePtr               node;
    RegisterContextRequest*  rcrP;
	bool                     ret = true;

    doc  = xmlParseDoc((const xmlChar*) buf.c_str());
    node = doc->children;

    if (strcmp((char*) node->name, "registerContextRequest") != 0)
	{
		LM_E(("XML Data should be 'registerContextRequest'. Was '%s'", (char*) node->name));
		restReply(fd, format, 404, "status", "XML Data should be 'registerContextRequest'");
		ret = false;
	}
	else
	{
		rcrP = registerContextRequestParse(node);
		if (rcrP != NULL)
		{
			if (registerContextRequestTreat(rcrP) != true)  // if OK, responds to REST request
			{
				restReply(fd, format, 500, "status", "XML data treat error");
				ret = false;
			}
		}
		else
		{
			restReply(fd, format, 500, "status", "XML data parse error");
			ret = false;
		}
	}

    xmlFreeDoc(doc);

	return ret;
}



/* ****************************************************************************
*
* registerContext - 
*/
bool registerContext(int fd, Verb verb, Format format, char* data)
{
	LM_T(LmtOperation, ("registerContext: %s", verbName(verb)));

	if (verb == POST)
    {
		if (httpDataLines == 0)
		{
			LM_W(("No data for registerContext'"));
			return restReply(fd, format, 404, "status", "No XML data");
		}
		else if (httpDataLines > 1)
		{
			LM_W(("More than one data line for registerContext (%d)", httpDataLines));
			return restReply(fd, format, 404, "status", "Only one line of XML data allowed");
		}

		return registerContextXmlDataParse(fd, format, data);
    }
        
	allow = "POST";
    return restReply(fd, format, 405, "error", "BAD VERB");
}
