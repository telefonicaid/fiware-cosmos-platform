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
  LmtCleanup                = 32,    // Valgrind ...
  LmtCleanup2               = 33,    // SharePointer destructor
  LmtRestData               = 34,
  LmtDelilahCommand         = 35,
  LmtDelilahComponent       = 36,

  LmtModuleManager          = 210,   // Module Manager component
  LmtThreadManager          = 211,   // Tracking threads in ThreadManager

  LmtIsolatedOperations     = 216,   // Traces to see start and stop of executions
  LmtIsolatedOutputs        = 218,   // See processing of operations outputs
  LmtFileDescriptors        = 219,   // to keep track of opening/closing file descriptors

  LmtUser01 = 250,
  LmtUser02 = 251,
  LmtUser03 = 252,
  LmtUser04 = 253,
  LmtUser05 = 254,
  LmtUser06 = 255,
} TraceLevels;




#endif  // ifndef TRACE_LEVELS_H
