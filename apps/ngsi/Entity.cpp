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

#include "globals.h"                            // TF
#include "traceLevels.h"                        // Trace levels for log msg library
#include "jsonParse.h"                          // JSON parsing function
#include "httpData.h"                           // httpData
#include "rest.h"                               // restReply
#include "ContextRegistrationAttributeList.h"   // ContextRegistrationAttributeList
#include "database.h"                           // db
#include "Attribute.h"                          // Attribute
#include "Metadata.h"                           // Metadata
#include "Entity.h"                             // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
static Entity*   entityList   = NULL;
static Entity*   entityLast   = NULL;
static int       entities     = 0;



/* ****************************************************************************
*
* entityInit - 
*/
void entityInit(void)
{
	entityList  = NULL;
	entities = 0;
}



/* ****************************************************************************
*
* entityCreate - 
*/
static Entity* entityCreate
(
	std::string  id,
	std::string  type,
	bool         isPattern,
	time_t       startTime,
	time_t       endTime,
	int          duration,
	std::string  providingApplication,
	std::string  registrationId,
	int          dbId
)
{
	Entity* entity = new Entity();

	if (entity == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	LM_T(LmtDuration, ("Creating an Entity - duration: %d", duration));

	entity->id                    = id;
	entity->type                  = type;
	entity->isPattern             = isPattern;
	entity->providingApplication  = providingApplication;
	entity->registrationId        = registrationId;
	entity->dbId                  = dbId;

	if (startTime == 0)
	{
		entity->startTime             = time(NULL);
		entity->endTime               = entity->startTime + duration;
	}
	else
	{
		entity->startTime             = startTime;
		entity->endTime               = endTime;
	}

	entity->next      = NULL;

	LM_T(LmtEntity, ("Created an entity struct for '%s' of %d bytes. id='%s', type='%s', providingApplication='%s', registrationId='%s'",
					 entity->id.c_str(), sizeof(Entity), entity->id.c_str(), entity->type.c_str(), entity->providingApplication.c_str(), entity->registrationId.c_str()));

	return entity;
}



/* ****************************************************************************
*
* entityAppend - 
*/
static void entityAppend(Entity* entity)
{
	LM_T(LmtEntity, ("Appending entity '%s'", entity->id.c_str()));

	if (entityLast == NULL)
		entityList = entity;
	else
		entityLast->next = entity;

	entityLast = entity;

	entitiesPresent();
}



/* ****************************************************************************
*
* entityLookupInDb - 
*/
static Entity* entityLookupInDb(std::string id, std::string type)
{
	MYSQL_RES*   result;
	MYSQL_ROW    row;
	int          rows;
	int          s;

	std::string query = "SELECT * from entity WHERE id = '" + id + "' AND type = '" + type + "'";

	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", mysql_error(db)));
		return NULL;
	}

	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return NULL;
	}

	rows = mysql_num_rows(result);
	if (rows == 0)
	{
		mysql_free_result(result);
		return NULL;
	}
	else if (rows != 1)
		LM_X(1, ("Found more than ONE instance of entity '%s:%s'", type.c_str(), id.c_str()));

	row = mysql_fetch_row(result);
	mysql_free_result(result);


	int          dbId                  = atoi(row[0]);
	std::string  id2                   = row[1];
	std::string  type2                 = row[2];
	bool         isPattern             = (std::string(row[3]) == "No")? false : true;
	std::string  providingApplication  = row[4];
	time_t       startTime             = strtoul(row[5], NULL, 10);
	time_t       endTime               = strtoul(row[6], NULL, 10);
	std::string  registrationId        = row[7];
	
	Entity* entityP = entityCreate(id, type, isPattern, startTime, endTime, 0, providingApplication, registrationId, dbId);

	return entityP;
}



