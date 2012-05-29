/* ****************************************************************************
*
*  FILE                 Entity.cpp -  
*
*
*
*
*/
#include <string.h>                             // strcmp, ...
#include <string>                               // std::string
#include <stdlib.h>                             // malloc, strtoul, ...
#include <errno.h>                              // errno
#include <mysql.h>                              // mysql

#include "logMsg/logMsg.h"                      // LM_*

#include "traceLevels.h"                        // Trace levels for log msg library
#include "jsonParse.h"                          // JSON parsing function
#include "httpData.h"                           // httpData
#include "rest.h"                               // restReply
#include "Attribute.h"                          // Attribute
#include "ContextRegistrationAttributeList.h"   // ContextRegistrationAttributeList
#include "database.h"                           // db
#include "Entity.h"                             // Own interface

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

	LM_T(LmtEntity, ("Created an entity struct for '%s' of %d bytes. id='%s', type='%s', providingApplication='%s', registrationId='%s'",
					 entity->id.c_str(), sizeof(Entity), entity->id.c_str(), entity->type.c_str(), entity->providingApplication.c_str(), entity->registrationId.c_str()));

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
* entityLookupInDb - 
*/
Entity* entityLookupInDb(std::string id, std::string type)
{
	MYSQL_RES*   result;
	int          results;
	MYSQL_ROW    row;
	MYSQL_ROW    firstRow;
	int          rows;
	int          s;

	std::string query = "SELECT * from entity WHERE id = '" + id + "' AND type = '" + type + "'";

	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", mysql_error(db)));
		return NULL;
	}

	if ((result = mysql_store_result(db)) == 0)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return NULL;
	}

	results = mysql_num_fields(result);
	rows    = 0;
	while ((row = mysql_fetch_row(result)))
	{
		if (rows == 0)
			firstRow = row;
		++rows;
	}

	mysql_free_result(result);
	if (rows != 1)
	{
		char rowsV[30];

		sprintf(rowsV, "%d", rows);

		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return NULL;
	}

	bool    isPattern = (std::string(firstRow[3]) == "No")? false : true;
		
	Entity* entityP = entityCreate(firstRow[1], firstRow[2], isPattern, firstRow[4], atoi(firstRow[5]), firstRow[6]);

	return entityP;
}



/* ****************************************************************************
*
* entityLookup -
*/
Entity* entityLookup(std::string id, std::string type)
{
	Entity* entityP = entityV;

	while (entityP != NULL)
	{
		if ((entityP->id == id) && (entityP->type == type))
			return entityP;

		entityP = entityP->next;
	}

	entityP = entityLookupInDb(id, type);
	if (entityP == NULL)
		return NULL;

	entityAppend(entityP);
	return entityP;
}



/* ****************************************************************************
*
* entityAdd - 
*/
Entity* entityAdd
(
	Entity*                            rcrEntity,
	std::string                        providingApplication,
	int                                duration,
	std::string                        registrationId,
	ContextRegistrationAttributeList*  attributeList,
	std::string*                       errorString
)
{
	Entity*       entityP;
    unsigned int  ix;

	if (providingApplication == "")
	{
		*errorString = "Empty providing application";
		LM_RE(NULL, ("Empty providing application"));
	}

	if (rcrEntity->id == "")
	{
		*errorString = "Empty entity id";
		LM_RE(NULL, ("Empty Empty entity id"));
	}

	if (rcrEntity->type == "")
	{
		*errorString = "Empty entity type";
		LM_RE(NULL, ("Empty Empty entity type"));
	}

	//
	// Lookup the entity 'id'
	//
	LM_T(LmtEntity, ("Looking up entity '%s'", rcrEntity->id.c_str()));
	entityP = entityLookup(rcrEntity->id, rcrEntity->type);
	if (entityP != NULL)
		LM_RE(NULL, ("Entity '%s' of type '%s' already exists", rcrEntity->id.c_str(), rcrEntity->type.c_str()));



	//
	// Adding entity to global RAM collection
	//
	entityP = entityCreate(rcrEntity->id, rcrEntity->type, rcrEntity->isPattern, providingApplication, duration, registrationId);
	entityAppend(entityP);



	//
	// Updating rcrEntity, for this request
	//
	rcrEntity->providingApplication = entityP->providingApplication;
	rcrEntity->startTime            = entityP->startTime;
	rcrEntity->endTime              = entityP->endTime;
	rcrEntity->registrationId       = entityP->registrationId;



	for (ix = 0; ix < attributeList->attributeV.size(); ix++)
	{
		attributeAdd(entityP,    attributeList->attributeV[ix]);
		attributeAdd(rcrEntity,  attributeList->attributeV[ix]);
	}

    return entityP;
}



