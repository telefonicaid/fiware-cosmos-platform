#ifndef TRACE_LEVELS_H
#define TRACE_LEVELS_H

/* ****************************************************************************
*
* FILE                     traceLevels.h - trace levels for entire Samson project
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/



enum PaTraceLevels
{
   LmtPaSetup          = 0,
   LmtPaConfigAction,
   LmtPaDefVal,
   LmtPaEnvVal,
   LmtPaRcFileVal,
   LmtPaComLineVal,
   LmtPaApVals,
   LmtPaLimits,
   LmtPaUsage,
   LmtPaRcFile,
   LmtPaSList,
   LmtPaIList,
   LmtHelp
};



//
// Network Trace Levels
//
#define LMT_NWRUN       51
#define LMT_SELECT      52
#define LMT_FDS         53
#define LMT_ENDPOINT    54
#define LMT_ACCEPT      55
#define LMT_ENDPOINTS   56
#define LMT_READ        57
#define LMT_WORKERS     58
#define LMT_WRITE       59
#define LMT_MSG         60
#define LMT_INIT_EP     61
#define LMT_CONFIG      62
#define LMT_HELLO       63
#define LMT_TREAT       64
#define LMT_RECONNECT   65
#define LMT_DELILAH     66
#define LMT_STAT        67
#define LMT_EP          68
#define LMT_RESTART     69
#define LMT_COREWORKER  70
#define LMT_MSGTREAT    71
#define LMT_JOB         72
#define LMT_TIMEOUT     73
#define LMT_FORWARD     74
#define LMT_SEND        75



//
// Process Trace Levels
//
#define LMT_COREWORKER       21
#define LMT_PA               22



//
// Data Trace Levels
//
#define LMT_FILE       81



//
// Core Trace Levels
//
#define LMT_CONFIG                81
#define LMT_TASK                  82
#define LMT_DELILAH               83
#define LMT_SAMSON_WORKER         84
#define LMT_WINIT                 85



//
// Samson Log Server Trace Levels
//
#define LMT_LOG_PROVIDER_LIST 201



//
// Samson Demo? Trace Levels
//
#define LMT_SAMSON_DEMO       201



//
// Samson Supervisor Trace Levels
//
#define LMT_CONFIG_FILE       201
#define LMT_CHECK             202
#define LMT_STARTER           203
#define LMT_PROCESS_LIST      204
#define LMT_SPAWNER_LIST      205
#define LMT_STARTER_LIST      206
#define LMT_PROCESS_LIST_TAB  207
#define LMT_QT                208

#endif
