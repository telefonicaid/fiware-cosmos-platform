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
#include <vector>

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
    bool               isDomain;
    std::string        type;
	std::string        ID;     // From metadata ...
	vector<Metadata*>  metadataV;
	struct Attribute*  next;
	Entity*            entityP;
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
