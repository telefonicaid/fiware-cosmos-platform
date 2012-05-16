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
    std::string        name;
    std::string        type;
	std::string        value;
	std::string        metaID;     // From metadata named 'ID' ...
    bool               isDomain;
	vector<Metadata*>  metadataV;
	struct Attribute*  next;
	struct Entity*     entityP;
} Attribute;



/* ****************************************************************************
*
* attributeAdd - 
*/
extern Attribute* attributeAdd(Entity* entityP, Attribute* attribute);



/* ****************************************************************************
*
* attributeLookup - 
*/
extern Attribute* attributeLookup(std::string name, std::string type, std::string ID);

#endif