/* ****************************************************************************
*
* entityLookupInDb - 
*/
static Entity* entityLookupInDb(int dbId)
{
	MYSQL_RES*   result;
	MYSQL_ROW    row;
	int          rows;
	int          s;
	char         dbIdV[32];

	sprintf(dbIdV, "%d", dbId);
	std::string query = std::string("SELECT * from entity WHERE dbId = ") + dbIdV;

	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", mysql_error(db)));
		return NULL;
	}

	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return NULL;
	}

	rows = mysql_num_rows(result);
	if (rows == 0)
	{
		mysql_free_result(result);
		return NULL;
	}
	else if (rows != 1)
		LM_X(1, ("Found more than ONE instance of entity with dbId '%d'", dbId));

	row = mysql_fetch_row(result);
	mysql_free_result(result);


	std::string  id                    = row[1];
	std::string  type                  = row[2];
	bool         isPattern             = (std::string(row[3]) == "No")? false : true;
	std::string  providingApplication  = row[4];
	time_t       startTime             = strtoul(row[5], NULL, 10);
	time_t       endTime               = strtoul(row[6], NULL, 10);
	std::string  registrationId        = row[7];
	
	Entity* entityP = entityCreate(id, type, isPattern, startTime, endTime, 0, providingApplication, registrationId, dbId);

	return entityP;
}



/* ****************************************************************************
*
* entityLookup -
*/
Entity* entityLookup(std::string id, std::string type)
{
	Entity* entityP = entityList;

	//
	// First lookup in RAM
	//
	while (entityP != NULL)
	{
		if ((entityP->id == id) && (entityP->type == type))
			return entityP;

		entityP = entityP->next;
	}

	//
	// Now try to find it in DB - if found, add to RAM
	//
	entityP = entityLookupInDb(id, type);
	if (entityP == NULL)
		return NULL;

	entityAppend(entityP);
	return entityP;
}



