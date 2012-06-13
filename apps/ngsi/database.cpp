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
* Preparing mysql (MacOS):
*   % sudo /usr/local/mysql/bin/mysqld_safe (use the alias 'mysql_start')
*   % mysql -u root
*   mysql> create database cm;
*   mysql> create user 'cm' identified by 'cm';
*   mysql> grant all on cm.* to 'cm'@'localhost' identified by 'cm';
*
*   Now implement a function to create all tables, one function per table,
*   and dbInit should detect missing tables and create them.
*   - to be taken from tora, where all tables are already created:
*   % mysqldump -u cm -p cm > guestdb.txt
*
* CHECK
* tag-delimited format for REST output:
*   PUT /x/y/z Content-type:test/plain
*       Name=ShipStopped;ShipID=RTX33;Long=46; ...
*/
#include <mysql.h>                              // mysql

#include "logMsg/logMsg.h"                      // LM_*

#include "traceLevels.h"                        // Trace levels for log msg library
#include "Registration.h"                       // Registration
#include "Metadata.h"                           // Metadata 
#include "database.h"                           // Own interface



/* ****************************************************************************
*
* db - database connection
*/
MYSQL*     db = NULL;



/* ****************************************************************************
*
* DbTable - 
*/
const std::string  DbTable[] =
{
   "attribute",
   "attributeMetadata",
   "entity",
   "entityAttribute",
   "metadata",
   "registration",
   "registrationMetadata",
   "__STOP__"
};



/* ****************************************************************************
*
* dbConnect - 
*/
static int dbConnect(void)
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
* dbTableExists - 
*/
static bool dbTableExists(std::string tableName)
{
	int          s;
	std::string  query;
	MYSQL_RES*   result;
	int          results;
	MYSQL_ROW    row;
	int          rows;

	// query = "SELECT TABLE_NAME FROM information_schema.tables WHERE table_schema = 'information_schema' AND table_name = '" + tableName + "'";
	query = "DESCRIBE cm." + tableName;
	s = mysql_query(db, query.c_str());
	if (s != 0)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	if ((result = mysql_store_result(db)) == 0)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	results = mysql_num_fields(result);
	rows    = 0;
	while ((row = mysql_fetch_row(result)))
		++rows;

	mysql_free_result(result);
	if (rows == 0)
	{
		LM_T(LmtDbTable, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
		return false;
	}

	LM_T(LmtDbTable, ("mysql_query(%s): table exists", query.c_str()));
	return true;
}



/* ****************************************************************************
*
* dbTableCreate - 
*/
static void dbTableCreate(std::string tableName)
{
	std::string  query;
	int          s;

	query = std::string("create table cm.") + tableName + " (";

	if (tableName == "attribute")
	{
		query += "`dbId`       int(10) unsigned  NOT NULL AUTO_INCREMENT,";
		query += "`name`       varchar(256)      NOT NULL,";
		query += "`type`       varchar(256)      NOT NULL,";
		query += "`value`      varchar(256)      NOT NULL,";
		query += "`metaID`     varchar(256)      NOT NULL,";
		query += "`isDomain`   enum('No','Yes')  NOT NULL,";
		query += "`timestamp`  timestamp         NOT NULL DEFAULT CURRENT_TIMESTAMP,";

		query += "PRIMARY KEY (`dbId`)";
	}
	else if (tableName == "attributeMetadata")
	{
		query += "`attributeId` int(10) unsigned  NOT NULL,";
		query += "`metadataId`  int(10) unsigned  NOT NULL";
	}
	else if (tableName == "entity")
	{
		query += "`dbId`                  int(10) unsigned     NOT NULL AUTO_INCREMENT,";
		query += "`id`                    varchar(256)         NOT NULL,";
		query += "`type`                  varchar(256)         DEFAULT NULL,";
		query += "`isPattern`             enum('No','Yes')     NOT NULL,";
		query += "`providingApplication`  varchar(256)         DEFAULT NULL,";
		query += "`startTime`             bigint(64)           unsigned NOT NULL,";
		query += "`endTime`               bigint(64) unsigned  NOT NULL,";
		query += "`registrationId`        varchar(128)         NOT NULL,";
		query += "`timestamp`             timestamp            NOT NULL DEFAULT CURRENT_TIMESTAMP,";

		query += "PRIMARY KEY (`dbId`)";
	}
	else if (tableName == "entityAttribute")
	{
		query += "`entityId`    int(10) unsigned  NOT NULL,";
		query += "`attributeId` int(10) unsigned  NOT NULL";
	}
	else if (tableName == "metadata")
	{
		query += "`dbId`       int(10) unsigned  NOT NULL AUTO_INCREMENT,";
		query += "`name`       varchar(256)      NOT NULL,";
		query += "`type`       varchar(256)      NOT NULL,";
		query += "`value`      varchar(256)      NOT NULL,";
		query += "`timestamp`  timestamp         NOT NULL DEFAULT CURRENT_TIMESTAMP,";

		query += "PRIMARY KEY (`dbId`)";
	}
	else if (tableName == "registration")
	{
		query += "`dbId`      int(10) unsigned  NOT NULL AUTO_INCREMENT,";
		query += "`id`        varchar(100)      NOT NULL,";
		query += "`timestamp` timestamp         NOT NULL DEFAULT CURRENT_TIMESTAMP,";

		query += "PRIMARY KEY (`dbId`)";
	}
	else if (tableName == "registrationMetadata")
	{
		query += "`registrationId` int(10) unsigned NOT NULL,";
		query += "`metadataId`     int(10) unsigned NOT NULL";
	}
	else
		LM_X(1, ("bad database table name: '%s'", tableName.c_str()));

	query += ") ENGINE=MyISAM DEFAULT CHARSET=utf8;";

	s = mysql_query(db, query.c_str());
	if (s != 0)
		LM_X(1, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));

	LM_T(LmtDbTable, ("executed SQL: %s", query.c_str()));
}



