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
* attributeCreate - 
*/
Attribute* attributeCreate(Entity* entityP, int dbId, std::string name, std::string type, std::string value, std::string metaId, bool isDomain)
{
	Attribute* attributeP = new Attribute();

	attributeP->entityP   = entityP;
	attributeP->dbId      = dbId;
	attributeP->name      = name;
	attributeP->type      = type;
	attributeP->value     = value;
	attributeP->metaId    = metaId;
	attributeP->isDomain  = isDomain;
	attributeP->next      = NULL;

	LM_T(LmtAttribute, ("Created an attribute: '%s:%s:%s'", attributeP->name.c_str(), attributeP->type.c_str(), attributeP->value.c_str()));

	return attributeP;
}



static Attribute* attributeList = NULL;
static Attribute* attributeLast = NULL;
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
}



/* ****************************************************************************
*
* attributeLookup - 
*/
Attribute* attributeLookup(Entity* entityP, std::string name, std::string type, std::string metaId)
{
	Attribute* aP;

	aP = attributeList;
	while (aP != NULL)
	{
		if ((aP->entityP == entityP) && (aP->name == name) && (aP->type == type) && (aP->metaId == metaId))
			return aP;

		aP = aP->next;
	}

	LM_X(1, ("try to lookup attribute in DB"));

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
int attributeToDb(Entity* entityP, Attribute* attribute)
{
	int          s;
	std::string  isDomain = (attribute->isDomain == true)? "Yes" : "No";
	std::string  query    = "INSERT into attribute (name, type, value, metaID, isDomain) VALUES ('" + attribute->name + "', '" + attribute->type + "', '" + attribute->value + "', '" + attribute->metaId + "', '" + isDomain + "')";

	LM_T(LmtDbEntity, ("SQL to insert a new Attribute: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	attribute->dbId = mysql_insert_id(db);

	char entityDbId[32];
	char attributeDbId[32];

	sprintf(entityDbId,    "%d", entityP->dbId);
	sprintf(attributeDbId, "%d", attribute->dbId);

	query = std::string("INSERT into entityAttribute (entityId, attributeId) VALUES (") + entityDbId + ", " + attributeDbId + ")";
	LM_T(LmtDbEntity, ("SQL to insert a new Entity-Attribute: '%s'", query.c_str()));
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return -1;
	}

	return 0;
}
