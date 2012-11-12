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
#ifndef ENTITY_H
#define ENTITY_H

/* ****************************************************************************
*
* FILE                  Entity.h 
*
*
*
*
*/
#include <string>

#include "Format.h"
#include "ContextRegistrationAttributeList.h"

using namespace std;



/* ****************************************************************************
*
* Entity - 
*/
typedef struct Entity
{
    int              dbId;
	std::string      id;
	std::string      type;
	bool             isPattern;
	std::string      providingApplication;
	time_t           startTime;
	time_t           endTime;
	std::string      registrationId;
	struct Entity*   next;
} Entity;



/* ****************************************************************************
*
* entityInit - 
*/
extern void    entityInit(void);



/* ****************************************************************************
*
* entityAdd - 
*/
extern Entity* entityAdd
(
    Entity*                            rcrEntity,
	std::string                        providingApplication,
	int                                duration,
	std::string                        registrationId,
	ContextRegistrationAttributeList*  attributeList,
	std::string*                       errorString
);



/* ****************************************************************************
*
* entityUpdate - 
*/
extern Entity* entityUpdate
(
    Entity*                            rcrEntity,
	std::string                        providingApplication,
	int                                duration,
	std::string                        registrationId,
	ContextRegistrationAttributeList*  attributeList,
	std::string*                       errorString
);



/* ****************************************************************************
*
* entityLookup -
*/
extern Entity* entityLookup(std::string id, std::string type);



/* ****************************************************************************
*
* entityLookupByDbId -
*/
extern Entity* entityLookupByDbId(int dbId);



/* ****************************************************************************
*
* entityToDb - 
*/
extern int entityToDb(Entity* entityP, bool update, long duration, std::string* errorString);



/* ****************************************************************************
*
* entityPresent - 
*/
extern void entityPresent(Entity* entityP);



/* ****************************************************************************
*
* entitiesPresent - 
*/
extern void entitiesPresent(bool force = false);

#endif
