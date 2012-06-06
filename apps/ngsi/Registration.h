#ifndef REGISTRATION_H
#define REGISTRATION_H

/* ****************************************************************************
*
* FILE                  Registration.h 
*
*
*
*
*/
#include <string>
#include <vector>

#include "Format.h"                         // Format
#include "Metadata.h"                       // Metadata

using namespace std;



/* ****************************************************************************
*
* Registration - 
*/
typedef struct Registration
{
    std::string           id;
    vector<Metadata*>     metadataV;
	struct Registration*  next;
} Registration;



/* ****************************************************************************
*
* registrationIdGet - 
*/
extern char* registrationIdGet(char* s, int sLen);



/* ****************************************************************************
*
* registrationAdd - 
*/
extern Registration* registrationAdd(std::string registrationId, vector<Metadata*> metadataV);



/* ****************************************************************************
*
* registrationUpdate - 
*/
extern Registration* registrationUpdate(std::string registrationId, vector<Metadata*> V);



/* ****************************************************************************
*
* registrationLookup -
*/
extern Registration* registrationLookup(std::string id);



/* ****************************************************************************
*
* registrationToDb - 
*/
extern int registrationToDb(std::string id);

#endif
