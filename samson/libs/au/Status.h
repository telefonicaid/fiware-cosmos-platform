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

/* ****************************************************************************
*
* FILE            status.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            September 2011
*
* DESCRIPTION
*
*      Definition of all the returned values in au library
*
* ****************************************************************************/

#ifndef _H_AU_STATUS
#define _H_AU_STATUS

namespace au {
typedef enum Status {
  OK,
  Error,          // Generic error

  NotImplemented,

  Timeout,

  OpenError,       // Error in the open call

  ConnectError,
  AcceptError,
  SelectError,
  SocketError,
  BindError,
  ListenError,
  ReadError,
  WriteError,
  ConnectionClosed,
  GetHostByNameError,

  // Google Protocol Buffer errros

  GPB_Timeout,
  GPB_ClosedPipe,
  GPB_ReadError,
  GPB_CorruptedHeader,
  GPB_WrongReadSize,
  GPB_ReadErrorParsing,
  GPB_NotInitializedMessage,
  GPB_WriteErrorSerializing,
  GPB_WriteError,
} Status;

const char *status(Status code);
}

#endif  // ifndef _H_AU_STATUS
