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
* FILE                  Registration.cpp
*
*
*
*
*/
#include <string>

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "globals.h"                        // TF
#include "Metadata.h"                       // Metadata
#include "database.h"                       // db, ...
#include "Registration.h"                   // Own interface

using namespace std;



/* ****************************************************************************
*
* Global variables
*/
static int             registrationNo    = 1;
static Registration*   registrationList  = NULL;
static Registration*   registrationLast  = NULL;



/* ****************************************************************************
*
* registrationInit - 
*/
void registrationInit(void)
{
	registrationList   = NULL;
	registrationNo  = 1;
}



/* ****************************************************************************
*
* registrationCreate - 
*/
static Registration* registrationCreate(std::string id)
{
	Registration* regP = new Registration();

	if (regP == NULL)
		LM_X(1, ("operator 'new' failed for a Registration ..."));

	regP->id = id;

	return regP;
}



/* ****************************************************************************
*
* registrationAppend - 
*/
static void registrationAppend(Registration* registration)
{
	LM_T(LmtRegistration, ("Appending registration '%s'", registration->id.c_str()));

	if (registrationLast == NULL)
		registrationList = registration;
	else
		registrationLast->next = registration;

	registrationLast = registration;
}



/* ****************************************************************************
*
* registrationLookupInDb - 
*/
static Registration* registrationLookupInDb(std::string id)
{
    MYSQL_RES*     result;
    MYSQL_ROW      row;
    int            rows;
    int            s;

	std::string query = "SELECT * from registration WHERE id = '" + id + "'";
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
        LM_X(1, ("Found more than ONE instance of registration '%s", id.c_str()));
	
	row = mysql_fetch_row(result);
	mysql_free_result(result);

	Registration* regP = registrationCreate(id);
	registrationAppend(regP);
	return regP;
}



/* ****************************************************************************
*
* registrationLookup -
*/
Registration* registrationLookup(std::string id)
{
	Registration* registrationP = registrationList;

	while (registrationP != NULL)
	{
		if (registrationP->id == id)
			return registrationP;

		registrationP = registrationP->next;
	}

	//
	// Not found in RAM - look it up in DB and save to RAM if found
	//
	LM_T(LmtRegistrationLookup, ("Registration '%s' not in RAM - looking it up in the database ...", id.c_str()));
	registrationP = registrationLookupInDb(id);

	return registrationP;
}



/* ****************************************************************************
*
* registrationIdGet - 
*/
char* registrationIdGet(char* s, int sLen)
{
	time_t now = time(NULL);
	
	snprintf(s, sLen, "%dT%03d", (int) now, registrationNo);
	++registrationNo;
	
	if (registrationNo >= 1000)
	    registrationNo = 1;

	return s;
}



/* ****************************************************************************
*
* registrationUpdate - 
*/
Registration* registrationUpdate(std::string registrationId, vector<Metadata*> metadataV)
{
	Registration* regP = registrationLookup(registrationId);

	if (regP == NULL)
		LM_RE(NULL, ("Can't find registration with id '%s'", registrationId.c_str()));

	LM_W(("************ PLEASE IMPLEMENT !!! ************"));

	return regP;
}



/* ****************************************************************************
*
* registrationAdd - 
*/
Registration* registrationAdd(std::string registrationId, vector<Metadata*> metadataV)
{
	unsigned int  ix;
	Registration* regP;

	regP = registrationLookup(registrationId);
	if (regP != NULL)
		LM_RE(NULL, ("registration with id '%s' already exists", registrationId.c_str()));

	regP = registrationCreate(registrationId);

	for (ix = 0; ix < metadataV.size(); ix++)
		regP->metadataV.push_back(metadataV[ix]);

	registrationAppend(regP);

	return regP;
}



/* ****************************************************************************
*
* registrationAdd - 
*/
Registration* registrationAdd(std::string registrationId)
{
	Registration* regP;

	regP = registrationLookup(registrationId);
	if (regP != NULL)
		LM_RE(NULL, ("registration with id '%s' already exists", registrationId.c_str()));

	regP = registrationCreate(registrationId);
	registrationAppend(regP);

	return regP;
}



/* ****************************************************************************
*
* registrationToDb - 
*/
int registrationToDb(std::string id, unsigned int* dbIdP)
{
	char query[512];
	int  s;

	snprintf(query, sizeof(query), "INSERT INTO cm.registration (`id`) VALUES ('%s');", id.c_str());

	LM_T(LmtDbEntity, ("SQL to insert a new Registration: '%s'", query));
	LM_T(LmtSqlQuery, ("SQL Query is '%s'", query));
	s = mysql_query(db, query);
	if (s != 0)
		LM_RE(-1, ("mysql_query(%s): %s", query, mysql_error(db)));

	if (dbIdP != NULL)
		*dbIdP = mysql_insert_id(db);

	return 0;
}
