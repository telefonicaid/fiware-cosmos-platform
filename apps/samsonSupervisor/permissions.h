#ifndef PERMISSIONS_H
#define PERMISSIONS_H

/* ****************************************************************************
*
* FILE                     permissions.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/



/* ****************************************************************************
*
* Permissions
*/
typedef enum Permissions
{
	UpAll            = 0xFFFFFFFF,
	UpNothing        = 0,
	UpStartProcesses = (1 << 0),
	UpStopProcesses  = (1 << 1),
	UpDelilah        = (1 << 2),
	UpDelilahConsole = (1 << 3),
	UpCreateQueues   = (1 << 4),
	UpRemoveQueues   = (1 << 5),
	UpSeeLogs        = (1 << 6),
} Permissions;



/* ****************************************************************************
*
* permissionName - 
*/
const char* permissionName(Permissions p);

#endif

