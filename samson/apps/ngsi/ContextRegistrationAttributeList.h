#ifndef CONTEXT_REGISTRATION_ATTRIBUTE_LIST_H
#define CONTEXT_REGISTRATION_ATTRIBUTE_LIST_H

/* ****************************************************************************
*
* FILE                  ContextRegistrationAttributeList.h
*
*
*
*
*/
#include <string>
#include <vector>

#include "Metadata.h"
#include "ContextRegistrationAttribute.h"
#include "Attribute.h"

using namespace std;



/* ****************************************************************************
*
* ContextRegistrationAttributeList - 
*/
typedef struct ContextRegistrationAttributeList
{
	vector<Attribute*> attributeV;
} ContextRegistrationAttributeList;

#endif
