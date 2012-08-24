
#include "engine/Buffer.h"                                              // samson::Buffer
#include "engine/MemoryManager.h"                                       // samson::MemoryManager
#include "samson/common/SamsonSetup.h"                                  // samson::SamsonSetup
#include "samson/common/samson.pb.h"                                    // network::...
#include "samson/delilah/Delilah.h"                                     // samson::Delilah
#include "samson/network/Message.h"                                     // samson::Message
#include "samson/network/Packet.h"                                      // samson::Packet

#include "TXTFileSet.h"  // Own interface

namespace samson {
int TXTFileSet::fill(engine::BufferPointer b) {
  // First the header

  if (finish) {
    return 0;                           // Just in case
  }
  if (previousBuffer) {
    if (previousBufferSize > 0) {
      bool ans = b->Write(previousBuffer, previousBufferSize);
      if (!ans) {
        LM_X(1, ("Error writing in a TXTFileSet"));
      }
    }
  }
  while (b->GetAvailableSizeToWrite() > 0) {                  // While there is space to fill
    b->Write(inputStream);

    // Open the next file if necessary
    if (inputStream.eof()) {
      inputStream.close();
      openNextFile();
      if (finish) {
        // Information in the header
        return 0;
      }
    }
  }

  // Remove previous buffer ( if any )
  if (previousBuffer) {
    free(previousBuffer);  // Remove the last chars until a complete line and keep for the next read
  }
  if (b->RemoveLastUnfinishedLine(previousBuffer, previousBufferSize) != 0) {
    return 1;     // Error filling the buffer
  }
  // No error here
  return 0;
}
}
