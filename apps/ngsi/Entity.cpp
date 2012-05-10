/* ****************************************************************************
*
*  FILE                 Entity.cpp -  
*
*
*
*
*/
#include <string.h>                         // strcmp, ...
#include <string>                           // std::string
#include <stdlib.h>                         // malloc
#include <errno.h>                          // errno

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "jsonParse.h"                      // JSON parsing function
#include "httpData.h"                       // httpData
#include "rest.h"                           // restReply
#include "Attribute.h"                      // Attribute
#include "Entity.h"                         // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
static Entity*       entityV          = NULL;
static int           entities         = 0;



/* ****************************************************************************
*
* entityInit - 
*/
void entityInit(void)
{
	entityV  = NULL;
	entities = 0;
}



/* ****************************************************************************
*
* entityLookup -
*/
static Entity* entityLookup(std::string id, std::string type)
{
	Entity* entityP = entityV;

	while (entityP != NULL)
	{
		if ((entityP->id == id) && (entityP->type == type))
			return entityP;

		entityP = entityP->next;
	}

	return NULL;
}



/* ****************************************************************************
*
* entityCreate - 
*/
static Entity* entityCreate(std::string id, std::string type, bool isPattern, std::string providingApplication, int duration, std::string registrationId)
{
	Entity* entity = new Entity();

	if (entity == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	entity->id                    = id;
	entity->type                  = type;
	entity->isPattern             = isPattern;
	entity->providingApplication  = providingApplication;
	entity->startTime             = time(NULL);
	entity->endTime               = entity->startTime + duration;
	entity->registrationId        = registrationId;

	entity->next      = NULL;

	LM_T(LmtEntity, ("Created an entity struct for '%s' of %d bytes", entity->id.c_str(), sizeof(Entity)));

	return entity;
}



Entity* entityNext = NULL;
/* ****************************************************************************
*
* entityAppend - 
*/
static void entityAppend(Entity* entity)
{
	LM_T(LmtEntity, ("Appending entity '%s'", entity->id.c_str()));

	if (entityNext == NULL)
		entityV = entity;
	else
		entityNext->next = entity;

	entityNext = entity;
}



/* ****************************************************************************
*
* entityAdd - 
*/
Entity* entityAdd(std::string id, std::string type, bool isPattern, std::string providingApplication, int duration, std::string registrationId, ContextRegistrationAttributeList* attributeList)
{
	Entity* entityP;

	//
	// Lookup the entity 'id'
	//
	LM_T(LmtEntity, ("Looking up entity '%s'", id.c_str()));
	entityP = entityLookup(id, type);
	if (entityP != NULL)
		LM_RE(NULL, ("Entity '%s' of type '%s' already exists", id.c_str(), type.c_str()));

	entityP = entityCreate(id, type, isPattern, providingApplication, duration, registrationId);
	entityAppend(entityP);

	for (int ix = 0; ix < attributeList->attributeV.size(); ix+)
		attributeAdd(entityP, attributeList->attributeV[ix]);

    return entityP;
}



/* ****************************************************************************
*
* entityUpdate - 
*/
Entity* entityUpdate(std::string id, std::string type, bool isPattern, std::string providingApplication, int duration, std::string registrationId, ContextRegistrationAttributeList* attributeList)
{
	Entity* entityP;

	//
	// Lookup the entity 'id'
	//
	LM_T(LmtEntity, ("Looking up entity '%s'", id.c_str()));
	entityP = entityLookup(id, type);
	if (entityP == NULL)
		LM_RE(NULL, ("Entity '%s' of type '%s' doesn't exist", id.c_str(), type.c_str()));

	//
	// Change the values of the entity
	//
	if (providingApplication != "")
		entityP->providingApplication = providingApplication;
	
	if (duration != 0)
	{
		int endTime;

		//
		// What I do with 'startTime' ... ?
		// Seems more logical to keep the old start time
		//
		
		entityP->endTime = time(NULL) + duration;
	}



	//
	// Update or add attributes
	//
	for (int ix = 0; ix < attributeList->attributeV.size(); ix+)
	{
		Attribute* aP;
		Attribute* attribute;

		aP        = attributeList->attributeV[ix];
		attribute = attributeLookup(aP->name, aP->type, aP->ID);

		attributeAdd(entityP, attributeList->attributeV[ix]);
	}

    return entityP;
}
