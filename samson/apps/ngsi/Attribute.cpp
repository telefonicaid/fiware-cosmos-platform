/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
*
* FILE                     Attribute.cpp -  
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 15 2012
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
#include "database.h"                       // db, ...
#include "Entity.h"                         // Entity
#include "Attribute.h"                      // Own interface

using namespace std;



/* ****************************************************************************
*
* Global variables
*/
Attribute*         attributeList = NULL;
static Attribute*  attributeLast = NULL;
int                attributes    = 0;



/* ****************************************************************************
*
* attributeLoadFromDb - 
*/
static void attributeLoadFromDb(void)
{
	Attribute*   attributeP;
	std::string  query = "SELECT * from attribute";
	int          s;
    int          results;
    MYSQL_RES*   result  = NULL;
	MYSQL_ROW    row;

    s = mysql_query(db, query.c_str());
    if (s != 0)
    {
        LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
        return;
    }

    if ((result = mysql_store_result(db)) == NULL)
    {
        LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
        return;
    }

	results = mysql_num_rows(result);
	if (results <= 0)
	{
		mysql_free_result(result);
		return;
	}

	while ((row = mysql_fetch_row(result)))
	{
		int         dbId           = atoi(row[0]);
		std::string name           = row[1];
		std::string type           = row[2];
		std::string metaId         = row[4];
		std::string isDomainString = row[5];
		bool        isDomain       = false;

		if (isDomainString == "Yes")
			isDomain = true;

		Entity e; // Don't ask ... it SIGSEGVs without this ...

		if (metaId == "")
			attributeP = attributeCreate(&e, dbId, name, type, "NO_VALUE", "NO_META_ID", isDomain);
		else
		{
			attributeP = attributeCreate(&e, dbId, name, type, "NO_VALUE", metaId, isDomain);
		}

		attributeP->entityP = NULL;
		attributeAppend(attributeP);
	}
	mysql_free_result(result);



	//
	// Now, lookup the entity for each of these attributes (table 'entityAttribute')
	// Note that the entities should be in RAM first ...
	//
	attributeP = attributeList;
	while (attributeP)
	{
		char     dbId[32];
		int      entityDbId;
		Entity*  entityP;

		LM_T(LmtAttributeInit, ("Looking up entity for attribute '%s:%s", attributeP->name.c_str(), attributeP->type.c_str()));

		sprintf(dbId, "%d", attributeP->dbId);
		query = std::string("SELECT * from entityAttribute WHERE attributeId=") + dbId;
		s = mysql_query(db, query.c_str());
		if (s != 0)
		{
			LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
			continue;
		}

		if ((result = mysql_store_result(db)) == NULL)
		{
			LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
			LM_T(LmtEntityAttribute, ("Attribute dbId '%s' doesn't exist in the DB", dbId));
			continue;
		}

		results = mysql_num_rows(result);
		if (results == 0)
		{
			// Consume ...
			row = mysql_fetch_row(result);
			mysql_free_result(result);
			continue;
		}
		if (results != 1)
			LM_X(1, ("Attribute dbId '%s' is in %d places in DB table 'entityAttribute'!!!", dbId));

		row = mysql_fetch_row(result);
		entityDbId = atoi(row[0]);
		mysql_free_result(result);
		LM_T(LmtAttributeInit, ("found entity id for attribute: %d", entityDbId));
		entityP = entityLookupByDbId(entityDbId);

		if (entityP == NULL)
			LM_W(("Entity with db id '%d' not found ...", entityDbId));
		else
		{
			LM_T(LmtEntityAttribute, ("Found entity '%s:%s' for attribute '%s:%s", entityP->id.c_str(), entityP->type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));
			attributeP->entityP = entityP;
		}

		attributeP = attributeP->next;
	}
}



/* ****************************************************************************
*
* attributeInit - 
*/
void attributeInit(void)
{
	int itemsInDb = dbItemsInTable("attribute");

	if (itemsInDb < 100)
		attributeLoadFromDb();
}



