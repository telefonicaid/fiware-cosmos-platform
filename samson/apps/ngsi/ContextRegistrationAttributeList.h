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