/* ****************************************************************************
*
* dbInit - 
*/
int dbInit(void)
{
	int   ix = 0;
	int   s;

	if ((s = dbConnect()) != 0)
		LM_RE(s, ("error connecting to mysql database"));

	while (DbTable[ix] != "__STOP__")
	{
		if (dbTableExists(DbTable[ix]) == false)
		{
			LM_T(LmtDbTable, ("db table '%s' doesn't exist - creating it", DbTable[ix].c_str()));
			dbTableCreate(DbTable[ix]);
		}
		LM_T(LmtDbTable, ("db table '%s' already exists - not creating it", DbTable[ix].c_str()));

		++ix;
	}

	return 0;
}



/* ****************************************************************************
*
* dbReset - 
*/
int dbReset(void)
{
	int          ix = 0;

	while (DbTable[ix] != "__STOP__")
	{
		std::string  query;
		int          s;

		query = "DELETE FROM cm." + DbTable[ix];
		s = mysql_query(db, query.c_str());
		if (s != 0)
			LM_RE(-1, ("mysql_query(%s): %s", query.c_str(), mysql_error(db)));

		++ix;
	}

	return 0;
}



/* ****************************************************************************
*
* dbItemsInTable - 
*/
int dbItemsInTable(std::string table)
{
	int          s;
    int          hits;
    MYSQL_RES*   result  = NULL;
	std::string  query   = "SELECT * FROM " + table;
	
    s = mysql_query(db, query.c_str());
    if (s != 0)
    {
        LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
        return 0;
    }

    if ((result = mysql_store_result(db)) == NULL)
    {
        LM_E(("mysql_query(%s): %s", query.c_str(), mysql_error(db)));
        return 0;
    }

	hits = mysql_num_rows(result);
	mysql_free_result(result);

	LM_T(LmtDbTable, ("Got %d hits in DB table %s", hits, table.c_str()));
	return hits;
}
