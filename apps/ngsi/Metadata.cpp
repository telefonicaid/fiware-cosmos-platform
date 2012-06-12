/* ****************************************************************************
*
* FILE                     Metadata.cpp -  
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            June 06 2012
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
#include "Entity.h"                         // Entity
#include "Attribute.h"                      // Attribute
#include "database.h"                       // db, ...
#include "Metadata.h"                       // Own interface

using namespace std;



/* ****************************************************************************
*
* Globals
*/
Metadata*        metadataList = NULL;
static Metadata* metadataLast = NULL;



/* ****************************************************************************
*
* metadataCreate - 
*/
Metadata* metadataCreate(struct Attribute* attribute, std::string name, std::string type, std::string value)
{
	Metadata* metadataP = new Metadata();

	metadataP->attributeP = attribute;
	metadataP->name       = name;
	metadataP->type       = type;
	metadataP->value      = value;
	metadataP->next       = NULL;

	LM_T(LmtMetadata, ("Created a metadata in RAM: '%s:%s:%s'", metadataP->name.c_str(), metadataP->type.c_str(), metadataP->value.c_str()));

	return metadataP;
}



/* ****************************************************************************
*
* metadataAppend - 
*/
void metadataAppend(Metadata* metadata)
{
	if (metadataList == NULL)
		metadataList = metadata;

	if (metadataLast == NULL)
		metadataLast = metadata;
	else
		metadataLast->next = metadata;

	metadata->next = NULL;
	metadataLast   = metadata;

	LM_T(LmtMetadata, ("Added metadata '%s:%s:%s' to linked RAM list", metadata->name.c_str(), metadata->type.c_str(), metadata->value.c_str()));
}