/* ****************************************************************************
*
* entityUpdate - 
*/
Entity* entityUpdate
(
	Entity*                            rcrEntity,
	std::string                        providingApplication,
	int                                duration,
	std::string                        registrationId,
	ContextRegistrationAttributeList*  attributeList,
	std::string*                       errorString
)
{
	Entity*       entityP;
    unsigned int  ix;

	if (rcrEntity->id == "")
	{
		*errorString = "Empty entity id";
		LM_RE(NULL, ("Empty Empty entity id"));
	}

	if (rcrEntity->type == "")
	{
		*errorString = "Empty entity type";
		LM_RE(NULL, ("Empty Empty entity type"));
	}

	//
	// Lookup the entity 'id'
	//
	LM_T(LmtEntity, ("Looking up entity '%s'", rcrEntity->id.c_str()));
	entityP = entityLookup(rcrEntity->id, rcrEntity->type);
	if (entityP == NULL)
		LM_RE(NULL, ("Entity '%s' of type '%s' doesn't exist", rcrEntity->id.c_str(), rcrEntity->type.c_str()));

	//
	// Change the values of the entity
	//
	if (providingApplication != "")
	{
		LM_W(("providingApplication changed from '%s' to '%s'", entityP->providingApplication.c_str(), providingApplication.c_str()));
		entityP->providingApplication   = providingApplication;
		rcrEntity->providingApplication = providingApplication;

	}
	
	if (duration != 0)
	{
		//
		// What I do with 'startTime' ... ?
		// Seems more logical to keep the old start time
		//
		
		entityP->endTime    = time(NULL) + duration;
		rcrEntity->endTime  = entityP->endTime;
	}



	//
	// Update or add attributes
	//
	for (ix = 0; ix < attributeList->attributeV.size(); ix++)
	{
		Attribute* aP;
		Attribute* attribute;

		aP        = attributeList->attributeV[ix];
		attribute = attributeLookup(aP->name, aP->type, aP->metaID);

		attributeAdd(entityP,   attributeList->attributeV[ix]);
		attributeAdd(rcrEntity, attributeList->attributeV[ix]);
	}

    return entityP;
}



/* ****************************************************************************
*
* entityExistsInDb - 
*/
static bool entityExistsInDb(Entity* entityP, std::string* errorString, bool* isPattern = NULL, std::string* providingApplication = NULL, int* endTime = NULL)
{
	int          s;
	MYSQL_RES*   result;
	int          results;
	MYSQL_ROW    row;
	MYSQL_ROW    firstRow;
	int          rows;

	// Check that the entity exists in DB 
	std::string query = "SELECT * from entity WHERE id = '" + entityP->id + "' AND type = '" + entityP->type + "'";
		
	LM_T(LmtDbEntity, ("SQL to verify that entity exists: %s", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", mysql_error(db)));
		*errorString = std::string("SQL error: ") + mysql_error(db);
		return false;
	}

	if ((result = mysql_store_result(db)) == 0)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		*errorString = std::string("mysql_store_result: ") + mysql_error(db);
		return false;
	}

	results = mysql_num_fields(result);
	rows    = 0;
	while ((row = mysql_fetch_row(result)))
	{
		if (rows == 0)
			firstRow = row;
		++rows;
	}

	if (isPattern != NULL)
	{
		if (strcmp(firstRow[3], "Yes") == 0)
			*isPattern = true;
		else
			*isPattern = false;

		if (providingApplication != NULL)
			*providingApplication = firstRow[4];

		if (endTime != NULL)
			*endTime = strtoul(firstRow[6], NULL, 10); 
	}

	mysql_free_result(result);
	if (rows != 1)
	{
		char rowsV[30];

		sprintf(rowsV, "%d", rows);

		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		*errorString = std::string(rowsV) + " rows for entity " + entityP->id + ":" + entityP->type;
		return false;
	}

	return true;
}



