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
 * FILE                     Message.cpp - message definitions for all Samson IPC
 *
 */
#include "samson/network/Message.h"            // Own interface

namespace samson {
namespace Message {
/* ****************************************************************************
 *
 * messageCode -
 */

const char *messageCode(MessageCode code) {
  switch (code) {
    case Hello:                            return "Hello";
    case StatusReport:                     return "StatusReport";
    case Alert:                            return "Alert";
    case ClusterInfoUpdate:                return "ClusterInfoUpdate";
    case PushBlock:                        return "PushBlock";
    case PushBlockResponse:                return "PushBlockResponse";
    case PopQueue:                         return "PopQueue";
    case PopQueueResponse:                 return "PopQueueResponse";
    case PopBlockRequest:                  return "PopBlockRequest";
    case PopBlockRequestConfirmation:      return "PopBlockRequestConfirmation";
    case PopBlockRequestResponse:          return "PopBlockRequestResponse";
    case BlockRequest:                     return "BlockRequest";
    case BlockRequestResponse:             return "BlockRequestResponse";
    case StreamOutQueue:                   return "StreamOutQueue";
    case WorkerCommand:                    return "WorkerCommand";
    case WorkerCommandResponse:            return "WorkerCommandResponse";
    case Message:                          return "Message";
    case Unknown:                          return "Unknown";
    default:                               LM_W(("Unknown message code(%d)", static_cast<int>(code)));
  }

  return (char *)"Unknown";
}
}
}
