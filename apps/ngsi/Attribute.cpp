/* ****************************************************************************
*
*  FILE                    Attribute.cpp -  
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 15 2012
*
*
*
*/
#include <string.h>                         // strcmp, ...
#include <string>                           // std::string
#include <stdlib.h>                         // malloc
#include <errno.h>                          // errno

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "Entity.h"                         // Entity
#include "Attribute.h"                      // Own interface

using namespace std;



/* ****************************************************************************
*
* attributeAdd - 
*/
Attribute* attributeAdd(Entity* entityP, Attribute* attribute)
{
	LM_M(("Please implement me!"));
    return NULL;
}



/* ****************************************************************************
*
* attributeLookup - 
*/
Attribute* attributeLookup(std::string name, std::string type, std::string ID)
{
	LM_M(("Please implement me!"));
    return NULL;
}
