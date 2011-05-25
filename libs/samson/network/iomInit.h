#ifndef IOM_INIT
#define IOM_INIT

/* ****************************************************************************
*
* FILE                     iomInit.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <vector>               // vector

#include "samson/network/Endpoint.h"			// Endpoint



/* ****************************************************************************
*
* iomInit -  connect to the controller
*/
extern void iomInit(samson::Endpoint* controller);

#endif