/* ****************************************************************************
*
* entityLookup -
*/
Entity* entityLookupByDbId(int dbId)
{
	Entity* entityP = entityList;

	//
	// lookup in RAM only ...
	//
	while (entityP != NULL)
	{
		if (entityP->dbId == dbId)
			return entityP;

		entityP = entityP->next;
	}

	entityP = entityLookupInDb(dbId);
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
    unsigned int  aIx;
    unsigned int  mIx;

	LM_T(LmtEntity, ("Adding entity '%s:%s'", rcrEntity->type.c_str(), rcrEntity->id.c_str()));

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
	// If already found, should I return it ... ?
	// Or, return NULL, to indicate it already existed ... ?
	//
	LM_T(LmtEntity, ("Looking up entity '%s'", rcrEntity->id.c_str()));
	entityP = entityLookup(rcrEntity->id, rcrEntity->type);
	if (entityP != NULL)
		LM_RE(NULL, ("Entity '%s' of type '%s' already exists", rcrEntity->id.c_str(), rcrEntity->type.c_str()));



	//
	// Adding entity to global RAM collection
	//
	entityP = entityCreate(rcrEntity->id, rcrEntity->type, rcrEntity->isPattern, 0, 0, duration, providingApplication, registrationId, 0);
	entityAppend(entityP);



	//
	// Updating rcrEntity, for this request
	//
	rcrEntity->providingApplication = entityP->providingApplication;
	rcrEntity->startTime            = entityP->startTime;
	rcrEntity->endTime              = entityP->endTime;
	rcrEntity->registrationId       = entityP->registrationId;



	//
	// Adding the attributes (and their metadata)
	//
	for (aIx = 0; aIx < attributeList->attributeV.size(); aIx++)
	{
		Attribute*  aP         = attributeList->attributeV[aIx];
		Attribute*  attribute;
		std::string metaId     = "";
		
		//
		// First, get the metaID, if any ...
		//
		LM_T(LmtAttributeMetaId, ("Looking for metadata 'ID' in %d metadatas for attribute '%s:%s'", aP->metadataV.size(), aP->name.c_str(), aP->type.c_str()));
		for (mIx = 0; mIx < aP->metadataV.size(); mIx++)
		{
			Metadata* mP        = aP->metadataV[mIx];

			if (mP->name == "ID")
			{
				metaId = mP->value;
				LM_T(LmtAttributeMetaId, ("Found metaId '%s' for attribute '%s:%s'", metaId.c_str(), aP->name.c_str(), aP->type.c_str()));
			}
		}

		if (metaId == "")
			LM_T(LmtAttributeMetaId, ("No metaId found for attribute '%s:%s'", aP->name.c_str(), aP->type.c_str()));

		attribute = attributeLookup(entityP, aP->name, aP->type, metaId, true);
		if (attribute == NULL)
		{
			LM_T(LmtToRAM, ("Could not find attribute '%s:%s:%s' in RAM - creating it", aP->name.c_str(), aP->type.c_str(), metaId.c_str()));
			attribute = attributeCreate(entityP, 0, aP->name, aP->type, aP->value, metaId, aP->isDomain);
			attributeAppend(attribute);
		}
		else
			LM_T(LmtToRAM, ("Found attribute '%s:%s:%s' - in RAM", aP->name.c_str(), aP->type.c_str(), metaId.c_str()));

		for (mIx = 0; mIx < aP->metadataV.size(); mIx++)
		{
			Metadata* mP        = aP->metadataV[mIx];
			Metadata* metadata;

			if ((mP->name == "") || (mP->name == "emptycontent") || (mP->type == "") || (mP->type == "emptycontent"))
			{
				LM_W(("Skipping Metadata with 'empty content'"));
				continue;
			}

			metadata = metadataLookup(attribute, mP->name, mP->type);
			if (metadata == NULL)
			{
				LM_T(LmtToRAM, ("Didn't find metadata '%s:%s' for attribute '%s:%s:%s' in RAM - creating it", mP->name.c_str(), mP->type.c_str(), aP->name.c_str(), aP->type.c_str(), metaId.c_str()));
				metadata = metadataCreate(attribute, mP->name, mP->type, mP->value);
				if (mP->name == "ID")
					attribute->metaId = mP->value;
				metadataAppend(metadata);
			}
		}
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
    unsigned int  aIx;
    unsigned int  mIx;

	LM_T(LmtEntity, ("Updating entity '%s:%s'", rcrEntity->type.c_str(), rcrEntity->id.c_str()));

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
	// Lookup the entity
	//
	LM_T(LmtEntity, ("Looking up entity '%s'", rcrEntity->id.c_str()));
	entityP = entityLookup(rcrEntity->id, rcrEntity->type);
	if (entityP == NULL)
		LM_RE(NULL, ("Entity '%s' of type '%s' doesn't exist", rcrEntity->id.c_str(), rcrEntity->type.c_str()));

	LM_T(LmtDbId, ("Found entity '%s:%s' with dbId %d", entityP->id.c_str(), entityP->type.c_str(), entityP->dbId));

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
		// Seems more logical to maintain the start time
		//
		// But, if startTime is in the future?
		//
		
		LM_T(LmtDuration, ("duration is %lu", duration));
		entityP->endTime    = time(NULL) + duration;
		rcrEntity->endTime  = entityP->endTime;
		LM_T(LmtDuration, ("Correct endTime is %lu", entityP->endTime));
	}



	//
	// Update or add attributes
	//
	for (aIx = 0; aIx < attributeList->attributeV.size(); aIx++)
	{
		Attribute* aP          = attributeList->attributeV[aIx];
		Attribute* attribute;
		std::string metaId     = "";

        //
        // First, get the metaID, if any ...
        //
		LM_T(LmtAttributeMetaId, ("Looking for metadata 'ID' in %d metadatas for attribute '%s:%s'", aP->metadataV.size(), aP->name.c_str(), aP->type.c_str()));
        for (mIx = 0; mIx < aP->metadataV.size(); mIx++)
        {
            Metadata* mP        = aP->metadataV[mIx];

			if (mP->name == "ID")
			{
				metaId = mP->value;
				LM_T(LmtAttributeMetaId, ("Found metaId '%s' for attribute '%s:%s'", metaId.c_str(), aP->name.c_str(), aP->type.c_str()));
			}
        }
		
		if (metaId == "")
			LM_T(LmtAttributeMetaId, ("No metaId found for attribute '%s:%s'", aP->name.c_str(), aP->type.c_str()));

		attribute = attributeLookup(entityP, aP->name, aP->type, metaId, true);
		if (attribute != NULL)
		{
			LM_T(LmtAttribute, ("Attribute %s:%s already exists - does it need updating ... ?", aP->name.c_str(), aP->type.c_str()));
		}
		else
		{
			LM_T(LmtAttributeCreate, ("Creating attribute %s:%s:%s", aP->name.c_str(), aP->type.c_str(), metaId.c_str()));
			attribute = attributeCreate(entityP, 0, aP->name, aP->type, aP->value, metaId, aP->isDomain);
			LM_T(LmtAttributeCreate, ("Created attribute %s:%s:%s", attribute->name.c_str(), attribute->type.c_str(), aP->metaId.c_str()));
			LM_T(LmtAttributeCreate, ("Appending attribute %s:%s:%s", attribute->name.c_str(), attribute->type.c_str(), aP->metaId.c_str()));
			attributeAppend(attribute);
			LM_T(LmtAttributeCreate, ("Appended attribute %s:%s:%s", attribute->name.c_str(), attribute->type.c_str(), aP->metaId.c_str()));
		}

		LM_T(LmtAttribute, ("Adding %d metadata(s) for attribute %s:%s", attribute->metadataV.size(), attribute->name.c_str(), attribute->type.c_str()));

		for (mIx = 0; mIx < attribute->metadataV.size(); mIx++)
		{
			Metadata* mP        = attribute->metadataV[mIx];
			Metadata* metadata;

			if ((mP->name == "") || (mP->name == "emptycontent") || (mP->type == "") || (mP->type == "emptycontent"))
			{
				if (attribute->entityP == NULL)
					LM_X(1, ("NULL entity pointer for attribute"));
				LM_W(("Empty content or type for metadata for attribute '%s:%s:%s' belonging to entity '%s:%s'- skipping it ...",
					  attribute->name.c_str(), attribute->type.c_str(), attribute->metaId.c_str(), attribute->entityP->id.c_str(), attribute->entityP->type.c_str()));
				continue;
			}

			metadata = metadataLookup(attribute, mP->name, mP->type);
			if (metadata == NULL)
			{
				metadata = metadataCreate(attribute, mP->name, mP->type, mP->value);
				metadataAppend(metadata);
				if ((mP->name == "ID") && (attribute->metaId == ""))
					attribute->metaId = mP->value;
			}
			else
				metadataUpdate(metadata, mP);
		}
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
	MYSQL_ROW    row;
	MYSQL_ROW    firstRow;
	int          rows;

	// Check that the entity exists in DB 
	std::string query = "SELECT * from entity WHERE id = '" + entityP->id + "' AND type = '" + entityP->type + "'";
		
	LM_T(LmtDbEntity, ("SQL to verify that entity exists: %s", query.c_str()));
	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", mysql_error(db)));
		*errorString = std::string("SQL error: ") + mysql_error(db);
		return false;
	}

	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		*errorString = std::string("mysql_store_result: ") + mysql_error(db);
		return false;
	}

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
int entityToDb(Entity* entityP, bool update, long duration, std::string* errorString)
{
	bool         isPattern = false;
	std::string  providingApplication;
	int          s;

	if (update == true)
	    LM_T(LmtEntityToDb, ("Entity { '%s', '%s', '%s' } DB update", entityP->id.c_str(), entityP->type.c_str(), TF(entityP->isPattern)));
	else
	    LM_T(LmtEntityToDb, ("Entity { '%s', '%s', '%s' } to DB", entityP->id.c_str(), entityP->type.c_str(), TF(entityP->isPattern)));

	LM_T(LmtDuration, ("entityToDb: endTime: %lu", entityP->endTime));
	if (update)
	{
		if (entityExistsInDb(entityP, errorString, &isPattern, &providingApplication) == false)
		{
			LM_W(("update for a non-existing entity (%s:%s)", entityP->id.c_str(), entityP->type.c_str()));
			return -1;
		}


		//
		// Now, entity exists in DB - let's update it, if necessary ... ?
		//
		if ((entityP->isPattern != isPattern) || (entityP->providingApplication != providingApplication) || (duration != 0))
		{
			char endTime[64];

			LM_W(("diff in isPattern OR providingApplication - what do I do (send the change to DB or flag an error?)"));

			std::string isPat = (entityP->isPattern == true)? "Yes" : "No";
			sprintf(endTime, "%lu", entityP->endTime);

			std::string query = "UPDATE entity SET isPattern='" + isPat + "', providingApplication='" + entityP->providingApplication + "', endTime='" + endTime + "'" +
				                " WHERE id='" + entityP->id + "' AND type='" + entityP->type + "'";

			LM_T(LmtDbEntity, ("SQL to UPDATE an Entity: '%s'", query.c_str()));
			LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
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
		LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
		s = mysql_query(db, query.c_str());
		if (s != 0)
		{
			LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
			*errorString = std::string("SQL error: ") + mysql_error(db);
			return -1;
		}

		entityP->dbId = mysql_insert_id(db);
		LM_T(LmtDbId, ("Inserted entity '%s:%s' has dbId: %d", entityP->id.c_str(), entityP->type.c_str(), entityP->dbId));
	}

	return 0;
}



/* ****************************************************************************
*
* entityPresent - 
*/
void entityPresent(Entity* entityP)
{
	int        attributes = 0;
	Attribute* attribute;

	LM_T(LmtEntityPresent, ("--------------- Entity '%s', type '%s' ---------------------------------", entityP->id.c_str(), entityP->type.c_str()));

	attribute = attributeList;
	while (attribute)
	{
		if (attribute->entityP == entityP)
			++attributes;
		attribute = attribute->next;
	}

	LM_T(LmtEntityPresent, ("  %d attributes:", attributes));

    attribute = attributeList;
    while (attribute)
	{
		int        metadatas = 0;
		Metadata*  metadata;

		if (attribute->entityP == entityP)
			LM_T(LmtEntityPresent, ("    %s:%s", attribute->name.c_str(), attribute->type.c_str()));

		metadata = metadataList;
		while (metadata)
		{
			if (metadata->attributeP == attribute)
				++metadatas;
			metadata = metadata->next;
		}

		LM_T(LmtEntityPresent, ("      %d metadatas", metadatas));

        metadata = metadataList;
        while (metadata)
        {
            if (metadata->attributeP == attribute)
                LM_T(LmtEntityPresent, ("        %s:%s:%s", metadata->name.c_str(), metadata->type.c_str(), metadata->value.c_str()));
			metadata = metadata->next;
        }

		attribute = attribute->next;
	}

	LM_T(LmtEntityPresent, ("---------------------------------------------------------------------------"));
}



/* ****************************************************************************
*
* entitiesPresent - 
*/
void entitiesPresent(void)
{
	Entity* entityP;
	int     entities = 0;

	entityP = entityList;
    while (entityP != NULL)
	{
		++entities;
		entityP = entityP->next;
	}

	LM_T(LmtEntitiesPresent, (""));
	LM_T(LmtEntitiesPresent, ("%d entities:", entities));
	entityP = entityList;
	while (entityP != NULL)
	{
		LM_T(LmtEntitiesPresent, ("o Entity   '%d'",  entityP->dbId));
		LM_T(LmtEntitiesPresent, ("    ID:    '%s'",  entityP->id.c_str()));
		LM_T(LmtEntitiesPresent, ("    dbId:  '%d'",  entityP->dbId));
		LM_T(LmtEntitiesPresent, ("    Type:  '%s'",  entityP->type.c_str())); 
		LM_T(LmtEntitiesPresent, ("    App:   '%s'",  entityP->providingApplication.c_str())); 
		LM_T(LmtEntitiesPresent, ("    Reg:   '%s'",  entityP->registrationId.c_str())); 
		LM_T(LmtEntitiesPresent, ("    Start: '%u'",  entityP->startTime));
		LM_T(LmtEntitiesPresent, ("    End:   '%u'",  entityP->endTime));
        LM_T(LmtEntitiesPresent, (""));

		entityP = entityP->next;
	}

	LM_T(LmtEntitiesPresent, (""));
}
