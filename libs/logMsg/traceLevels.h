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
//
// Network Trace Levels 20-50
//
	LmtInit         = 20,
	LMT_NWRUN,
	LMT_SELECT,
	LMT_FDS,
	LMT_ENDPOINT,
	LMT_ACCEPT,
	LMT_ENDPOINTS,
	LMT_READ,
	LMT_WORKERS,
	LMT_WRITE,
	LMT_MSG,

	LMT_INIT_EP,
	LMT_CONFIG,
	LMT_HELLO,
	LMT_TREAT,
	LMT_RECONNECT,
	LMT_DELILAH,
	LMT_STAT,
	LMT_EP,
	LMT_RESTART,
	LMT_COREWORKER,

	LMT_MSGTREAT,
	LMT_JOB,
	LMT_TIMEOUT,
	LMT_FORWARD,
	LMT_SEND,



//
// Process Trace Levels  51-60
//
	LMT_PA  = 51,



//
// Data Trace Levels  61-70
//
	LMT_FILE    = 61,



//
// Core Trace Levels  71-80
//
	LMT_TASK             = 71,
	LMT_SAMSON_WORKER,
	LMT_WINIT,



//
// Samson Log Server Trace Levels:  81-90
//
	LmtLogProvider  = 81,
	LmtLogProviderList,


//
// Samson Demo? Trace Levels:  91-100
//
	LMT_SAMSON_DEMO         = 91,



//
// Samson Supervisor Trace Levels:  101-110
//
	LMT_CONFIG_FILE = 101,
	LMT_CHECK,
	LMT_STARTER,
	LMT_PROCESS_LIST,
	LMT_SPAWNER_LIST,
	LMT_STARTER_LIST,
	LMT_PROCESS_LIST_TAB,
	LMT_QT
} TraceLevels;



/* ****************************************************************************
*
* traceLevelName - 
*/
extern char* traceLevelName(TraceLevels level);

#endif
