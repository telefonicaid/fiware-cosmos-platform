/* ****************************************************************************
*
* FILE                     database.cpp - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 16 2012
*
*
*
*/
#include <mysql.h>                          // mysql

#include "logMsg/logMsg.h"                  // LM_*

#include "Registration.h"                   // Registration
#include "Metadata.h"                       // Metadata 
#include "database.h"                       // Own interface



/* ****************************************************************************
*
* db - database connection
*/
static MYSQL*     db = NULL;



/* ****************************************************************************
*
* dbConnect - 
*/
int dbConnect(void)
{
	const char*  server     = "localhost";
	const char*  database   = "cm";
	const char*  user       = "cm";
	const char*  pwd        = "cm";

	db = mysql_init(NULL);
	if (!mysql_real_connect(db, server, user, pwd, database, 0, NULL, 0))
		LM_RE(-1, ("mysql_real_connect: %s", mysql_error(db)));

	return 0;
}



/* ****************************************************************************
*
* dbReset - 
*/
int dbReset(void)
{
	int          ix;
	const char*  dbTable[] =
		{
			"attribute",
			"attributeMetadata",
			"entity",
			"entityAttribute",
			"metadata",
			"registration",
			"registrationMetadata",
			NULL
		};

	ix = 0;
	while (dbTable[ix] != NULL)
	{
		char query[256];
		int  s;

		snprintf(query, sizeof(query), "DELETE FROM %s", dbTable[ix]);
		s = mysql_query(db, query);
		if (s != 0)
			LM_RE(-1, ("mysql_query(%s): %s", query, mysql_error(db)));

		++ix;
	}

	return 0;
}



/* ****************************************************************************
*
* dbRegistrationAdd - 
*/
int dbRegistrationAdd(std::string id)
{
	char query[512];
	int  s;

	snprintf(query, sizeof(query), "INSERT INTO registration (`id`) VALUES ('%s');", id.c_str());

	s = mysql_query(db, query);
	if (s != 0)
		LM_RE(-1, ("mysql_query(%s): %s", query, mysql_error(db)));

	return 0;
}



/* ****************************************************************************
*
* dgRegistrationMetadataAdd - 
*/
int dgRegistrationMetadataAdd(Registration* regP, Metadata* metadata)
{
	return 0;
}
