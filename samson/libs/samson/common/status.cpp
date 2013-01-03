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
 * FILE                     status.cpp
 *
 * DESCRIPTION              Status values for most of samson methods
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Jun 06 2011
 *
 */
#include "status.h"



namespace samson {
/* *******************************************************************************
 *
 * status -
 */
const char *status(Status s) {
  switch (s) {
    case OK:                   return "OK";

    case NotImplemented:       return "Not Implemented";

    case BadMsgType:           return "BadMsgType";

    case NullHost:             return "Null Host";

    case BadHost:              return "Bad Host";

    case NullPort:             return "Null Port";

    case Duplicated:           return "Duplicated";

    case KillError:            return "Kill Error";

    case NotHello:             return "Not Hello";

    case NotAck:               return "Not an Ack";

    case NotMsg:               return "Not a Msg";

    case Error:                return "Error";

    case ConnectError:         return "Connect Error";

    case AcceptError:          return "Accept Error";

    case NotListener:          return "Not a Listener";

    case SelectError:          return "Select Error";

    case SocketError:          return "Socket Error";

    case GetHostByNameError:   return "Get Host By Name Error";

    case BindError:            return "Bind Error";

    case ListenError:          return "Listen Error";

    case ReadError:            return "Read Error";

    case WriteError:           return "Write Error";

    case Timeout:              return "Timeout";

    case ConnectionClosed:     return "Connection Closed";

    case PThreadError:         return "Thread Error";

    case WrongPacketHeader:    return "Wrong Packet Header";

    case ErrorParsingGoogleProtocolBuffers: return "Error parsing Google Protocol Buffer message";
  }

  return "Unknown Status";
}

Status au_status(au::Status s) {
  switch (s) {
    case au::OK:                      return OK;

    case au::Error:                   return Error;

    case au::NotImplemented:          return NotImplemented;

    case au::GetHostByNameError:      return GetHostByNameError;

    case au::SelectError:
    case au::Timeout:                 return Timeout;

    case au::ConnectError:            return ConnectError;

    case au::AcceptError:             return AcceptError;

    case au::SocketError:             return SocketError;

    case au::BindError:               return BindError;

    case au::ListenError:             return ListenError;

    case au::ReadError:               return ReadError;

    case au::WriteError:              return WriteError;

    case au::ConnectionClosed:        return ConnectError;

    case au::OpenError:

    case au::GPB_Timeout:
    case au::GPB_ClosedPipe:
    case au::GPB_ReadError:
    case au::GPB_CorruptedHeader:
    case au::GPB_WrongReadSize:
    case au::GPB_ReadErrorParsing:
    case au::GPB_NotInitializedMessage:
    case au::GPB_WriteErrorSerializing:
    case au::GPB_WriteError:

      return Error;
  }

  return Error;
}
}
