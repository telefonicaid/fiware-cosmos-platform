

#include "au/mutex/TokenTaker.h"
#include "samson/common/coding.h"

#include "samson/client/SamsonClient.h"

#include "SamsonPushBuffer.h"  // Own interface


namespace samson {
SamsonPushBuffer::SamsonPushBuffer(SamsonClient *client, std::string queue) : token_("SamsonPushBuffer") {
  // Client and queue name to push data to
  samson_client_ = client;
  queue_ = queue;

  // Init buffer
  buffer_ = engine::Buffer::Create("SamsonPushBuffer", "push", 64 * 1024 * 1024);
}

SamsonPushBuffer::~SamsonPushBuffer() {
}

void SamsonPushBuffer::push(const char *data, size_t size, bool flushing) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Statistics
  rate_.Push(size);

  if (( size + buffer_->size()) > buffer_->max_size()) {
    flush();
  }
  if (size > 1024 * 1024 * 1024) {
    LM_X(1, ("Non supported size to push %s", au::str(size, "B").c_str()));
  }
  if (size > buffer_->size()) {
    flush();   // Flush current buffer
    // Create another buffer to meet the size
    buffer_ = engine::Buffer::Create("SamsonPushBuffer", "push", size);
  }

  buffer_->Write(data, size);

  LM_V(("Accumulated %s in push buffer", au::str(buffer_->size(), "B").c_str()));

  if (flushing) {
    flush();
  }
}

void SamsonPushBuffer::flush() {
  au::TokenTaker tt(&token_);


  if (buffer_->size() > 0) {
    // Push to the client
    LM_V(("SamsonPushBuffer: Pushing a bufer %s to SAMSON queue %s"
          , au::str(buffer_->size()).c_str()
          , queue_.c_str()));

    samson_client_->push(buffer_, queue_);

    // Create a new buffer to continue
    buffer_ = engine::Buffer::Create("SamsonPushBuffer", "push", 64 * 1024 * 1024);
  } else {
    LM_V(("Not flishing since no data accumulated in push_buffer "));
  }
}
}