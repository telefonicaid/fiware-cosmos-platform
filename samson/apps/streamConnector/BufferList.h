

#ifndef _H_STREAM_CONNECTOR_BUFFER_LIST
#define _H_STREAM_CONNECTOR_BUFFER_LIST

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/tables/Table.h"

#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"


namespace stream_connector {
// ------------------------------------------------------------------------------------
//
// Class BufferList
//
// List of buffers with a persistance mechanism based on DiskManager / Engine
//
// ------------------------------------------------------------------------------------

class BufferListItem : public engine::NotificationListener {
  engine::BufferPointer buffer_;      // Buffer if it is on memory
  std::string file_name_;                         // Full name of the file if it is necessary to persist on disk

  size_t buffer_size;
  std::string buffer_name;
  std::string buffer_type;

  au::Token token;

  typedef enum {
    on_memory,
    writing,
    on_memory_and_disk,
    reading,
    on_disk,
  } State;

  State state;

public:

  BufferListItem(engine::BufferPointer buffer, const std::string& file_name);

  // Get size information
  size_t getSize();
  size_t getSizeOnMemory();

  // Check is on memory
  bool is_on_memory();

  // Return buffer if it is on memory
  engine::BufferPointer buffer();

  // Method to receive a particular notification
  virtual void notify(engine::Notification *notification);
  void flush_to_disk();
  void load_from_disk();
};


// Buffer of blocks with a maximum size
class BufferList {
  au::Token token;

  // Persistancy
  std::string persistence_directory_;   // Place to store excedent messages
  size_t max_size_on_memory_;           // Max size to keep on memory

  size_t file_id_;                      // Interal counter to give names

  // List of objects
  au::list<BufferListItem> items;   // List of items containing blocks

public:

  BufferList();
  BufferList(std::string persistence_directory, size_t max_size_on_memory);

  // Review persistance based on max_size_on_memory_
  void review_persistence();

  // Insert a buffer in the system
  void push(engine::BufferPointer buffer);

  // Pop the next buffer to be sent
  engine::BufferPointer pop();

  void extract_buffer_from(BufferList *buffer_list);

  size_t getSize();

  size_t getSizeOnMemory();
  size_t getNumBuffers();
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_BUFFER_LIST
