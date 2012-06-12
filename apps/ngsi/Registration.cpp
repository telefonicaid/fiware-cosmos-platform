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
static Registration*   registrationV     = NULL;
static Registration*   registrationNext  = NULL;



/* ****************************************************************************
*
* registrationInit - 
*/
void registrationInit(void)
{
	registrationV   = NULL;
	registrationNo  = 1;
}



/* ****************************************************************************
*
* registrationAppend - 
*/
static void registrationAppend(Registration* registration)
{
	LM_T(LmtRegistration, ("Appending registration '%s'", registration->id.c_str()));

	if (registrationNext == NULL)
		registrationV = registration;
	else
		registrationNext->next = registration;

	registrationNext = registration;
}



/* ****************************************************************************
*
* registrationLookup -
*/
Registration* registrationLookup(std::string id)
{
	Registration* registrationP = registrationV;

	while (registrationP != NULL)
	{
		if (registrationP->id == id)
			return registrationP;

		registrationP = registrationP->next;
	}

	return NULL;
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

	regP = new Registration();

	regP->id = registrationId;

	for (ix = 0; ix < metadataV.size(); ix++)
		regP->metadataV.push_back(metadataV[ix]);

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
