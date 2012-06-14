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
    LmtCleanup                = 32,  // Valgrind ...
    LmtCleanup2               = 33,  // Valgrind ...
    LmtRestData               = 34,
    LmtDelilahCommand         = 35,

	LmtEngine                 = 200, // SamsonEngine
	LmtEngineNotification     = 201,
	LmtEngineTime             = 202,
	LmtDisk                   = 203, // disk library	
	LmtMemory                 = 204, // memory library	
	LmtProcessManager         = 204,
	LmtBlockManager           = 205,

    LmtBuffer                 = 206, 

	LmtModuleManager          = 210,                // Module Manager component
	LmtThreadManager          = 211,    // Tracking threads in ThreadManager

    
	LmtIsolated               = 215, // Isolated mechanism
    LmtIsolatedOperations     = 216, // Traces to see start and stop of executions
    LmtReduceOperation        = 217, // Reduce operation steps
    LmtIsolatedOutputs        = 218, // See processing of operations outputs
    LmtFileDescriptors        = 219, // to keep track of opening/closing file descriptors
    
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
