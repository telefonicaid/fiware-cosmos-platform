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
* FILE                  
*
*
*
*
*/
#include <string>
#include <vector>

#include "Entity.h"
#include "ContextRegistrationAttributeList.h"
#include "Metadata.h"

using namespace std;



/* ****************************************************************************
*
* RegisterContextRequest - 
*/
typedef struct RegisterContextRequest
{
    vector<Entity*>                  entityV;
    ContextRegistrationAttributeList attributeList;
    vector<Metadata*>                registrationMetadataV;
    std::string                      providingApplication;
    std::string                      duration;
    std::string                      registrationId;
} RegisterContextRequest;

#endif
