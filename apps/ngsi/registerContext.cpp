#include <sstream>                          // std::ostringstream
#include <string>                           // std::stream

#include <libxml/tree.h>                    // XML library

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "globals.h"                        // TF
#include "database.h"                       // dbRegistrationAdd
#include "ws.h"                             // whitespace functions
#include "Format.h"                         // Format
#include "Verb.h"                           // Verb
#include "rest.h"                           // allow, restReply
#include "httpData.h"                       // httpDataLines, httpData
#include "duration.h"                       // durationStringToSeconds
#include "RegisterContextRequest.h"         // RegisterContextRequest
#include "Attribute.h"                      // Attribute
#include "Registration.h"                   // Registration, registrationIdGet, ...
#include "Entity.h"                         // Entity, entityToDb
#include "registerContext.h"                // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
static int          entityIdIx         = -1;
static int          craIx              = -1;
static int          cralMetaIx         = -1;
static int          rMetaIx            = -1;
static std::string  lastRegistrationId = "";



/* ****************************************************************************
*
* registerContextResponse - 
*/
static bool registerContextResponse(int fd, Format format, int httpCode = 200, const char* duration = NULL, int errorCode = 0, const char* errorString = NULL, const char* errorDetails = NULL)
{
	std::ostringstream  output;

	//
	// Start of XML 
	//
	output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    output << "<registerContextResponse>";

	//
	// duration
	//
    if (duration != NULL)
		output << "<duration>" << duration << "</duration>";


	//
	// registrationId
	//
	if (lastRegistrationId == "")
	{
		LM_E(("Error - NULL registrationId - using 43 as substitute ..."));
		lastRegistrationId = "43";
	}
	output << "<registrationId>" << lastRegistrationId << "</registrationId>";
	lastRegistrationId = "";


	//
	// errorCode
	//
	if (errorCode != 0)
	{
		output << "<errorCode>";

		output << "<code>"         << errorCode    << "</code>";
		output << "<reasonPhrase>" << errorString  << "</reasonPhrase>";

		if (errorDetails != NULL)
			output << "<details>"      << errorDetails << "</details>";

		output << "</errorCode>";
	}



	//
	// End of XML
	//
	output << "</registerContextResponse>";



	//
	// Now send it
	//
	return restReplySend(fd, format, httpCode, output.str().c_str());
}



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

	LM_T(LmtRestPath, ("Path: %s", path.c_str()));

    contentP = wsClean(contentP);
    if (contentP != NULL)
        content  = std::string(contentP);

    if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId.id")
    {
		LM_M(("Setting id to '%s' for entity id %d", content.c_str(), entityIdIx));
		rcReqP->entityV[entityIdIx]->id = content;
    }
	else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId.type")
    {
		LM_M(("Setting type to '%s' for entity id %d", content.c_str(), entityIdIx));
        rcReqP->entityV[entityIdIx]->type = content;
    }
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId.isPattern")
    {
		LM_M(("Setting isPattern to '%s' for entity id %d", content.c_str(), entityIdIx));
        rcReqP->entityV[entityIdIx]->isPattern = (content == "false")? false : true;
    }
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.name")
    {
		rcReqP->attributeList.attributeV[craIx]->name = content;

		for (int eIx = 0; eIx <= entityIdIx; eIx++)
			LM_M(("Add attribute: '%s' to entity '%s:%s'", content.c_str(), rcReqP->entityV[eIx]->id.c_str(), rcReqP->entityV[eIx]->type.c_str()));
	}
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.type")
       rcReqP->attributeList.attributeV[craIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.isDomain")
       rcReqP->attributeList.attributeV[craIx]->isDomain = (content == "true")? true : false;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.name")
    {
		LM_M(("metadata '%s' belongs to attribute '%s:%s'",
			  content.c_str(),
			  rcReqP->attributeList.attributeV[craIx]->name.c_str(),
			  rcReqP->attributeList.attributeV[craIx]->type.c_str()));

			  rcReqP->attributeList.attributeV[craIx]->metadataV[cralMetaIx]->name = content;
    }
	else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.type")
       rcReqP->attributeList.attributeV[craIx]->metadataV[cralMetaIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata.value")
       rcReqP->attributeList.attributeV[craIx]->metadataV[cralMetaIx]->value = content;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata.name")
       rcReqP->registrationMetadataV[rMetaIx]->name = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata.type")
       rcReqP->registrationMetadataV[rMetaIx]->type = content;
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.registrationMetaData.contextMetadata.value")
       rcReqP->registrationMetadataV[rMetaIx]->value = content;

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.providingApplication")
    {
		rcReqP->providingApplication = content;
		LM_M(("Got a providingApplication: %s", rcReqP->providingApplication.c_str()));
	}
    else if (path == "registerContextRequest.duration")
       rcReqP->duration = content;
    else if (path == "registerContextRequest.registrationId")
    {
		rcReqP->registrationId = content;
		lastRegistrationId     = content;
	}

    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.entityIdList.entityId")
    {
		rcReqP->entityV.push_back(new Entity());
		++entityIdIx;
		LM_M(("Pushed back Entity for vector slot %d", entityIdIx));

		if (content == "emptycontent")
		{
			char* id        = (char*) xmlGetProp(nodeP, (xmlChar*) "id");
			char* type      = (char*) xmlGetProp(nodeP, (xmlChar*) "type");
			char* isPattern = (char*) xmlGetProp(nodeP, (xmlChar*) "isPattern");

			LM_M(("attribute entityId.id:        '%s'", id));
			LM_M(("attribute entityId.type:      '%s'", type));
			LM_M(("attribute entityId.isPattern: '%s'", isPattern));

			rcReqP->entityV[entityIdIx]->id         = id;
			rcReqP->entityV[entityIdIx]->type       = type;
			rcReqP->entityV[entityIdIx]->isPattern  = isPattern;
		}
    }
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList")
	{
		rcReqP->attributeList.attributeV.push_back(new Attribute());
		++craIx;
	}
    else if (path == "registerContextRequest.contextRegistrationList.contextRegistration.contextRegistrationAttributeList.metaData.contextMetadata")
    {
       rcReqP->attributeList.attributeV[0]->metadataV.push_back(new Metadata());
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
static void registerContextRequest(RegisterContextRequest* rcReqP, xmlNodePtr nodeP, int level)
{
    xmlNodePtr child = nodeP->children;;

	registerContextRequestNode(rcReqP, nodeP, level);

	while (child != NULL)
	{
		registerContextRequest(rcReqP, child, level + 1);
		child = child->next;
	}
}



/* ****************************************************************************
*
* registerContextRequestToDatabase - 
*/
static int registerContextRequestToDatabase(RegisterContextRequest* rcReqP, bool isUpdate, std::string* errorString)
{
	unsigned int  eIx;
	unsigned int  aIx;
	unsigned int  mIx;
	unsigned int  entities;
	unsigned int  attributes;
	unsigned int  metadatas;
	int           s;
	Entity*       entityP;
	Attribute*    attributeP;

	LM_T(LmtDbRegReq, ("------------ To DB ------------"));
	entities = rcReqP->entityV.size();
	for (eIx = 0; eIx < entities; eIx++)
	{
		entityP = entityLookup(rcReqP->entityV[eIx]->id, rcReqP->entityV[eIx]->type);
		if (entityP == NULL)
		{
			LM_W(("Entity '%s:%s' not found - this is a bug!", rcReqP->entityV[eIx]->id.c_str(), rcReqP->entityV[eIx]->type.c_str()));
			continue;
		}

		if (isUpdate == true)
			LM_T(LmtDbRegReq, ("Entity { '%s', '%s', '%s' } NOT to DB - this is an update", entityP->id.c_str(), entityP->type.c_str(), TF(entityP->isPattern)));
		else
			LM_T(LmtDbRegReq, ("Entity { '%s', '%s', '%s' } to DB", entityP->id.c_str(), entityP->type.c_str(), TF(entityP->isPattern)));

		LM_M(("Sending entity %d to db. providing app: '%s'", eIx, entityP->providingApplication.c_str()));
		s = entityToDb(entityP, isUpdate, errorString);
		if (s != 0)
			LM_RE(-1, ("Error %s entity '%s:%s'", (isUpdate == true)? "updating" : "creating", entityP->id.c_str(), entityP->type.c_str()));

		attributes = rcReqP->attributeList.attributeV.size();
		for (aIx = 0; aIx < attributes; aIx++)
		{
			std::string metaId = "";

			metadatas = rcReqP->attributeList.attributeV[aIx]->metadataV.size();

			//
			// Peeking to find metadata 'ID'
			//
			for (mIx = 0; mIx < metadatas; mIx++)
			{
				Metadata* mP = rcReqP->attributeList.attributeV[aIx]->metadataV[mIx];

				if (mP->name == "ID")
					metaId = mP->value;
			}
			
			LM_T(LmtDbRegReq, ("Attribute { '%s', '%s' } to DB for the %d entities", rcReqP->attributeList.attributeV[aIx]->name.c_str(), rcReqP->attributeList.attributeV[aIx]->type.c_str(), entities));

			attributeP = attributeLookup(entityP, rcReqP->attributeList.attributeV[aIx]->name, rcReqP->attributeList.attributeV[aIx]->type, metaId);
			if (attributeP == NULL)
			{
				LM_W(("Attribute '%s:%s' not found - this is a bug!", rcReqP->attributeList.attributeV[aIx]->name.c_str(), rcReqP->attributeList.attributeV[aIx]->type.c_str()));
				continue;
			}
			
			s = attributeToDb(entityP, attributeP);
			for (mIx = 0; mIx < metadatas; mIx++)
			{
				Metadata* mP = rcReqP->attributeList.attributeV[aIx]->metadataV[mIx];
				metadataToDb(attributeP, mP);
			}
		}
	}

	return 0;
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
	craIx       = -1;
    cralMetaIx  = -1;
    rMetaIx     = -1;

    registerContextRequest(rcReqP, nodeP, 0);
    return rcReqP;
}



/* ****************************************************************************
*
* registerContextRequestTreat - add/update context entities
*
* This function always responds to the REST request 
*
* RETURN VALUE
*  o true     OK
*  o false    Error
*
* It's either ADD or UPDATE, never a mix.
* If an 'update' request comes in with one entity that exists and another that doesn't,
* nothing is done and an error is flagged
* 
*/
static bool registerContextRequestTreat(int fd, Format format, RegisterContextRequest* rcrP)
{
	unsigned int  ix;
	std::string   registrationId;
	int           duration = 0;
	bool          isUpdate = false;
	Entity*       entityP;
	std::string   errorString;

	if (rcrP->registrationId == "")
	{
		char s[64];
		
		registrationId     = std::string(registrationIdGet(s, sizeof(s)));
		if (registrationToDb(registrationId) != 0)
		{
			registerContextResponse(fd, format, 502, NULL, 502, "database error", "error adding a Registration to database");
			LM_RE(false, ("error adding a Registration to database"));
		}

		lastRegistrationId = registrationId;
	}
	else
	{
		Registration* registrationP;

		LM_M(("Got a registerContextRequest with a registrationId - treat like update ..."));
		isUpdate       = true;
		registrationId = rcrP->registrationId;

		registrationP = registrationLookup(registrationId);
		if (registrationP == NULL)
		{
			registerContextResponse(fd, format, 474, NULL, 474, "registration id not found", NULL);
			return false;
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
			if ((entityP = entityLookup(rcrP->entityV[ix]->id, rcrP->entityV[ix]->type)) == NULL)
			{
				errorString = "Entity '" + entityP->id + "', type '" + entityP->type + "' not found in an Update Request";
				registerContextResponse(fd, format, 404, NULL, 404, "entity not found", errorString.c_str());
				LM_RE(false, ("Cannot find entity with id '%s and type '%s'", rcrP->entityV[ix]->id.c_str(), rcrP->entityV[ix]->type.c_str()));
			}

			if (entityP->registrationId != registrationId)
			{
				errorString = "bad registration id " + registrationId + ". Expected " + entityP->registrationId + " for entity '" + entityP->id + "', type '" + entityP->type + "'";
				registerContextResponse(fd, format, 400, NULL, 400, "bad registration id", errorString.c_str());
				LM_RE(false, ("Entity with id '%s and type '%s' has registration id '%s'. Incoming registration id was: '%s'",
							  entityP->id.c_str(), entityP->type.c_str(), entityP->registrationId.c_str(), registrationId.c_str()));
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
				errorString = "Entity '" + rcrP->entityV[ix]->id + "', type '" + rcrP->entityV[ix]->type + "' already exists in a 'Create' Request";
				registerContextResponse(fd, format, 404, NULL, 404, "already exists", errorString.c_str());
				LM_RE(false, ("entity with id '%s and type '%s' already exists", rcrP->entityV[ix]->id.c_str(), rcrP->entityV[ix]->type.c_str()));
			}
		}
	}


	//
	// Now treat the request
	//
	for (ix = 0; ix < rcrP->entityV.size(); ix++)
	{
		std::string  errorString;

		LM_T(LmtEntity, ("Adding entity '%s'", rcrP->entityV[ix]->id.c_str()));
		if (isUpdate)
			entityP = entityUpdate(rcrP->entityV[ix], rcrP->providingApplication, duration, registrationId, &rcrP->attributeList, &errorString);
		else
			entityP = entityAdd(rcrP->entityV[ix],    rcrP->providingApplication, duration, registrationId, &rcrP->attributeList, &errorString);

		if (entityP == NULL)
		{
			registerContextResponse(fd, format, 404, NULL, 404, "error", errorString.c_str());
			LM_RE(false, ("entityUpdate/entityAdd error: %s", errorString.c_str()));
		}
	}
		

	//
	// Push request data to the DB
	//
	if (registerContextRequestToDatabase(rcrP, isUpdate, &errorString) != 0)
	{
		registerContextResponse(fd, format, 500, NULL, 500, "db error", errorString.c_str());
		LM_RE(false, ("error saving to database"));
	}



	//
	// Save the registration info
	//
	registrationAdd(registrationId, rcrP->registrationMetadataV);

	registerContextResponse(fd, format);
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
	else // OK
	{
		rcrP = registerContextRequestParse(node);
		if (rcrP != NULL)
		{
			ret = registerContextRequestTreat(fd, format, rcrP);
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
