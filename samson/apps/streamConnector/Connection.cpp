

#include "Adaptor.h"
#include "BufferProcessor.h"
#include "Connection.h"  // Own interface
#include "StreamConnector.h"
#include "au/Singleton.h"
#include "au/file.h"

extern char working_directory[1024];

namespace stream_connector {
Connection::Connection(Adaptor *_item, ConnectionType _type, std::string _name) : token("Connection") {
  // Keep information
  type = _type;
  item = _item;
  description_ = _name;

  // Buffer processor created on demand ( first time )
  buffer_processor = NULL;

  canceled = false;
  finished = false;
  initialized = false;

  output_buffer_list = NULL;
  input_buffer_list = NULL;

  id = (size_t )-1;
}

Connection::~Connection() {
  if (buffer_processor)
    delete buffer_processor; if (input_buffer_list)
    delete input_buffer_list; if (output_buffer_list)
    delete input_buffer_list;
}

void Connection::report_output_size(size_t size) {
  traffic_statistics.push_output(size);
  item->report_output_size(size);
}

void Connection::report_input_size(size_t size) {
  traffic_statistics.push_input(size);
  item->report_input_size(size);
}

engine::BufferPointer Connection::getNextBufferToSent() {
  // If output list is not created, there are no output buffers
  if (!output_buffer_list)
    return engine::BufferPointer(NULL);

  // Extract buffer from the list
  engine::BufferPointer buffer = output_buffer_list->pop();

  if (buffer != NULL)
    report_output_size(buffer->getSize()); return buffer;
}

void Connection::pushInputBuffer(engine::BufferPointer buffer) {
  if (buffer == NULL)
    return;

  // Report input block
  report_input_size(buffer->getSize());

  if (item->channel->getSplitter() == "") {
    item->channel->push(buffer);
  } else {
    // Put in the input buffer list ??
    //
    // Currecntly the same thread for reading is used for processing si input_buffer_list is not really necessary.
    // An option in the future could be to inject input data in input_buffer_process and use engine to process stuff

    // Create buffer processor to process all input buffers
    if (!buffer_processor) {
      buffer_processor = new BufferProcessor(item->channel);  // push the block processor
    }
    buffer_processor->push(buffer);
  }
}

void Connection::flushInputBuffers() {
  if (buffer_processor)
    buffer_processor->flush();
}

void Connection::push(engine::BufferPointer buffer) {
  if (!output_buffer_list) {
    std::string directory = std::string(working_directory) + "/" + getFullName() + "/output";
    size_t max_memory_size =  5 * 1024 * 1024;
    au::createFullDirectory(directory);
    output_buffer_list = new BufferList(directory, max_memory_size);
  }

  // Put in the list to be emitted
  output_buffer_list->push(buffer);
}

size_t Connection::bufferedSize() {
  if (type == connection_output) {
    if (!output_buffer_list)
      return 0; return output_buffer_list->getSize();
  } else {
    if (!input_buffer_list)
      return 0; return input_buffer_list->getSize();
  }
}

size_t Connection::bufferedSizeOnMemory() {
  if (type == connection_output) {
    if (!output_buffer_list)
      return 0; return output_buffer_list->getSizeOnMemory();
  } else {
    if (!input_buffer_list)
      return 0; return input_buffer_list->getSizeOnMemory();
  }
}

// get type
ConnectionType Connection::getType() {
  return type;
}

const char *Connection::getTypeStr() {
  return str_ConnectionType(type);
}

std::string Connection::getDescription() {
  return description_;
}

size_t Connection::getId() {
  return id;
}

std::string Connection::getFullName() {
  if (!item)
    return "[Unassigned connection]";

  return au::str("%s.%lu"
                 , item->getFullName().c_str()
                 , getId()
                 );
}

void Connection::set_as_finished() {
  if (finished)
    return;

  // Log activity
  log("Message", "Set as finished");

  finished = true;
}

bool Connection::is_finished() {
  return finished;
}

// Log system
void Connection::log(std::string type, std::string message) {
  log(new Log(getFullName(), type, message));
}

void Connection::log(Log *log) {
  LogManager *log_manager = au::Singleton<LogManager>::shared();

  log_manager->log(log);
}

void Connection::init_connecton() {
  if (initialized)
    return; initialized = true;
  start_connection();
}

void Connection::cancel_connecton() {
  canceled = true;
  stop_connection();
}

void Connection::review() {
  if (canceled) {
    return;   // Not call review
  }
  if (!finished) {
    review_connection();  // Review persistancy in input/output buffer lists
  }
  if (input_buffer_list)
    input_buffer_list->review_persistence(); if (output_buffer_list)
    output_buffer_list->review_persistence();
}

std::string Connection::str() {
  return au::str("%s %s [%s] In: %s Out: %s"
                 , getFullName().c_str()
                 , getDescription().c_str()
                 , getStatus().c_str()
                 , au::str(traffic_statistics.get_input_total(), "B").c_str()
                 , au::str(traffic_statistics.get_output_total(), "B").c_str()
                 );
}

void Connection::cancel_connection() {
  stop_connection();   // Stop all theads ( so this can be deleted )
}
}
