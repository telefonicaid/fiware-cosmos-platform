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
extern Registration* registrationAdd(std::string registrationId);



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
extern int registrationToDb(std::string id, unsigned int* dbIdP = NULL);

#endif
