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
* metadataCreate - 
*/
Metadata* metadataCreate(struct Attribute* attribute, std::string name, std::string type, std::string value)
{
	Metadata* metadataP = new Metadata();

	metadataP->name  = name;
	metadataP->type  = type;
	metadataP->value = value;
	metadataP->next  = NULL;

	LM_T(LmtMetadata, ("Created an metadata: '%s:%s:%s'", metadataP->name.c_str(), metadataP->type.c_str(), metadataP->value.c_str()));

	return metadataP;
}



static Metadata* metadataList = NULL;
static Metadata* metadataLast = NULL;
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
}



/* ****************************************************************************
*
* metadataUpdate - 
*
* Should it be possible to update metadatas?
* And if so, only type and value?
*/
int metadataUpdate(Metadata* metadata, Metadata* updateFrom)
{
	std::string  oldType = metadata->type;
	int          s;

	metadata->type  = updateFrom->type;
	metadata->value = updateFrom->value;

	std::string query = "UPDATE metadata SET type='" + metadata->type + ", value='" + metadata->value + "' WHERE name='" + metadata->name + "' AND type='" + oldType + "'";
	s = mysql_query(db, query.c_str());
	if (s != 0)
		LM_RE(-1, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));

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
* metadataToDb - 
*/
int metadataToDb(Attribute* attributeP, Metadata* metadata)
{
	int          s;
	std::string  query    = "INSERT into metadata (name, type, value) VALUES ('" + metadata->name + "', '" + metadata->type + "', '" + metadata->value + "')";

	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	metadata->dbId = mysql_insert_id(db);

	char attributeDbId[32];
	char metadataDbId[32];

	sprintf(attributeDbId, "%d", attributeP->dbId);
	sprintf(metadataDbId, "%d", metadata->dbId);

	query = std::string("INSERT into attributeMetadata (attributeId, metadataId) VALUES (") + attributeDbId + ", " + metadataDbId + ")";
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	return 0;
}
