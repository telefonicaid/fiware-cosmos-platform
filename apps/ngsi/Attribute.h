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
extern Attribute* attributeLookup(struct Entity* entityP, std::string name, std::string type, std::string ID);



/* ****************************************************************************
*
* attributeToDb - 
*/
extern int attributeToDb(Entity* entityP, Attribute* attribute);

#endif
