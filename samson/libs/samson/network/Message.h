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
#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

#include <cstring>

#include "logMsg/logMsg.h"
/* ****************************************************************************
 *
 * FILE                     Message.h - message definitions for all Samson IPC
 *
 */

namespace samson {
/**
 * \brief Namespace for messages used in SAMSON library
 */
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

  PopQueue,                     // Request from delilah for a particular queue ( returns information about queue )
  PopQueueResponse,             // Response from worker

  PopBlockRequest,              // Request from delilah for a block and range
  PopBlockRequestConfirmation,      // Confirmation this block can be served
  PopBlockRequestResponse,      // Block response from worker

  StreamOutQueue,               // Unused message?

  Message,

  BlockRequest,                 // Request a particular block to a worker
  BlockRequestResponse,         // Answer to the block

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

  static const size_t kMagicNumber = 4050769273219470657;
  static const size_t kMaxBufLen = 10000000;
  static const size_t kMaxKvDataLen = 200 * 1024 * 1024;

  bool Check() const {
    if (magic != kMagicNumber) {
      LM_E(("Error checking received header, wrong magic number header(%lu) != MagicNumber(%lu)", magic, kMagicNumber));
      return false;
    }
    if (gbufLen > kMaxBufLen) {
      LM_E(("Error checking received header, wrong length(%lu) > MaxBufLen(%lu)", gbufLen, kMaxBufLen));
      return false;
    }
    if (kvDataLen > kMaxKvDataLen) {
      LM_E(("Error checking received header, wrong kvDataLen(%lu) > MaxKvDataLen(%lu)", kvDataLen, kMaxKvDataLen));
      return false;
    }
    return true;
  }

  void setMagicNumber() {
    magic = kMagicNumber;
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
