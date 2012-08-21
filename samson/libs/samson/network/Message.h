#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

#include <cstring>

/* ****************************************************************************
 *
 * FILE                     Message.h - message definitions for all Samson IPC
 *
 */

namespace samson { namespace Message {
                   /* ****************************************************************************
                    *
                    * MessageCode
                    */
                   typedef enum MessageCode {
                     Hello,

                     StatusReport,

                     Alert,

                     ClusterInfoUpdate,

                     WorkerCommand,
                     WorkerCommandResponse,

                     PushBlock,         // Push a block from delilah
                     PushBlockResponse,  // Confirmation from worker that block has been distributed

                     PushBlockCommit,   // Commit uploaded block
                     PushBlockCommitResponse,  // Confirmation from worker that block has been commited

                     PopQueue,          // Request for information
                     PopQueueResponse,

                     PopBlockRequest,   // Request for a block and range
                     PopBlockRequestConfirmation,  // Confirmation this block can be served
                     PopBlockRequestResponse,  // Block response

                     StreamOutQueue,

                     Message,

                     DuplicateBlock,
                     DuplicateBlockResponse,

                     BlockRequest,         // requesting a blocl
                     BlockRequestResponse,  // Return content of a block

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
                       if (magic != 4050769273219470657)
                         return false;




                       if (gbufLen > 10000000)
                         return false;




                       if (kvDataLen > ( 200 * 1024 * 1024 ))
                         return false;




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
                   } }  // end of namespace samson::Message

#endif  // ifndef SAMSON_MESSAGE_H
