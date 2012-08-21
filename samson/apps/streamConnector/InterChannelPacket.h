
#ifndef _H_INTER_CHANNEL_PACKET
#define _H_INTER_CHANNEL_PACKET

#include "au/Object.h"
#include "au/Status.h"
#include "au/network/FileDescriptor.h"
#include "message.pb.h"
#include <string>


#define InterChannelPacketHeader_magic_number 876875

namespace stream_connector {
// ------------------------------------------------------------------
//
// InterChannelPacket
//
// Unit of information exchanged in a InterChannel link
//
// ------------------------------------------------------------------

typedef struct {
  int magic_number;      // Magic number to check sync
  size_t message_size;   // Size of the companion GPB message
  size_t buffer_size;    // Optional buffer size
} InterChannelPacketHeader;

class InterChannelPacket : public au::Object {
  InterChannelPacketHeader header_;            // Header
  Message *message;                            // GPB Message with additional information
  engine::BufferPointer buffer_;   // Optional buffer with data

public:

  InterChannelPacket();
  ~InterChannelPacket();

  // GPB message inside the packet
  Message *getMessage();

  // Set and get methods for data buffer
  engine::BufferPointer buffer();
  void set_buffer(engine::BufferPointer buffer);

  // Read and write over filedescriptor
  au::Status read(au::FileDescriptor *fd);
  au::Status write(au::FileDescriptor *fd);

  // Get information
  size_t getSize();
  std::string str();

private:

  bool is_valid_packet();
  void recompute_sizes_in_header();
};
}

#endif  // ifndef _H_INTER_CHANNEL_PACKET

