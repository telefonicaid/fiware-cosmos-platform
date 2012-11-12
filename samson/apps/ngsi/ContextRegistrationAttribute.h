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
#ifndef CONTEXT_REGISTRATION_ATTRIBUTE_H
#define CONTEXT_REGISTRATION_ATTRIBUTE_H

/* ****************************************************************************
*
* FILE                  ContextRegistrationAttribute.h
*
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
* ContextRegistrationAttribute - 
*/
typedef struct ContextRegistrationAttribute
{
    std::string         name;
    std::string         type;
    bool                isDomain;
    vector<Metadata*>   metadataV;
} ContextRegistrationAttribute;

#endif
