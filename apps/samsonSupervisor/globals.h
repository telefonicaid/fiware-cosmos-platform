#ifndef GLOBALS_H
#define GLOBALS_H

/* ****************************************************************************
*
* FILE                     globals.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include "Network.h"            // Network
#include "SamsonSupervisor.h"   // SamsonSupervisor
#include "TabManager.h"         // TabManager



class SamsonSupervisor;
/* ****************************************************************************
*
* Global variables
*/
extern ss::Network*       networkP;
extern SamsonSupervisor*  supervisorP;
extern TabManager*        tabManager;



/* ****************************************************************************
*
* Option variables
*/
extern int     endpoints;
extern char    controllerName[80];
extern char    cfPath[80];
extern bool    qt;

#endif
