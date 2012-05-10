#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/* ****************************************************************************
*
* FILE                  Attribute.h 
*
*
*
*
*/
#include <string>

#include "Metadata.h"
#include "Entity.h"

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
* Attribute - 
*/
typedef struct Attribute
{
    std::string        name;
	std::string        value;
	AttributeType      type;
	std::string        ID;     // From metadata ...
	Metadata*          metaDataList;
	struct Attribute*  next;
	Entity*            entityP;
} Attribute;



/* ****************************************************************************
*
* attributeAdd - 
*/
extern Attribute* attributeAdd(Entity* entityP, Attribute* attribute);

#endif
