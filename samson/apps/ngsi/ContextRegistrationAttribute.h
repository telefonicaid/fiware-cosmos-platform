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
