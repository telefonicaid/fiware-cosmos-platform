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

    case PushBlockCommit:                  return "PushBlockCommit";

    case PushBlockCommitResponse:          return "PushBlockCommitRensponse";

    case PopQueue:                         return "PopQueue";

    case PopQueueResponse:                 return "PopQueueResponse";

    case PopBlockRequest:                  return "PopBlockRequest";

    case PopBlockRequestConfirmation:      return "PopBlockRequestConfirmation";

    case PopBlockRequestResponse:          return "PopBlockRequestResponse";

    case BlockDistribution:                return "BlockDistribution";

    case BlockDistributionResponse:        return "BlockDistributionResponse";

    case BlockRequest:                     return "BlockRequest";

    case StreamOutQueue:                   return "StreamOutQueue";

    case WorkerCommand:                    return "WorkerCommand";

    case WorkerCommandResponse:            return "WorkerCommandResponse";

    case Message:                          return "Message";

    case Unknown:                          return "Unknown";
  }

  return (char *)"Unknown";
}
}
}