/* ****************************************************************************
*
* attributeCreate - 
*/
Attribute* attributeCreate(Entity* entityP, int dbId, std::string name, std::string type, std::string value, std::string metaId, bool isDomain)
{
	Attribute* attributeP = new Attribute();

	attributeP->entityP   = entityP;
	attributeP->dbId      = dbId;
	attributeP->name      = name;
	attributeP->type      = type;
	attributeP->isDomain  = isDomain;
	attributeP->next      = NULL;

	if ((value != "NO_VALUE") && (value != ""))
		attributeP->value     = value;

	if ((metaId != "NO_META_ID") && (metaId != ""))
		attributeP->metaId    = metaId;

	if (entityP != NULL)
		LM_T(LmtAttributeCreate, ("Created attribute: '%s:%s:%s' for entity '%s:%s' - dbId: %d", attributeP->name.c_str(), attributeP->type.c_str(), attributeP->metaId.c_str(), entityP->type.c_str(), entityP->id.c_str(), attributeP->dbId));
	else
		LM_T(LmtAttributeCreate, ("Created attribute: '%s:%s:%s' for entity '%s:%s' - dbId: %d", attributeP->name.c_str(), attributeP->type.c_str(), attributeP->metaId.c_str(), entityP->type.c_str(), entityP->id.c_str(), attributeP->dbId));

	return attributeP;
}



/* ****************************************************************************
*
* attributeAppend - 
*/
void attributeAppend(Attribute* attribute)
{
	if (attributeList == NULL)
		attributeList = attribute;

	if (attributeLast == NULL)
		attributeLast = attribute;
	else
		attributeLast->next = attribute;

	attribute->next = NULL;
	attributeLast   = attribute;

	++attributes;
	LM_T(LmtAttribute, ("XXX2 - Appended attribute %d: '%s:%s:%s'", attributes, attribute->name.c_str(), attribute->type.c_str(), attribute->metaId.c_str()));
}



/* ****************************************************************************
*
* entityAttributeExistsInDb - 
*/
bool entityAttributeExistsInDb(char* entityDbId, char* attributeDbId)
{
    int          s;
    int          results;
    MYSQL_RES*   result  = NULL;
	std::string  query   = std::string("SELECT * from entityAttribute WHERE entityId='") + entityDbId + "' AND attributeId='" + attributeDbId + "'";

	LM_T(LmtEntityAttribute, ("Checking whether the entityAttribute-pair '%s - %s' is already in DB", entityDbId, attributeDbId));
	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
    s = mysql_query(db, query.c_str());
    if (s != 0)
    {
        LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
        return false;
    }

    if ((result = mysql_store_result(db)) == NULL)
    {
        LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		LM_T(LmtEntityAttribute, ("And NO - the entityAttribute-pair '%s - %s' doesn't exist in the DB", entityDbId, attributeDbId));
        return false;
    }

	results = mysql_num_rows(result);
	mysql_free_result(result);

	if (results == 1)
	{
		LM_T(LmtEntityAttribute, ("And YES - the entityAttribute-pair '%s - %s' does exist in the DB", entityDbId, attributeDbId));
		return true;
	}

	LM_T(LmtEntityAttribute, ("And NO - the entityAttribute-pair '%s - %s' doesn't exist (2) in the DB", entityDbId, attributeDbId));
	return false;
}



