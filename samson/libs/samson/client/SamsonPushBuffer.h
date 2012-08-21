#ifndef _H_SamsonPushBuffer_SamsonClient
#define _H_SamsonPushBuffer_SamsonClient

#include "au/Rate.h"
#include "au/mutex/Token.h"
#include <string>



namespace  samson {
class SamsonClient;


/*
 * Class used to push data to a txt queue in buffer mode
 */

class SamsonPushBuffer {
  SamsonClient *samson_client_;        // Client to use to push data
  std::string queue_;                  // Queue to push data to

  // Buffer
  engine::BufferPointer buffer_;

  // Mutes protection
  au::Token token_;

public:

  // Statistics about rate
  au::rate::Rate rate_;

  // Constructor
  SamsonPushBuffer(SamsonClient *client, std::string queue);
  ~SamsonPushBuffer();

  // Method to push data
  void push(const char *data, size_t length, bool flushing);

  // Flush accumulated data to samson client
  void flush();

  // Recevie notifications
  void notify(engine::Notification *notification);
};
}

#endif // ifndef _H_SamsonPushBuffer_SamsonClient