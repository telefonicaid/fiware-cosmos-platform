#include "BufferContainer.h"  // Own interface

namespace  samson {

void BufferContainer::Push(const std::string& queue_name, engine::BufferPointer buffer) {
  au::TokenTaker tt(&token_);

  if (buffer == NULL) {
    return;
  }

  if (buffer->size() == 0) {
    return;
  }

  au::Queue<engine::Buffer> *queue = queues_.findOrCreate(queue_name);
  queue->Push(buffer);
}



engine::BufferPointer BufferContainer::Pop(const std::string& queue_name) {
  au::TokenTaker tt(&token_);

  au::Queue<engine::Buffer> *queue = queues_.findInMap(queue_name);

  if (!queue) {
    LM_W(("queue '%s' not found", queue_name.c_str()));
    return engine::BufferPointer(NULL);
  } else {
    return queue->Pop();
  }
}

}