/* ****************************************************************************
*
* attributeExistsInDb - 
*/
static bool attributeExistsInDb(Entity* entityP, std::string name, std::string type, std::string metaId)
{
	int          s;
	MYSQL_RES*   result  = NULL;
	int          results;
	MYSQL_ROW    row;
	char         dbId[32];
	std::string  query;
	
	LM_T(LmtAttributeInDB, ("---------------------------------------------------------------------------------"));
	LM_T(LmtAttributeInDB, ("Looking up attribute '%s:%s:%s' for entity %d in DB", name.c_str(), type.c_str(), metaId.c_str(), entityP->dbId));

	sprintf(dbId, "%d", entityP->dbId);
	query = std::string("SELECT * from entityAttribute WHERE entityId='") + dbId + "'";
	LM_T(LmtAttributeInDB, ("Query to get all attributes: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		LM_T(LmtAttributeInDB, ("---------------------------------------------------------------------------------"));
		return false;
	}
	
	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		LM_T(LmtAttributeInDB, ("Cant find ANY attributes for entity %d", entityP->dbId));
		LM_T(LmtAttributeInDB, ("---------------------------------------------------------------------------------"));
		return false;
	}

	results = mysql_num_rows(result);

	//
	// Copying results to avoid 'mysql out of sync'
	//
	char** attributeDbIdV = (char**) malloc(sizeof(char*) * results);
	int    rowIx          = 0;

	if (attributeDbIdV == NULL)
		LM_X(1, ("error allocating a vector of %d char-pointers - cannot continue", results));
	
	while ((row = mysql_fetch_row(result)))
	{
		attributeDbIdV[rowIx] = strdup(row[1]);
		LM_T(LmtAttributeInDB, ("Attribute ID: '%s'", attributeDbIdV[rowIx]));
		++rowIx;
	}

	mysql_free_result(result);

	if (rowIx != results)
		LM_X(1, ("Expected %d results, got %d rows ... something is rotten ...", results, rowIx));

		 
	LM_T(LmtAttributeInDB, ("Checking %d attributes", results));
	for (rowIx = 0; rowIx < results; rowIx++)
	{
		char*       attributeDbId = attributeDbIdV[rowIx];
	   
		LM_T(LmtAttributeInDB, ("Testing attribute with attributeDbId == %s", attributeDbId));

		if (metaId != "")
			query = std::string("SELECT * FROM attribute WHERE dbId=") + attributeDbId + " AND name='" + name + "' AND type='" + type + "' AND metaID='" + metaId + "'";
		else
			query = std::string("SELECT * FROM attribute WHERE dbId=") + attributeDbId + " AND name='" + name + "' AND type='" + type + "'";
		LM_T(LmtAttributeInDB, ("Query is: '%s'", query.c_str()));

		s = mysql_query(db, query.c_str());
		LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
		if (s != 0)
		{
			LM_T(LmtAttributeInDB, ("mysql_query returned %d - I continue ... (%s)", s, mysql_error(db)));
			continue;
		}


		//
		// Now, let's see if we found what we're looking for ...
		//
		result   = mysql_store_result(db);
		int rows = mysql_num_rows(result);
		if (result)
			mysql_free_result(result);

		bool ret = true;
		if (rows == 0)
		{
			LM_T(LmtAttributeInDB, ("Nope, attribute dbId %s doesn't do it ...", attributeDbIdV[rowIx]));
			continue;
		}

		LM_T(LmtAttributeInDB, ("OK, found the attribute - the dbId is '%s'", attributeDbIdV[rowIx]));


		//
		// Free up the allocated attribute db ids ...
		//
		for (int ix = 0; ix < results; ix++)
			free(attributeDbIdV[ix]);
		free(attributeDbIdV);

		LM_T(LmtAttributeInDB, ("Attribute '%s:%s:%s' %sfound in DB", name.c_str(), type.c_str(), metaId.c_str(), (ret == true)? "" : "NOT "));
		LM_T(LmtAttributeInDB, ("---------------------------------------------------------------------------------"));
		return true;
	}

	for (int ix = 0; ix < results; ix++)
		free(attributeDbIdV[ix]);
	free(attributeDbIdV);

	LM_T(LmtAttributeInDB, ("Attribute '%s:%s:%s' for entity '%s:%s' not found in DB", name.c_str(), type.c_str(), metaId.c_str(), entityP->id.c_str(),entityP->type.c_str()));
	LM_T(LmtAttributeInDB, ("---------------------------------------------------------------------------------"));
	return false;
}



