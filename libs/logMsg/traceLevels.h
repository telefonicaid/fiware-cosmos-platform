#ifndef TRACE_LEVELS_H
#define TRACE_LEVELS_H

/* ****************************************************************************
*
* FILE                     traceLevels.h - trace levels for entire Samson project
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/



/* ****************************************************************************
*
* TraceLevels - 
*/
typedef enum TraceLevels
{
	LmtFile,

    LmtExcesiveTime           = 5,
    
    LmtSocketConnection       = 10,  // 
    LmtNetworkListener        = 11,  // 
    
    LmtNetworkConnection      = 15,  // 
    
    LmtNetworkNodeMessages    = 20,  // Worker & Delilah messages
    LmtNetworkInterface       = 22,  // Network interface 

    LmtHost                   = 30,  // Host & HostManager    
    LmtRest                   = 31,  // REST Interface

	LmtEngine                 = 200, // SamsonEngine
	LmtEngineNotification,
	LmtEngineTime,
    
    
	LmtDisk,                         // disk library	
	LmtMemory,                       // memory library	
	LmtProcessManager,
	LmtBlockManager,

	LmtModuleManager,                // Module Manager component
	LmtOperations,                   // Operations organization (map & reduce level)
    
	LmtIsolated,                     // Isolated mechanism
    
	LmtUser01 = 250,
	LmtUser02 = 251,
	LmtUser03 = 252,
	LmtUser04 = 253,
	LmtUser05 = 254,
	LmtUser06 = 255,
} TraceLevels;



/* ****************************************************************************
*
* traceLevelName - 
*/
extern char* traceLevelName(TraceLevels level);

#endif
