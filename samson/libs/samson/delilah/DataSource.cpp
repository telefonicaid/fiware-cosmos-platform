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

#include "engine/Buffer.h"                                              // samson::Buffer
#include "engine/MemoryManager.h"                                       // samson::MemoryManager
#include "samson/common/SamsonSetup.h"                                  // samson::SamsonSetup
// network::...
#include "samson/delilah/Delilah.h"                                     // samson::Delilah
#include "samson/network/Message.h"                                     // samson::Message
#include "samson/network/Packet.h"                                      // samson::Packet

#include "DataSource.h"  // Own interface

namespace samson {
int AgregatedFilesDataSource::fill(engine::BufferPointer b) {
  // First the header

  if (finish_) {
    return 0;                           // Just in case
  }
  if (previous_buffer_) {
    if (previous_buffer_size_ > 0) {
      bool ans = b->Write(previous_buffer_, previous_buffer_size_);
      if (!ans) {
        LM_X(1, ("Error writing in a TXTFileSet"));
      }
    }
  }
  while (b->GetAvailableSizeToWrite() > 0) {                  // While there is space to fill
    b->Write(current_input_stream_);

    // Open the next file if necessary
    if (current_input_stream_.eof()) {
      current_input_stream_.close();
      OpenNextFile();
      if (finish_) {
        // Information in the header
        return 0;
      }
    }
  }

  // Remove previous buffer ( if any )
  if (previous_buffer_) {
    free(previous_buffer_);  // Remove the last chars until a complete line and keep for the next read
  }
  if (b->RemoveLastUnfinishedLine(previous_buffer_, previous_buffer_size_) != 0) {
    return 1;     // Error filling the buffer
  }
  // No error here
  return 0;
}
}
