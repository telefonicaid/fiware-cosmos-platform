#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

#include <cstring>

/* ****************************************************************************
 *
 * FILE                     Message.h - message definitions for all Samson IPC
 *
 */

namespace samson {
namespace Message {
/* ****************************************************************************
 *
 * MessageCode
 */
typedef enum MessageCode {
  Hello,                        // Identification message in all interconnections worker - delilah or worker - worker
  StatusReport,
  Alert,
  ClusterInfoUpdate,            // Update information about the cluster to a delilah node
  WorkerCommand,                // Command from a delilah node to a worker
  WorkerCommandResponse,
  PushBlock,                    // Push a block from delilah to a worker
  PushBlockResponse,            // Confirmation from worker that block has been distributed in the cluster
  PushBlockCommit,              // Command to commit uploaded block to data model
  PushBlockCommitResponse,      // Confirmation from worker to delilah that block has been commited to data model

  PopQueue,                     // Request from delilah for a particular queue ( returns information about queue )
  PopQueueResponse,             // Response from worker

  PopBlockRequest,              // Request from delilah for a block and range
  PopBlockRequestConfirmation,      // Confirmation this block can be served
  PopBlockRequestResponse,          // Block response from worker

  StreamOutQueue,     // Unused message?

  Message,

  BlockDistribution,                // Message to distribute a block from a worker to another worker
  BlockDistributionResponse,        // Answer from the target so the paquet has been received
  BlockRequest,                     // Request a particular block to a worker ( when starting worker )

  Unknown,
} MessageCode;



/* ****************************************************************************
 *
 * Header -
 */

typedef struct Header {
  size_t magic;
  MessageCode code;
  size_t gbufLen;
  size_t kvDataLen;

  bool check() {
    if (magic != 4050769273219470657) {
      return false;
    }

    if (gbufLen > 10000000) {
      return false;
    }
    if (kvDataLen > ( 200 * 1024 * 1024 )) {
      return false;
    }

    return true;
  }

  void setMagicNumber() {
    magic = 4050769273219470657;
  }
} Header;


/* ****************************************************************************
 *
 * messageCode -
 */

const char *messageCode(MessageCode code);
}
}      // end of namespace samson::Message

#endif  // ifndef SAMSON_MESSAGE_H