/* ****************************************************************************
*
* attributeDbLookup - 
*/
Attribute* attributeDbLookup(Entity* entityP, std::string name, std::string type, std::string metaId)
{
	int          s;
	MYSQL_RES*   result  = NULL;
	MYSQL_RES*   result2 = NULL;
	int          results;
	MYSQL_ROW    row;
	char         dbId[32];
	std::string  query;
	
	LM_T(LmtAttributeDbLookup, ("Looking up attribute '%s:%s:%s' for entity %d", name.c_str(), type.c_str(), metaId.c_str(), entityP->dbId));

	sprintf(dbId, "%d", entityP->dbId);
	query = std::string("SELECT * from entityAttribute WHERE entityId='") + dbId + "'";
	s = mysql_query(db, query.c_str());
	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return NULL;
	}
	
	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		LM_T(LmtAttribute, ("Cant find ANY attributes for entity %d", entityP->dbId));
		return NULL;
	}

	results = mysql_num_rows(result);

    //
    // Copying results to avoid 'mysql out of sync'
    //
	char** attributeDbIdV = (char**) malloc(sizeof(char*) * results);
	int    rowIx          = 0;

	if (attributeDbIdV == NULL)
		LM_X(1, ("error allocating a vector of %d char-pointers - cannot continue", results));

	while ((row = mysql_fetch_row(result)))
	{
		attributeDbIdV[rowIx] = strdup(row[1]);
		++rowIx;
	}

	mysql_free_result(result);

	if (rowIx != results)
		LM_X(1, ("Expected %d results, got %d rows ... something is rotten ...", results, rowIx));

	for (rowIx = 0; rowIx < results; rowIx++)
	{
		char*       attributeDbId = attributeDbIdV[rowIx];
		MYSQL_ROW   attribute     = NULL;
	   
		LM_T(LmtAttribute, ("Testing attribute with attributeDbId == %s", attributeDbId));

		if (metaId != "")
			query = std::string("SELECT * FROM attribute WHERE dbId=") + attributeDbId + " AND name='" + name + "' AND type='" + type + "' AND metaID='" + metaId + "'";
		else
			query = std::string("SELECT * FROM attribute WHERE dbId=") + attributeDbId + " AND name='" + name + "' AND type='" + type + "'";
		LM_T(LmtAttribute, ("Query is: '%s'", query.c_str()));

		LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
		s = mysql_query(db, query.c_str());
		if (s != 0)
		{
			LM_T(LmtAttribute, ("mysql_query returned %d - I continue ... (%s)", s, mysql_error(db)));
			continue;
		}

		if ((result2 = mysql_store_result(db)) == NULL)
		{
			LM_E(("Error storing result from an otherwise OK query ..."));
			return NULL;
		}

		int rows = mysql_num_rows(result2);
		LM_T(LmtAttribute, ("found %d rows for query", rows));
		
		if (rows != 0)
		{
			attribute = mysql_fetch_row(result2);
			if (attribute == NULL)
			{
				mysql_free_result(result2);

				LM_E(("Error fetching row from an otherwise OK query ..."));
				return NULL;
			}
		}

		//
		// Seems we found it in the database ...
		// Now, it is in RAM?
		// If so, just return it, otherwise, add it to RAM
		//
		if (rows != 0)
			LM_T(LmtAttribute, ("Seems we found the attribute in the database - if it is already in RAM - just return it"));
		else
			LM_T(LmtAttribute, ("The attribute is NOT in the database - if it is already in RAM - just return it"));

		Attribute* attr = attributeLookup(entityP, name, type, metaId, true);
		if (attr != NULL)
		{
			LM_T(LmtAttribute, ("Seems we found the attribute in DB and also in RAM - now just returning it"));

			//
			// I could double check that the query returned what it should, but frankly ...
			//
			mysql_free_result(result2);

			return attr;
		}

		if (rows == 0)
		{
			LM_T(LmtAttribute, ("Attribute '%s:%s' doesn't exist", name.c_str(), type.c_str()));
			return NULL;
		}

		Attribute* attributeP;
		bool       isDomain = false;

		LM_T(LmtAttribute, ("Not in RAM - let's put it there ..."));
		if (std::string((char*) attribute[5]) == "Yes")
			isDomain = true;

		attributeP = attributeCreate(entityP, atoi(attributeDbId), attribute[1], attribute[2], attribute[3], attribute[4], isDomain);
		attributeAppend(attributeP);

		mysql_free_result(result2);

		return attributeP;
	}

	return NULL;
}



