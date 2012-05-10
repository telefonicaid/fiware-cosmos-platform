/* ****************************************************************************
*
* FILE                  Entity.h 
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
	registrationV  = NULL;
	entities = 0;
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
static Registration* registrationLookup(std::string id)
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
	
	snprintf(s, sLen, "%dT%d", now, registrationNo);
	++registrationNo;

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

	LM_M(("************ PLEASE IMPLEMENT !!! ************"));

	return regP;
}



/* ****************************************************************************
*
* registrationAdd - 
*/
Registration* registrationAdd(std::string registrationId, vector<Metadata*> metadataV)
{
	Registration* regP;

	regP = registrationLookup(registrationId);
	if (regP != NULL)
		LM_RE(NULL, ("registration with id '%s' already exists", registrationId.c_str()));

	regP = new Registration();

	regP->id = registrationId;

	for (int ix = 0; ix < attributeV.size(); ix++)
		regP->attributeV.push_back(attributeV[ix]);

	registrationAppend(regP);

	return regP:
}



/* ****************************************************************************
*
* registrationLookup - 
*/
Registration* registrationLookup(std::string registrationId)
{
}



/* ****************************************************************************
*
* registrationLookup - 
*/
Registration* registrationLookup(std::string registrationId)
{
}