/* ****************************************************************************
*
* entityToDb - 
*/
int entityToDb(Entity* entityP, bool update, std::string* errorString)
{
	bool         isPattern = false;
	std::string  providingApplication;
	int          duration;
	int          s;

	if (update)
	{
		if (entityExistsInDb(entityP, errorString, &isPattern, &providingApplication, &duration) == false)
		{
			LM_W(("update for a non-existing entity (%s:%s)", entityP->id.c_str(), entityP->type.c_str()));
			return -1;
		}

		//
		// Now, entity exists in DB - let's update it, if necessary ... ?
		//
		if ((entityP->isPattern != isPattern) || (entityP->providingApplication != providingApplication) || duration != 0)
		{
			char endTime[64];

			LM_W(("diff in isPattern OR providingApplication - what do I do (send the change to DB or flag an error?)"));

			std::string isPat = (isPattern == true)? "Yes" : "No";


			std::string query = "UPDATE entity SET isPattern=" + isPat + ", providingApplication='" + providingApplication + "'";

			if (duration != 0)
			{
				LM_W(("What startTime should I use to get endTime ... ?   Old one or new one? New one!"));
				duration += entityP->startTime;
				query    += std::string(", endTime=") + endTime;
			}

			query += " WHERE id='" + entityP->id + "' AND type='" + entityP->type + "'";
			LM_T(LmtDbEntity, ("SQL to UPDATE an Entity: '%s'", query.c_str()));
			s = mysql_query(db, query.c_str());
			if (s != 0)
			{
				LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
				*errorString = std::string("SQL error: ") + mysql_error(db);
				return -1;
			}
		}
	}
	else // Creation of a new entity - make sure it doesn't exist first
	{
		char startTimeV[64];
		char endTimeV[64];

		if (entityExistsInDb(entityP, errorString) == true)
			LM_RE(-1, ("Entity '%s:%s' already exists in database", entityP->id.c_str(), entityP->type.c_str()));

		sprintf(startTimeV, "%ld", entityP->startTime);
		sprintf(endTimeV,   "%ld", entityP->endTime);

		if (entityP->providingApplication == "")
		{
			*errorString = std::string("No providing application");
			LM_RE(-1, ("No providing application"));
		}

		if (entityP->registrationId == "")
        {
            *errorString = std::string("No registrationId");
            LM_RE(-1, ("No registrationId"));
        }

		std::string query = "INSERT INTO entity (id, type, isPattern, providingApplication, startTime, endTime, registrationId) VALUES ('" + 
			entityP->id                                              + "', '" + 
			entityP->type                                            + "', '" + 
			std::string((entityP->isPattern == true)? "Yes" : "No")  + "', '" + 
			entityP->providingApplication                            + "', '" + 
			startTimeV                                               + "', '" + 
			endTimeV                                                 + "', '" + 
			entityP->registrationId                                  + "')";

		LM_T(LmtDbEntity, ("SQL to insert a new Entity: '%s'", query.c_str()));
		s = mysql_query(db, query.c_str());
		if (s != 0)
		{
			LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
			*errorString = std::string("SQL error: ") + mysql_error(db);
			return -1;
		}
	}

	return 0;
}