/* ****************************************************************************
*
* attributeLookup - 
*/
Attribute* attributeLookup(Entity* entityP, std::string name, std::string type, std::string metaId, bool onlyRAM)
{
	Attribute*   aP;

	LM_T(LmtAttributeLookup, ("Looking up attribute '%s:%s:%s' for Entity '%s:%s'", type.c_str(), name.c_str(), metaId.c_str(), entityP->type.c_str(), entityP->id.c_str()));
	aP = attributeList;
	while (aP != NULL)
	{
		LM_T(LmtAttributeLookup, ("Comparing attribute '%s:%s:%s %p' to wanted '%s:%s:%s %p'",
								  aP->type.c_str(), aP->name.c_str(), aP->metaId.c_str(), aP->entityP,
								  type.c_str(), name.c_str(), metaId.c_str(), entityP));
		if ((aP->entityP == entityP) && (aP->name == name) && (aP->type == type) && (aP->metaId == metaId))
		{
			LM_T(LmtAttributeLookup, ("Found attribute '%s:%s' for Entity '%s:%s'", type.c_str(), name.c_str(), entityP->type.c_str(), entityP->id.c_str()));
			return aP;
		}

		aP = aP->next;
	}

	if (onlyRAM == true)
	{
		LM_T(LmtAttributeLookup, ("attribute '%s:%s' for Entity '%s:%s' not found in RAM", type.c_str(), name.c_str(), entityP->type.c_str(), entityP->id.c_str()));
		return NULL;
	}

	if (entityP->dbId == 0)
		LM_X(1, ("Entity '%s:%s' has no db id ...", entityP->id.c_str(), entityP->type.c_str()));

	//
	// If not found in RAM, look it up in DB - attributeDbLookup adds the attribute to RAM
	//
	LM_T(LmtAttribute, ("Not found in RAM - looking in DB"));
	return attributeDbLookup(entityP, name, type, metaId);
}



/* ****************************************************************************
*
* attributeLookup - 
*/
Attribute* attributeLookup(Entity* entityP, std::string name)
{
	Attribute*   aP;

	LM_T(LmtAttributeLookup, ("Looking up attribute '%s' for Entity '%s:%s'", name.c_str(), entityP->type.c_str(), entityP->id.c_str()));
	aP = attributeList;
	while (aP != NULL)
	{
		LM_T(LmtAttributeLookup, ("Comparing attribute '%s %p' to wanted '%s %p'", aP->name.c_str(), aP->entityP, name.c_str(), entityP));

		if ((aP->entityP == entityP) && (aP->name == name))
		{
			LM_T(LmtAttributeLookup, ("Found attribute '%s:%s' for Entity '%s:%s'", aP->type.c_str(), aP->name.c_str(), entityP->type.c_str(), entityP->id.c_str()));
			return aP;
		}

		aP = aP->next;
	}

	//
	// Not found in RAM - look in DB
	//
	
	return NULL;
}



/* ****************************************************************************
*
* attributeUpdate - 
*/
void attributeUpdate(Attribute* attribute, Attribute* updateFrom)
{
	attribute->metaId = updateFrom->metaId;
}



