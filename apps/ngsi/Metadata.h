#ifndef METADATA_H
#define METADATA_H

/* ****************************************************************************
*
* FILE                  Metadata.h
*
*
*
*
*/
#include <string>
using namespace std;



/* ****************************************************************************
*
* Metadata - 
*/ 
typedef struct Metadata
{
    std::string        name;
    std::string        type;
    std::string        value;
	struct Attribute*  attributeP;
	struct Metadata*   next;
} Metadata;

#endif
