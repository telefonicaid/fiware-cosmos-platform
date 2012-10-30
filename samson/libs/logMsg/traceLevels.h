/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
typedef enum TraceLevels {
  LmtFile,

  LmtExcesiveTime           = 5,

  LmtSocketConnection       = 10,    //
  LmtNetworkListener        = 11,    //

  LmtNetworkConnection      = 15,    //

  LmtNetworkNodeMessages    = 20,    // Worker & Delilah messages
  LmtNetworkInterface       = 22,    // Network interface

  LmtHost                   = 30,    // Host & HostManager
  LmtRest                   = 31,    // REST Interface
  LmtCleanup                = 32,    // Valgrind ...
  LmtCleanup2               = 33,    // SharePointer destructor
  LmtRestData               = 34,
  LmtDelilahCommand         = 35,
  LmtDelilahComponent       = 36,

  LmtEngine                 = 200,   // SamsonEngine
  LmtEngineNotification     = 201,
  LmtEngineTime             = 202,
  LmtEngineDiskManager      = 203,   // SamsonEngine
  LmtDisk                   = 204,   // disk library
  LmtMemory                 = 205,   // memory library
  LmtProcessManager         = 206,
  LmtBlockManager           = 207,

  LmtBuffer                 = 208,

  LmtModuleManager          = 210,   // Module Manager component
  LmtThreadManager          = 211,   // Tracking threads in ThreadManager

  LmtClusterSetup           = 212,   // Cluster setup ( interaction with zookeeper  )

  LmtWorkerTasks            = 213,   // Scheduing of tasks

  LmtIsolated               = 215,   // Isolated mechanism
  LmtIsolatedOperations     = 216,   // Traces to see start and stop of executions
  LmtReduceOperation        = 217,   // Reduce operation steps
  LmtIsolatedOutputs        = 218,   // See processing of operations outputs
  LmtFileDescriptors        = 219,   // to keep track of opening/closing file descriptors

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
extern char *traceLevelName(TraceLevels level);

#endif  // ifndef TRACE_LEVELS_H
