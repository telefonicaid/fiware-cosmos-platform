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
#include "Endpoint.h"			// Endpoint



namespace ss
{


/* ****************************************************************************
*
* iomInit -  connect to the controller
*/
extern void iomInit(Endpoint* controller);

}

#endif
