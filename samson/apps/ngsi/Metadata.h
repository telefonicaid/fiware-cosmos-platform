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
#ifndef METADATA_H
#define METADATA_H

/* ****************************************************************************
*
* FILE                  Metadata.h
*
*
*
*
*/
#include <string>

using namespace std;



/* ****************************************************************************
*
* Forward declarations
*/
struct Attribute;



/* ****************************************************************************
*
* Metadata - 
*/ 
typedef struct Metadata
{
	int                dbId;
    std::string        name;
    std::string        type;
    std::string        value;
	struct Attribute*  attributeP;
	struct Metadata*   next;
} Metadata;



/* ****************************************************************************
*
* Globals
*/
extern Metadata*        metadataList;



/* ****************************************************************************
*
* metadataCreate - 
*/
extern Metadata* metadataCreate(struct Attribute* attribute, std::string name, std::string type, std::string value);



/* ****************************************************************************
*
* metadataAppend - 
*/
extern void metadataAppend(Metadata* metadata);



/* ****************************************************************************
*
* metadataUpdate - 
*/
extern int metadataUpdate(Metadata* metadata, Metadata* updateFrom);



/* ****************************************************************************
*
* metadataLookup - 
*/
Metadata* metadataLookup(struct Attribute* aP, std::string name, std::string type);



/* ****************************************************************************
*
* metadataToDb - 
*/
extern int metadataToDb(Attribute* attributeP, Metadata* metadata);
extern int metadataToDb(unsigned int registrationDbId, Metadata* metadata);



/* ****************************************************************************
*
* metadatasPresent - 
*/
extern void metadatasPresent(bool force = false);

#endif
