/* ****************************************************************************
*
* FILE                     permissions.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/
#include "permissions.h"        // Own interface



/* ****************************************************************************
*
* permissionName - 
*/
const char* permissionName(Permissions p)
{
	switch (p)
	{
	case UpStartProcesses:       return "StartProcesses";
	case UpStopProcesses:        return "StopProcesses";
	case UpDelilah:              return "Delilah";
	case UpDelilahConsole:       return "DelilahConsole";
	case UpCreateQueues:         return "CreateQueues";
	case UpRemoveQueues:         return "RemoveQueues";
	case UpSeeLogs:              return "SeeLogs";
	case UpAll:                  return "ALL permissions";
	case UpNothing:              return "NO permissions";
	case UpRawPlatformAccess:    return "RawPlatformAccess";
	}

	return "Unknown Permission";
}