/* ****************************************************************************
*
* metadataUpdate - 
*
* Only the value can be updated, right ... ?
*/
int metadataUpdate(Metadata* metadata, Metadata* updateFrom)
{
	int  s;

	metadata->value = updateFrom->value;
	LM_T(LmtMetadataToDb, ("Updated metadata '%s:%s:%s' in RAM. New value: '%s'", metadata->name.c_str(), metadata->type.c_str(), metadata->value.c_str()));

	std::string query = "UPDATE metadata SET value='" + metadata->value + "' WHERE name='" + metadata->name + "' AND type='" + metadata->type + "'";
	LM_T(LmtMetadataToDb, ("Updating value for a metadata: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
		LM_RE(-1, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));

	LM_T(LmtMetadataToDb, ("Updated metadata '%s:%s:%s' in DB. New value: '%s'", metadata->name.c_str(), metadata->type.c_str(), metadata->value.c_str()));
	return 0;
}



/* ****************************************************************************
*
* metadataLookup - 
*/
Metadata* metadataLookup(struct Attribute* aP, std::string name, std::string type)
{
	Metadata* mP = metadataList;

	while (mP != NULL)
	{
		if ((mP->name == name) && (mP->type == type) && (mP->attributeP == aP))
			return mP;

		mP = mP->next;
	}

	//
	// Look it up in database
	//

	return NULL;
}



/* ****************************************************************************
*
* metadataExistsInDb - 
*/
static bool metadataExistsInDb(Attribute* attributeP, std::string name, std::string type, int* metadataDbIdP)
{
	std::string  query;
	char         attributeDbId[32];
	int          s;
    MYSQL_RES*   result  = NULL;
    int          results;
    MYSQL_ROW    row;

	LM_T(LmtMetadataToDb, ("Is the metadata '%s:%s' for attribute '%s:%s' already in the DB?", name.c_str(), type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));

	sprintf(attributeDbId, "%d", attributeP->dbId);

	query = std::string("SELECT * from attributeMetadata WHERE attributeId='") + attributeDbId + "'";
	s     = mysql_query(db, query.c_str());
    LM_T(LmtMetadataToDb, ("SQL Query to search attributeMetadata: '%s'", query.c_str()));
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtMetadataToDb, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	results = mysql_num_rows(result);
	if (results == 0)
	{
		LM_T(LmtMetadataToDb, ("The metadata '%s:%s' for attribute '%s:%s' is NOT in the DB", name.c_str(), type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));
		mysql_free_result(result);
		return false;
	}

	//
	// Copying results to avoid 'mysql out of sync'
	//
	char** metadataDbIdV = (char**) malloc(sizeof(char*) * results);
	int    rowIx          = 0;

	if (metadataDbIdV == NULL)
		LM_X(1, ("error allocating a vector of %d char-pointers - cannot continue", results));

	while ((row = mysql_fetch_row(result)))
	{
		metadataDbIdV[rowIx] = strdup(row[1]);
		++rowIx;
	}

	mysql_free_result(result);

	if (rowIx != results)
		LM_X(1, ("Expected %d results, got %d rows ... something is rotten ...", results, rowIx));

	for (rowIx = 0; rowIx < results; rowIx++)
	{
		char*  metadataDbId = metadataDbIdV[rowIx];
		int    rows;

		LM_T(LmtMetadataToDb, ("Testing metadata with metadataDbId == %s", metadataDbId));

		query = std::string("SELECT * FROM metadata WHERE dbId=") + metadataDbId + " AND name='" + name + "' AND type='" + type + "'";
		LM_T(LmtMetadataToDb, ("Query to search metadata: '%s'", query.c_str()));
		s = mysql_query(db, query.c_str());
		if (s != 0)
		{
			LM_T(LmtMetadata, ("mysql_query returned %d - I continue ... (%s)", s, mysql_error(db)));
			continue;
		}

		//
		// Let's see if I found it, and I have to consume it ...
		//
		result = mysql_store_result(db);
		rows   = mysql_num_rows(result);
		if (result)
			mysql_free_result(result);

		if (rows != 0)
		{
			LM_T(LmtMetadataToDb, ("The metadata '%s:%s' for attribute '%s:%s' found in the DB", name.c_str(), type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));

			//
			// Saving metadataDbId for caller
			//
			if (metadataDbIdP != NULL)
				*metadataDbIdP = atoi(metadataDbId);

			//
			// Freeing up allcated stuff
			//
			for (int ix = 0; ix < results; ix++)
				free(metadataDbIdV[ix]);
			free(metadataDbIdV);

			return true;
		}
	}

	//
	// Freeing up allcated stuff
	//
	for (int ix = 0; ix < results; ix++)
		free(metadataDbIdV[ix]);
	free(metadataDbIdV);

	LM_T(LmtMetadataToDb, ("The metadata '%s:%s' for attribute '%s:%s' is NOT in the DB", name.c_str(), type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));
	return false;
}



/* ****************************************************************************
*
* metadataExistsInDb - 
*/
static bool metadataExistsInDb(unsigned int registrationId, std::string name, std::string type, int* metadataDbIdP)
{
	std::string  query;
	char         registrationDbId[32];
	int          s;
    MYSQL_RES*   result  = NULL;
    int          results;
    MYSQL_ROW    row;

	LM_T(LmtRegMetadataToDb, ("Is the metadata '%s:%s' for registration '%d' already in the DB?", name.c_str(), type.c_str(), registrationId));

	sprintf(registrationDbId, "%d", registrationId);

	query = std::string("SELECT * from registrationMetadata WHERE registrationId='") + registrationDbId + "'";
	s     = mysql_query(db, query.c_str());
    LM_T(LmtRegMetadataToDb, ("SQL Query to add to table registrationMetadata: '%s'", query.c_str()));
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	if ((result = mysql_store_result(db)) == NULL)
	{
		LM_T(LmtRegMetadataToDb, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	results = mysql_num_rows(result);
	if (results == 0)
	{
		LM_T(LmtRegMetadataToDb, ("The metadata '%s:%s' for registration dbId '%d' is NOT in the DB", name.c_str(), type.c_str(), registrationId));
		mysql_free_result(result);
		return false;
	}

	//
	// Copying results to avoid 'mysql out of sync'
	//
	char** metadataDbIdV = (char**) malloc(sizeof(char*) * results);
	int    rowIx          = 0;

	if (metadataDbIdV == NULL)
		LM_X(1, ("error allocating a vector of %d char-pointers - cannot continue", results));

	while ((row = mysql_fetch_row(result)))
	{
		metadataDbIdV[rowIx] = strdup(row[1]);
		++rowIx;
	}

	mysql_free_result(result);

	if (rowIx != results)
		LM_X(1, ("Expected %d results, got %d rows ... something is rotten ...", results, rowIx));

	for (rowIx = 0; rowIx < results; rowIx++)
	{
		char*  metadataDbId = metadataDbIdV[rowIx];
		int    rows;

		LM_T(LmtRegMetadataToDb, ("Testing metadata with metadataDbId == %s", metadataDbId));

		query = std::string("SELECT * FROM metadata WHERE dbId=") + metadataDbId + " AND name='" + name + "' AND type='" + type + "'";
		s     = mysql_query(db, query.c_str());
		LM_T(LmtRegMetadataToDb, ("SQL Query to search registration metadata: '%s'", query.c_str()));
		if (s != 0)
		{
			LM_T(LmtRegMetadataToDb, ("mysql_query returned %d - I continue ... (%s)", s, mysql_error(db)));
			continue;
		}

		//
		// Let's see if I found it, and I have to consume it ...
		//
		result = mysql_store_result(db);
		rows   = mysql_num_rows(result);
		if (result)
			mysql_free_result(result);

		if (rows != 0)
		{
			LM_T(LmtRegMetadataToDb, ("The metadata '%s:%s' for registration dbId '%d' found in the DB", name.c_str(), type.c_str(), registrationId));

			//
			// Saving metadataDbId for caller
			//
			if (metadataDbIdP != NULL)
				*metadataDbIdP = atoi(metadataDbId);

			//
			// Freeing up allcated stuff
			//
			for (int ix = 0; ix < results; ix++)
				free(metadataDbIdV[ix]);
			free(metadataDbIdV);

			return true;
		}
	}

	//
	// Freeing up allcated stuff
	//
	for (int ix = 0; ix < results; ix++)
		free(metadataDbIdV[ix]);
	free(metadataDbIdV);

	LM_T(LmtRegMetadataToDb, ("The metadata '%s:%s' for registration dbId '%d' is NOT in the DB", name.c_str(), type.c_str(), registrationId));
	return false;
}



/* ****************************************************************************
*
* metadataToDb - 
*/
int metadataToDb(Attribute* attributeP, Metadata* metadata)
{
	int          s;
	int          metadataDbIdValue;
	char         metadataDbIdString[32];
	bool         existsInDb = metadataExistsInDb(attributeP, metadata->name, metadata->type, &metadataDbIdValue);
	std::string  query;

	if (existsInDb == true)
	{
		sprintf(metadataDbIdString, "%d", metadataDbIdValue);
		query = "UPDATE metadata SET value='" + metadata->value + "' WHERE dbId=" + metadataDbIdString; // only 'value' can change ...
		LM_T(LmtMetadataToDb, ("Updating metadata '%s:%s' for attribute '%s:%s' in DB", metadata->name.c_str(), metadata->type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));
	}
	else
	{
		query = "INSERT into metadata (name, type, value) VALUES ('" + metadata->name + "', '" + metadata->type + "', '" + metadata->value + "')";
		LM_T(LmtMetadataToDb, ("Adding metadata '%s:%s' for attribute '%s:%s' in DB", metadata->name.c_str(), metadata->type.c_str(), attributeP->name.c_str(), attributeP->type.c_str()));
	}

	LM_T(LmtMetadataToDb, ("SQL to insert a new Metadata: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	if (existsInDb == true)
		return 0;

	metadata->dbId = mysql_insert_id(db);

	char attributeDbId[32];
	char metadataDbId[32];

	sprintf(attributeDbId, "%d", attributeP->dbId);
	sprintf(metadataDbId, "%d", metadata->dbId);

	query = std::string("INSERT into attributeMetadata (attributeId, metadataId) VALUES (") + attributeDbId + ", " + metadataDbId + ")";
	LM_T(LmtMetadataToDb, ("SQL to insert a new Attribute-Metadata: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	return 0;
}



/* ****************************************************************************
*
* metadataToDb - 
*/
int metadataToDb(unsigned int registrationDbId, Metadata* metadata)
{
	int          s;
	int          metadataDbIdValue;
	char         metadataDbIdString[32];
	bool         existsInDb = metadataExistsInDb(registrationDbId, metadata->name, metadata->type, &metadataDbIdValue);
	std::string  query;

	if (existsInDb == true)
	{
		sprintf(metadataDbIdString, "%d", metadataDbIdValue);
		query = "UPDATE metadata SET value='" + metadata->value + "' WHERE dbId=" + metadataDbIdString; // only 'value' can change ...
		LM_T(LmtRegMetadataToDb, ("Updating metadata '%s:%s' for registration '%d' in DB", metadata->name.c_str(), metadata->type.c_str(), registrationDbId));
	}
	else
	{
		query = "INSERT into metadata (name, type, value) VALUES ('" + metadata->name + "', '" + metadata->type + "', '" + metadata->value + "')";
		LM_T(LmtRegMetadataToDb, ("Adding registration metadata '%s:%s' for registration '%d' in DB: '%s'", metadata->name.c_str(), metadata->type.c_str(), registrationDbId, query.c_str()));
	}

	LM_T(LmtRegMetadataToDb, ("SQL to insert a new Registration Metadata: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	if (existsInDb == true)
		return 0;

	metadata->dbId = mysql_insert_id(db);

	char registrationDbIdString[32];
	char metadataDbId[32];

	sprintf(registrationDbIdString, "%d", registrationDbId);
	sprintf(metadataDbId, "%d", metadata->dbId);

	query = std::string("INSERT into registrationMetadata (registrationId, metadataId) VALUES (") + registrationDbIdString + ", " + metadataDbId + ")";
	LM_T(LmtRegMetadataToDb, ("SQL to insert a new Registration-Metadata: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	return 0;
}



/* ****************************************************************************
*
* metadatasPresent - 
*/
void metadatasPresent(void)
{
	Metadata*  mP;
	int        metadatas = 0;

	mP = metadataList;
	while (mP != NULL)
	{
		++metadatas;
		mP = mP->next;
	}
	
	LM_T(LmtMetadatasPresent, (""));
	LM_T(LmtMetadatasPresent, ("%d metadatas", metadatas));
    mP = metadataList;
    while (mP != NULL)
    {
		LM_T(LmtMetadatasPresent, ("o %s, type '%s', value '%s', belongs to attribute '%s:%s' THAT belongs to entity '%s:%s",
								   mP->name.c_str(), mP->type.c_str(), mP->value.c_str(),
								   mP->attributeP->name.c_str(), mP->attributeP->type.c_str(),
								   mP->attributeP->entityP->id.c_str(), mP->attributeP->entityP->type.c_str()));
        mP = mP->next;
    }

    LM_T(LmtMetadatasPresent, (""));
}
