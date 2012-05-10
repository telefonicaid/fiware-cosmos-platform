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

#include "Metadata.h"
#include "ContextRegistrationAttribute.h"

using namespace std;



/* ****************************************************************************
*
* ContextRegistrationAttributeList - 
*/
typedef struct ContextRegistrationAttributeList
{
	vector<ContextRegistrationAttribute*> attributeV;
} ContextRegistrationAttributeList;

#endif
