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

#endif
