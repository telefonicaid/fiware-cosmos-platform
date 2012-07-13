#include <libxml/tree.h>
#include <string>
#include <sstream>                          // std::ostringstream
#include <libxml/tree.h>

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "ws.h"                             // whitespace functions
#include "Scope.h"                          // Scope
#include "Format.h"                         // Format
#include "Verb.h"                           // Verb
#include "rest.h"                           // allow, restReply
#include "httpData.h"                       // httpDataLines, httpData
#include "Registration.h"                   // Registration
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

    LM_T(LmtRestPath, ("Path: %s", path.c_str()));

    contentP = wsClean(contentP);
    if (contentP != NULL)
        content  = std::string(contentP);

    if (path == "discoverContextAvailabilityRequest.entityIdList.entityId.id")
    {
		if (dcarP->entityV[entityIdIx]->id != "")
            LM_W(("overwriting 'id' for entity %d (old value: '%s')", entityIdIx, dcarP->entityV[entityIdIx]->id.c_str()));

		LM_T(LmtDiscover, ("*Setting id to '%s' for entity id %d", content.c_str(), entityIdIx));
		dcarP->entityV[entityIdIx]->id = content;
	}
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
        LM_T(LmtEntity, ("*** Got a new Entity: content='%s'", content.c_str()));

        dcarP->entityV.push_back(new Entity());
        ++entityIdIx;

        char* id        = (char*) xmlGetProp(nodeP, (xmlChar*) "id");
        char* type      = (char*) xmlGetProp(nodeP, (xmlChar*) "type");
        char* isPattern = (char*) xmlGetProp(nodeP, (xmlChar*) "isPattern");

        LM_T(LmtRegister, ("attribute entityId.id:        '%s'", id));
        LM_T(LmtRegister, ("attribute entityId.type:      '%s'", type));
        LM_T(LmtRegister, ("attribute entityId.isPattern: '%s'", isPattern));

        if (id != NULL)
        {
            LM_T(LmtRegister, ("Setting id to '%s' for entity id %d", id, entityIdIx));
            dcarP->entityV[entityIdIx]->id = id;
        }

        if (type != NULL)
        {
            LM_T(LmtRegister, ("Setting type to '%s' for entity id %d", type, entityIdIx));
            dcarP->entityV[entityIdIx]->type = type;
        }

        if (isPattern != NULL)
        {
            LM_T(LmtRegister, ("Setting isPattern to '%s' for entity id %d", isPattern, entityIdIx));
            dcarP->entityV[entityIdIx]->isPattern = (isPattern == std::string("false"))? false : true;
        }
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
* discoverContextAvailabilityRequestPresent - 
*/
static void discoverContextAvailabilityRequestPresent(DiscoverContextAvailabilityRequest* dcarP)
{
    unsigned int ix;

	printf("\n\ndiscoverContextAvailabilityRequest Tree:\n");
	printf("--------------------------------------------------------\n");
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



/* ****************************************************************************
*
* discoverContextAvailabilityRequestParse - 
*/
static DiscoverContextAvailabilityRequest* discoverContextAvailabilityRequestParse(xmlNodePtr nodeP)
{
    DiscoverContextAvailabilityRequest* dcarP = new DiscoverContextAvailabilityRequest();

    entityIdIx  = -1;
    attributeIx = -1;
    scopeIx     = -1;

    discoverContextAvailabilityRequest(dcarP, nodeP, 0);
    discoverContextAvailabilityRequestPresent(dcarP);

    return dcarP;
}



/* ****************************************************************************
*
* errorCodeAdd - 
*/
static std::string errorCodeAdd(int code, std::string reasonPhrase, std::string details)
{
	char codeV[32];
	std::string s;

	sprintf(codeV, "%d", code);

	s  = std::string("<errorCode>");
	s += std::string("<code>") + codeV + "</code>";
	s += std::string("<reasonPhrase>") + reasonPhrase + "</reasonPhrase>";

	if (details != "")
		s += std::string("<details>") + details + "</details>";

	s += std::string("</errorCode>");

	return s;
}



/* ****************************************************************************
*
* entityAddToResponse - 
*/
std::string entityAddToResponse(Entity* entityP)
{
	std::string s = "<entityId";

	s += std::string(" type=\"") + entityP->type + "\"";
	s += std::string(" isPattern=\"") + ((entityP->isPattern == true)? "true" : "false") + "\"" + ">";
	s += std::string("<id>") + entityP->id + "</id>";
	s += std::string("</entityId>");

	return s;
}



/* ****************************************************************************
*
* attributeAddToResponse - 
*/
std::string attributeAddToResponse(Attribute* attributeP)
{
	std::string s = "<contextRegistrationAttribute>";

	s += std::string("<name>")     + attributeP->name + "</name>"; 
	s += std::string("<type>")     + attributeP->type + "</type>"; 
	s += std::string("<isDomain>") + ((attributeP->isDomain == true)? "true" : "false") + "</isDomain>"; 

	Metadata* metadata = metadataList;

	s += std::string("<metaData>");
	while (metadata != NULL)
	{
		if (metadata->attributeP == attributeP)
		{
			s += std::string("<contextMetadata>");
			s += std::string("<name>")  + metadata->name  + "</name>";
			s += std::string("<type>")  + metadata->type  + "</type>";
			s += std::string("<value>") + metadata->value + "</value>";
			s += std::string("</contextMetadata>");
		}

		metadata = metadata->next;
	}

	s += std::string("</metaData>");
	s += std::string("</contextRegistrationAttribute>");

	return s;
}



/* ****************************************************************************
*
* discoverContextAvailabilityResponse - 
*/
static bool discoverContextAvailabilityResponse(int fd, Format format, DiscoverContextAvailabilityRequest* dcarP)
{
    int                  entities = dcarP->entityV.size();
	std::ostringstream   output;
	vector<Attribute*>   attributeV;
	int                  attributes;
	bool                 attributeInAllEntities;
	Entity*              entity0P;
	Entity*              entityP;

	output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	output << "<discoverContextAvailabilityResponse>";
	

	//
	// Sanity check - one or more entities in the request?
	//
	if (dcarP->entityV.size() == 0)
	{
		LM_E(("No entities is discovery request"));
        output << errorCodeAdd(400, std::string("no entities"), std::string("No entities in request - this is NOT optional"));
        output << "</discoverContextAvailabilityResponse>";
        restReplySend(fd, format, 400, output.str().c_str());
		
		return false;
	}



	//
	// Lookup first entity
	//
	entity0P = entityLookup(dcarP->entityV[0]->id, dcarP->entityV[0]->type);
	if (entity0P == NULL)
	{
		output << errorCodeAdd(400, std::string("entity not found"), std::string("entity '") + dcarP->entityV[0]->id + "' not found");
		output << "</discoverContextAvailabilityResponse>";
		restReplySend(fd, format, 400, output.str().c_str());

		return true;
	}



	//
	// Sanity check - do all entites have the same providing app?
	//                AND registration ID !
	//
	if (entities > 1)
	{
		for (int eIx = 1; eIx < entities; eIx++)
		{
			entityP = entityLookup(dcarP->entityV[eIx]->id, dcarP->entityV[eIx]->type);
			if (entityP == NULL)
			{
				output << errorCodeAdd(400, std::string("entity not found"), std::string("entity '") + dcarP->entityV[eIx]->id + "' not found");
				output << "</discoverContextAvailabilityResponse>";
				restReplySend(fd, format, 400, output.str().c_str());
				return true;
			}

			if (entityP->registrationId != entity0P->registrationId)
			{
				output << errorCodeAdd(400, std::string("incompatible entities"), std::string("registration id differs for the entities"));
				output << "</discoverContextAvailabilityResponse>";
				restReplySend(fd, format, 400, output.str().c_str());
				return true;
			}

			if (entityP->providingApplication != entity0P->providingApplication)
			{
				output << errorCodeAdd(400, "incompatible entities", "providing application differs for the input entities");
				output << "</discoverContextAvailabilityResponse>";
				restReplySend(fd, format, 400, output.str().c_str());
				return true;
			}
		}
	}


	//
	// Sanity check - is the registration found in our vector?
	//
	Registration* registration = registrationLookup(entity0P->registrationId);
	if (registration == NULL)
	{
		LM_E(("cannot find registration id '%s'", entity0P->registrationId.c_str()));
		output << errorCodeAdd(500, "internal error", "registration not found for entity");
		output << "</discoverContextAvailabilityResponse>";
		restReplySend(fd, format, 500, output.str().c_str());
		return true;
	}


	//
	// Continue the header down to 'entityIdList'
	//
	output << "<contextRegistrationResponseList>";
	output << "<contextRegistrationResponse>";
	output << "<contextRegistration>";



	//
	// Entities at the head of the response XML 
	//
	output << "<entityIdList>";
    for (int eIx = 0; eIx < entities; eIx++)
		output << entityAddToResponse(dcarP->entityV[eIx]);
	output << "<entityIdList>";

	

	//
	// The list of attributes for the response - only those all entities have ...
	//
	// Loop through all attributes for first entity in the list.
	// Keep only the attributes that are common for all entities
	//
	output << "<contextRegistrationAttributeList>";
	attributes = dcarP->attributeV.size();
    for (int aIx = 0; aIx < attributes; aIx++)
	{
		int         attributeNo;
		std::string attributeName = dcarP->attributeV[aIx];
		Entity*     entityP;
		Attribute*  attribute;

		attributeInAllEntities = true;
		attributeNo            = 0;
		for (int eIx = 0; eIx < entities; eIx++)
		{
			entityP = entityLookup(dcarP->entityV[eIx]->id, dcarP->entityV[eIx]->type);
			if (entityP == NULL)
			{
				LM_W(("Entity '%s:%s' not found", dcarP->entityV[eIx]->type.c_str(), dcarP->entityV[eIx]->id.c_str()));
				attributeInAllEntities = false;
				break;
			}

			attribute = attributeLookup(entityP, attributeName);
			if (attribute == NULL)
			{
				attributeInAllEntities = false;
				break;
			}
		}

		if (attributeInAllEntities == true)
			output << attributeAddToResponse(attribute);
	}
	output << "</contextRegistrationAttributeList>";



	//
	// Now, the metadata of the registration ...
	// Here I take the values for Entity 0, later I should check that the other entities do not differ ...
	//
	unsigned int rmIx;

	output << "<registrationMetaData>";
	for (rmIx = 0; rmIx < registration->metadataV.size(); rmIx++)
	{
	}
	output << "</registrationMetaData>";



	//
	// Terminating the response
	//
	output << "<providingApplication>" << entity0P->providingApplication << "</providingApplication>";
	output << "</contextRegistration>";
	output << "</contextRegistrationResponse>";
	output << "</contextRegistrationResponseList>";
	output << errorCodeAdd(200, "Ok", "No problem");
	output << "</discoverContextAvailabilityResponse>";

	restReplySend(fd, format, 200, output.str().c_str());

	return true;
}



/* ****************************************************************************
*
* discoverContextAvailabilityRequestTreat - 
*/
static bool discoverContextAvailabilityRequestTreat(int fd, Format format, DiscoverContextAvailabilityRequest* dcarP)
{
	int entities = dcarP->entityV.size();
	LM_T(LmtDiscoverTreat, ("Treating discovery of %d entities", dcarP->entityV.size()));

	if (entities == 0)
		LM_E(("no entities in request"));
	return discoverContextAvailabilityResponse(fd, format, dcarP);
}



/* ****************************************************************************
*
* discoverContextAvailabilityXmlDataParse - 
*/
static bool discoverContextAvailabilityXmlDataParse(int fd, Format format, std::string buf)
{
    xmlDocPtr                            doc;
    xmlNodePtr                           node;
    DiscoverContextAvailabilityRequest*  dcarP;
	bool                                 ret = true;

    doc  = xmlParseDoc((const xmlChar*) buf.c_str());
    node = doc->children;

    if (strcmp((char*) node->name, "discoverContextAvailabilityRequest") != 0)
    {
		LM_E(("XML Data should be 'discoverContextAvailabilityRequest'. Was '%s'", (char*) node->name));
		restReply(fd, format, 404, "status", "XML Data should be 'discoverContextAvailabilityRequest'");
		ret = false;
	}
	else
	{
		dcarP = discoverContextAvailabilityRequestParse(node);
		if (dcarP != NULL)
		{
			if (discoverContextAvailabilityRequestTreat(fd, format, dcarP) != true) // if OK, responds to REST request
			{
				restReply(fd, format, 500, "status", "XML data treat error");
				ret = false;
			}
		}
		else
		{
			restReply(fd, format, 404, "status", "error parsing XML rquest data");
			ret = false;
		}
	}

	xmlFreeDoc(doc);

	return ret;
}



/* ****************************************************************************
*
* discoverContextAvailability - 
*/
bool discoverContextAvailability(int fd, Verb verb, Format format, char* data)
{
	LM_T(LmtOperation, ("discoverContextAvailability: %s", verbName(verb)));

	if (verb == POST)
    {
        LM_T(LmtOperation, ("discoverContextAvailability: POST"));

		if (httpDataLines == 0)
		{
			LM_W(("No data for discoverContextAvailability"));
			return restReply(fd, format, 404, "status", "No XML data");
		}
		else if (httpDataLines > 1)
		{
			LM_W(("More than one data line for discoverContextAvailability (%d)", httpDataLines));
			data = (char*) httpDataString.c_str();
		}

		return discoverContextAvailabilityXmlDataParse(fd, format, data);
    }
        
	allow = "POST";
    return restReply(fd, format, 405, "error", "BAD VERB");
}
