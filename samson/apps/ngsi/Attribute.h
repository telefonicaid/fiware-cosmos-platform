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
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/* ****************************************************************************
*
* FILE                     Attribute.h - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 15 2012
*
*
*
*/
#include <string>
#include <vector>

#include "Metadata.h"

using namespace std;



/* ****************************************************************************
*
* AttributeType - 
*/
typedef enum AttributeType
{
	IntegerAttribute,
	StringAttribute,
	BooleanAttribute,
	DoubleAttribute
} AttributeType;



/* ****************************************************************************
*
* Forward declarations
*/
struct Entity;



/* ****************************************************************************
*
* Attribute - 
*/
typedef struct Attribute
{
	int                dbId;
    std::string        name;
    std::string        type;
	std::string        value;
	std::string        metaId;     // From metadata named 'ID' ...
    bool               isDomain;
	vector<Metadata*>  metadataV;
	struct Attribute*  next;
	struct Entity*     entityP;
} Attribute;



/* ****************************************************************************
*
* Global variables
*/
extern Attribute*         attributeList;
extern int                attributes;



/* ****************************************************************************
*
* attributeInit - 
*/
extern void attributeInit(void);



/* ****************************************************************************
*
* attributeCreate - 
*/
extern Attribute* attributeCreate(Entity* entityP, int dbId, std::string name, std::string type, std::string value, std::string metaId, bool isDomain);



/* ****************************************************************************
*
* attributeUpdate - 
*/
extern void attributeUpdate(Attribute* attribute, Attribute* updateFrom);



/* ****************************************************************************
*
* attributeAppend - 
*/
extern void attributeAppend(Attribute* attribute);



/* ****************************************************************************
*
* attributeLookup - 
*/
extern Attribute* attributeLookup(struct Entity* entityP, std::string name, std::string type, std::string ID, bool onlyRAM);
extern Attribute* attributeLookup(struct Entity* entityP, std::string name);



/* ****************************************************************************
*
* attributeToDb - 
*/
extern int attributeToDb(Entity* entityP, Attribute* attribute, bool update);



/* ****************************************************************************
*
* attributesPresent - 
*/
extern void attributesPresent(bool force = false);

#endif
