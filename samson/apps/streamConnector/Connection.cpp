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


#include "Adaptor.h"
#include "BufferProcessor.h"
#include "Connection.h"  // Own interface
#include "StreamConnector.h"
#include "au/file.h"
#include "au/singleton/Singleton.h"

extern char working_directory[1024];

namespace stream_connector {
Connection::Connection(Adaptor *item, ConnectionType type, std::string name) : token("Connection") {
  // Keep information
  type_ = type;
  item_ = item;
  description_ = name;

  // Buffer processor created on demand ( first time )
  buffer_processor_ = NULL;

  canceled_ = false;
  finished_ = false;
  initialized_ = false;

  output_buffer_list_ = NULL;
  input_buffer_list_ = NULL;

  id_ = SIZE_T_UNDEFINED;
}

Connection::~Connection() {
  if (buffer_processor_ != NULL) {
    delete buffer_processor_;
  }
  if (input_buffer_list_ != NULL) {
    delete input_buffer_list_;
  }
  if (output_buffer_list_ != NULL) {
    delete input_buffer_list_;
  }
}

void Connection::report_output_size(size_t size) {
  traffic_statistics.push_output(size);
  item_->report_output_size(size);
}

void Connection::report_input_size(size_t size) {
  traffic_statistics.push_input(size);
  item_->report_input_size(size);
}

engine::BufferPointer Connection::getNextBufferToSent() {
  // If output list is not created, there are no output buffers
  if (output_buffer_list_ == NULL) {
    return engine::BufferPointer(NULL);
  }

  // Extract buffer from the list
  engine::BufferPointer buffer = output_buffer_list_->pop();

  if (buffer != NULL) {
    report_output_size(buffer->size());
  }
  return buffer;
}

void Connection::pushInputBuffer(engine::BufferPointer buffer) {
  if (buffer == NULL) {
    return;
  }

  // Report input block
  report_input_size(buffer->size());

  if (item_->channel_->getSplitter() == "") {
    item_->channel_->push(buffer);
  } else {
    // Put in the input buffer list ??
    //
    // Currecntly the same thread for reading is used for processing si input_buffer_list is not really necessary.
    // An option in the future could be to inject input data in input_buffer_process and use engine to process stuff

    // Create buffer processor to process all input buffers
    if (buffer_processor_ == NULL) {
      buffer_processor_ = new BufferProcessor(item_->channel_);  // push the block processor
    }
    buffer_processor_->push(buffer);
  }
}

void Connection::flushInputBuffers() {
  if (buffer_processor_ != NULL) {
    buffer_processor_->flush();
  }
}

void Connection::push(engine::BufferPointer buffer) {
  if (output_buffer_list_ == NULL) {
    std::string directory = std::string(working_directory) + "/" + fullname() + "/output";
    size_t max_memory_size =  5 * 1024 * 1024;
    au::CreateFullDirectory(directory);
    output_buffer_list_ = new BufferList(directory, max_memory_size);
  }

  // Put in the list to be emitted
  output_buffer_list_->push(buffer);
}

size_t Connection::bufferedSize() const {
  if (type_ == connection_output) {
    if (output_buffer_list_ == NULL) {
      return 0;
    }
    return output_buffer_list_->getSize();
  } else {
    if (input_buffer_list_ == NULL) {
      return 0;
    }
    return input_buffer_list_->getSize();
  }
}

size_t Connection::bufferedSizeOnMemory() const {
  if (type_ == connection_output) {
    if (output_buffer_list_ == NULL) {
      return 0;
    }
    return output_buffer_list_->getSizeOnMemory();
  } else {
    if (!input_buffer_list_) {
      return 0;
    }
    return input_buffer_list_->getSizeOnMemory();
  }
}

// get type
ConnectionType Connection::type() const {
  return type_;
}

const char *Connection::GetTypeStr() const {
  return str_ConnectionType(type_);
}

std::string Connection::description() const {
  return description_;
}

size_t Connection::id() const {
  return id_;
}

std::string Connection::fullname() const {
  if (item_ == NULL) {
    return "[Unassigned connection]";
  }

  return au::str("%s.%lu"
                 , item_->fullname().c_str()
                 , id_
                 );
}

void Connection::set_as_finished() {
  if (finished_) {
    return;
  }
  finished_ = true;
}

bool Connection::is_finished() {
  return finished_;
}

void Connection::init_connecton() {
  if (initialized_) {
    return;
  }
  initialized_ = true;
  start_connection();
}

void Connection::cancel_connecton() {
  canceled_ = true;
  stop_connection();
}

void Connection::review() {
  if (canceled_) {
    return;   // Not call review
  }
  if (!finished_) {
    review_connection();  // Review persistancy in input/output buffer lists
  }
  if (input_buffer_list_) {
    input_buffer_list_->review_persistence();
  }
  if (output_buffer_list_) {
    output_buffer_list_->review_persistence();
  }
}

std::string Connection::str() {
  return au::str("%s %s [%s] In: %s Out: %s"
                 , fullname().c_str()
                 , description().c_str()
                 , getStatus().c_str()
                 , au::str(traffic_statistics.get_input_total(), "B").c_str()
                 , au ::str(traffic_statistics.get_output_total(), "B").c_str()
                 );
}

void Connection::cancel_connection() {
  stop_connection();   // Stop all theads ( so this can be deleted )
}
}
