

#include "BufferList.h"
#include "engine/Notification.h"

namespace stream_connector {
BufferListItem::BufferListItem(engine::BufferPointer buffer, const std::string& file_name) : token("BufferListItem") {
  buffer_ = buffer;
  file_name_ = file_name;

  buffer_size = buffer->getSize();
  buffer_name = buffer->getName();
  buffer_type = buffer->getType();

  // Initial state
  state = on_memory;

  // Be able to receive notification from disk manager
  // listen( notification_disk_operation_request_response );
}

size_t BufferListItem::getSize() {
  return buffer_size;
}

size_t BufferListItem::getSizeOnMemory() {
  if (is_on_memory()) {
    return buffer_size;
  } else {
    return 0;
  }
}

bool BufferListItem::is_on_memory() {
  switch (state) {
    case on_memory:
    case on_memory_and_disk:
    case writing:
      return true;

    default:
      return false;
  }
  return buffer_size;
}

// Return buffer if it is on memory
engine::BufferPointer BufferListItem::buffer() {
  au::TokenTaker tt(&token);

  return buffer_;
}

// Method to receive a particular notification
void BufferListItem::notify(engine::Notification *notification) {
  au::TokenTaker tt(&token);

  if (notification->isName(notification_disk_operation_request_response)) {
    std::string type      = notification->environment().Get("type", "-");

    if (type == "write") {
      if (state != writing) {
        LM_W(("Unexpected state in BufferListItem"  ));
        return;
      }
      state = on_memory_and_disk;
      return;
    } else if (type == "read") {
      if (state != reading) {
        LM_W(("Unexpected state in BufferListItem"  ));
        return;
      }
      state = on_memory_and_disk;
      return;
    } else if (type == "remove") {
      // Nothing to do here...
    }

    return;
  }

  LM_W(("Unknown notification received in BufferListItem"));
}

void BufferListItem::flush_to_disk() {
  au::TokenTaker tt(&token);

  switch (state) {
    case on_memory_and_disk:
    {
      // Direct transition to on_disk
      state = on_disk;
      buffer_ = NULL;
      break;
    }

    case on_memory:
    {
      // squedule_write
      if (buffer_ == NULL) {
        LM_X(1, ("Internal error"));
      }
      engine::DiskOperation *o = engine::DiskOperation::newWriteOperation(buffer_, file_name_,
                                                                          getEngineId());
      au::SharedPointer<engine::DiskOperation> operation(o);
      engine::Engine::disk_manager()->Add(operation);

      state = writing;
      return;
    }

    default:
      break;
  }
}

void BufferListItem::load_from_disk() {
  au::TokenTaker tt(&token);

  switch (state) {
    case on_disk:
    {
      // Squedule reading
      // Create the buffer
      buffer_ = engine::Buffer::create(buffer_name, buffer_type, buffer_size);

      engine::DiskOperation::newReadOperation(buffer_->getData(), file_name_, 0, buffer_size, getEngineId());

      state = reading;
      return;
    }

    default:
      break;
  }
}

// --------------------------------------------------------
// BufferList
// --------------------------------------------------------

BufferList::BufferList() : token("BufferList") {
  // No limit in size
  max_size_on_memory_ = 0;
  file_id_ = 1;
}

BufferList::BufferList(std::string persistence_directory, size_t max_size_on_memory) : token("BufferList") {
  persistence_directory_ = persistence_directory;
  max_size_on_memory_ = max_size_on_memory;

  file_id_ = 1;
}

void BufferList::review_persistence() {
  if (max_size_on_memory_ == 0) {
    return;   // Nothing to do here
  }
  // Schedule read or write tasks
  au::list<BufferListItem>::iterator it;
  size_t total = 0;
  for (it = items.begin(); it != items.end(); it++) {
    BufferListItem *item = *it;

    if (total < max_size_on_memory_) {
      item->load_from_disk();
    } else {
      // Flushing to disk
      item->flush_to_disk();
    } total += item->getSize();
  }
}

// Insert a buffer in the system
void BufferList::push(engine::BufferPointer buffer) {
  au::TokenTaker tt(&token);
  std::string name = au::str("%s/buffer_%lu", persistence_directory_.c_str(), file_id_++);

  items.push_back(new BufferListItem(buffer, name));
}

// Pop the next buffer to be sent
engine::BufferPointer BufferList::pop() {
  au::TokenTaker tt(&token);

  if (items.size() == 0) {
    return engine::BufferPointer(NULL);
  }

  BufferListItem *item = items.front();
  engine::BufferPointer buffer = item->buffer();
  items.pop_front();
  delete item;

  return buffer;
}

void BufferList::extract_buffer_from(BufferList *buffer_list) {
  while (true) {
    engine::BufferPointer buffer = buffer_list->pop();
    if (buffer != NULL) {
      push(buffer);
    } else {
      break;
    }
  }
}

size_t BufferList::getSize() {
  au::TokenTaker tt(&token);
  size_t total = 0;

  au::list<BufferListItem>::iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    BufferListItem *item = *it;
    total += item->getSize();
  }
  return total;
}

size_t BufferList::getSizeOnMemory() {
  au::TokenTaker tt(&token);
  size_t total = 0;

  au::list<BufferListItem>::iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    BufferListItem *item = *it;
    total += item->getSizeOnMemory();
  }
  return total;
}

size_t BufferList::getNumBuffers() {
  return items.size();
}
}