/* ****************************************************************************
*
* attributeToDb - 
*/
int attributeToDb(Entity* entityP, Attribute* attribute, bool update)
{
	int          s;
	std::string  isDomain   = (attribute->isDomain == true)? "Yes" : "No";
	bool         existsInDb = attributeExistsInDb(entityP, attribute->name, attribute->type, attribute->metaId);
	std::string  query;

	LM_T(LmtAttribute, ("Attribute '%s:%s' to db", attribute->name.c_str(), attribute->type.c_str()));

	if (existsInDb == true)
		LM_T(LmtAttributeInDB, ("Attribute '%s:%s' for entity %s:%s' already is in the DB", attribute->name.c_str(), attribute->type.c_str(), entityP->id.c_str(), entityP->type.c_str()));
	else
		LM_T(LmtAttributeInDB, ("Attribute '%s:%s' for entity %s:%s' not in the DB", attribute->name.c_str(), attribute->type.c_str(), entityP->id.c_str(), entityP->type.c_str()));

	if ((update == false) && (existsInDb == true))
		LM_RE(-1, ("Not an update, but the attribute '%s:%s:%s' already exists ...", attribute->name.c_str(), attribute->type.c_str(), attribute->metaId.c_str()));

	if (existsInDb == false)
	{
		query = "INSERT into attribute (name, type, value, metaID, isDomain) VALUES ('" + attribute->name + "', '" + attribute->type + "', '" + attribute->value + "', '" + attribute->metaId + "', '" + isDomain + "')";
		LM_T(LmtDbEntity, ("SQL to insert a new Attribute: '%s'", query.c_str()));
	}
	else
	{
		//
		// First, get and update the RAM copy
		//
		Attribute* aP = attributeLookup(entityP, attribute->name, attribute->type, attribute->metaId, true);

		if (aP == NULL)
			LM_X(1, ("Cannot find an attribute that is supposed to exist!!!"));

		aP->value    = attribute->value;
		aP->isDomain = attribute->isDomain;

		//
		// Then, create the query to UPDATE the attribute in the database
		//
		if (aP->value == "")
			query = std::string("UPDATE attribute SET isDomain='") + std::string((attribute->isDomain == true)? "Yes" : "No") + "'";
		else
			query = std::string("UPDATE attribute SET value='") + aP->value + "', isDomain='" + std::string((attribute->isDomain == true)? "Yes" : "No") + "'";

		LM_T(LmtAttributeCreate, ("Not an insert - old dbId == %d", aP->dbId));
	}

	LM_T(LmtAttributeCreate, ("SQL Query is '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	if (existsInDb == false)
	{
		attribute->dbId = mysql_insert_id(db);
		LM_T(LmtAttributeCreate, ("Just inserted attribute got dbId %d", attribute->dbId));
	}
	else
		LM_T(LmtAttributeCreate, ("No dbId assigned to attribute - was it an UPDATE?"));

	char entityDbId[32];
	char attributeDbId[32];

	sprintf(entityDbId,    "%d", entityP->dbId);
	sprintf(attributeDbId, "%d", attribute->dbId);

	bool entityAttributeExists = entityAttributeExistsInDb(entityDbId, attributeDbId);

	if (entityAttributeExists == false)
	{
		query = std::string("INSERT into entityAttribute (entityId, attributeId) VALUES (") + entityDbId + ", " + attributeDbId + ")";
		LM_T(LmtDbEntity, ("SQL to insert a new Entity-Attribute: '%s'", query.c_str()));
		s = mysql_query(db, query.c_str());
		LM_T(LmtSqlQuery, ("SQL Query is '%s'", query.c_str()));
		if (s != 0)
		{
			LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
			return -1;
		}
	}

	return 0;
}



/* ****************************************************************************
*
* attributesPresent - 
*/
void attributesPresent(bool force)
{
	Attribute*   aP;
	int          attributes = 0;
	char         oldValue = lmTraceIsSet(LmtAttributesPresent);

	if (force == true)
		lmTraceLevelSet(LmtAttributesPresent, true);

	LM_T(LmtAttributesPresent, (""));
	aP = attributeList;
	while (aP != NULL)
	{
		++attributes;
		aP = aP->next;
	}	

	LM_T(LmtAttributesPresent, ("%d attributes:", attributes));
    aP = attributeList;
    while (aP != NULL)
    {
        LM_T(LmtAttributesPresent, ("o %s, type %s, metaId '%s' belongs to entity '%s:%s'", aP->name.c_str(), aP->type.c_str(), aP->metaId.c_str(), aP->entityP->id.c_str(), aP->entityP->type.c_str()));
        aP = aP->next;
    }
	LM_T(LmtAttributesPresent, (""));

	if ((force == true) && (oldValue == false))
		lmTraceLevelSet(LmtAttributesPresent, false);
}
