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


#include <sys/stat.h>                  // mkdir

#include "au/string/S.h"

#include "engine/MemoryManager.h"      // samson::MemoryManager
#include "engine/Notification.h"       // engine::Notification

#include "engine/Buffer.h"             // engine::Buffer
#include "engine/Notification.h"       // engine::Notificaiton

#include "samson/delilah/Delilah.h"    // samson::Delilah
#include "samson/network/Message.h"    // samson::Message
#include "samson/network/Packet.h"     // samson::Packet

#include "samson/common/KVHeader.h"
#include "samson/common/SamsonSetup.h"  // samson::SamsonSetup
#include "samson/common/samson.pb.h"   // network::...
// samson::MemoryInput , samson::MemoryOutput...

#include "PushDelilahComponent.h"      // Own interface


namespace samson {
void *run_PushDelilahComponent(void *p) {
  PushDelilahComponent *component = (PushDelilahComponent *)p;

  component->run_in_background();
  return NULL;
}

PushDelilahComponent::PushDelilahComponent(DataSource *data_source, const std::vector<std::string>& queues)
  : DelilahComponent(DelilahComponent::push)
    , token("PushDelilahComponent") {
  // copy Queues
  for (size_t i = 0; i < queues.size(); i++) {
    queues_.push_back(queues[i]);
  }

  // Data source
  data_source_ = data_source;

  uploaded_size_ = 0;
  sent_size_ = 0;

  uploading_module_ = false;     // By default we update a normal block of data

  // Set this to false ( true will be the end of processing data )
  finish_process = false;

  setConcept(
    au::str("Pushing data from '%s' to queue/s '%s'", data_source_->get_name().c_str(), queues_.str().c_str())
    );

  listen("push_operation_finished");
}

PushDelilahComponent::~PushDelilahComponent() {
  delete data_source_;
}

std::string PushDelilahComponent::getShortDescription() {
  if (isComponentFinished()) {
    std::ostringstream output;
    output << "[ ";
    output << "Id " << id << " ";
    output << "Finished uploaded " << au::str(uploaded_size_, "B");
    output << "]";
    return output.str();
  }

  std::ostringstream output;
  output << "[ ";
  output << "Id " << id << " ";
  output << "Pushed ";
  output << au::str(sent_size_, "B") << " Uploaded " << au::str(uploaded_size_, "B");
  output << "]";
  return output.str();
}

void PushDelilahComponent::run() {
  pthread_t t;

  au::Singleton<au::ThreadManager>::shared()->addThread("PushDelilahComponent", &t, NULL, run_PushDelilahComponent,
                                                        this);
}

void PushDelilahComponent::run_in_background() {
  while (true) {
    current_status_ = "Looping to push new data...";

    // If data_source if finished, just wait for push_operations to finish
    if (data_source_->isFinish()) {
      // Activate this flag just to show information correctly
      finish_process = true;

      current_status_ = "Waiting to finish scheduled push items...";
      while (true) {
        if (push_ids_.size() == 0) {
          LM_T(LmtDelilahComponent, ("push is finished"));
          setComponentFinished();
          return;
        }
        LM_T(LmtDelilahComponent, ("data_source is finished, sleeping with %lu push_ids", push_ids_.size()));
        usleep(10000);
      }
    }

    // Wait until memory is available
    au::Cronometer cronometer;
    current_status_ = "Waiting until used memory is under 70%";
    while (engine::Engine::memory_manager()->memory_usage() > 0.7) {
      LM_T(LmtDelilahComponent, ("Waiting until used memory(%d) is under 70%", engine::Engine::memory_manager()->memory_usage()));
      usleep(10000);
    }

    // Create a buffer to be filled
    size_t buffer_max_size = (64 * 1024 * 1024) + sizeof(KVHeader);
    engine::BufferPointer buffer = engine::Buffer::Create("PushDelilahComponent", "push", buffer_max_size);

    // Skip KVHeader to write the header at the end
    buffer->SkipWrite(sizeof(KVHeader));

    // Full the buffer with the content from the files
    current_status_ = au::S() <<  "Filling a new buffer " << buffer->str();
    if (data_source_->fill(buffer) != 0) {
      LM_E(("Error filling buffer from '%s'", data_source_->get_name().c_str()));
      setComponentFinishedWithError(au::str("Error filling buffer from '%s'", data_source_->get_name().c_str()));
      return;
    }

    // Set the header
    KVHeader *header = (KVHeader *)buffer->data();
    if (uploading_module_) {
      header->InitForModule(buffer->size() - sizeof(KVHeader));
    } else {
      header->InitForTxt(buffer->size() - sizeof(KVHeader));
    }

    // Get the size to update the total process info
    sent_size_ += buffer->size();

    // Add this buffer to be pushed by delilah
    current_status_ = au::S() <<  "Pushing block " << buffer->str() << " using delilah component ";
    size_t push_id = delilah->push(buffer, queues_);

    // Collect this push_id to be tracked...
    {
      au::TokenTaker tt(&token);
      push_ids_.insert(push_id);
    }
  }
}

// Notifications

void PushDelilahComponent::notify(engine::Notification *notification) {
  // Remove confirmed push operations
  if (notification->isName("push_operation_finished")) {
    au::TokenTaker tt(&token);
    size_t push_id = notification->environment().Get("push_id", (size_t)-1);
    size_t size    = notification->environment().Get("size", 0);
    uploaded_size_ += size;

    push_ids_.erase(push_id);
  }
}

std::string PushDelilahComponent::getStatus() {
  return au::str("Sent %s Uploaded %s"
                 , au::str(sent_size_, "B").c_str()
                 , au::str(uploaded_size_, "B").c_str());
}

std::string PushDelilahComponent::getExtraStatus() {
  std::ostringstream output;

  au::tables::Table table(au::StringVector("Concept", "Value"));

  table.setTitle(concept);


  {
    au::StringVector values;
    values.push_back("Current task");
    values.push_back(current_status_);
    table.addRow(values);
  }

  {
    au::StringVector values;
    values.push_back("Sent");
    values.push_back(au::str(sent_size_, "B"));
    table.addRow(values);
  }

  {
    au::StringVector values;
    values.push_back("Uploaded");
    values.push_back(au::str(uploaded_size_, "B"));
    table.addRow(values);
  }

  {
    au::StringVector values;
    values.push_back("Pending push operations");
    values.Push(push_ids_.size());
    table.addRow(values);
  }

  {
    au::StringVector values;
    values.push_back("Finish input data");
    values.Push(finish_process ? "yes" : "no");
    table.addRow(values);
  }



  output << table.str();
  return output.str();
}

void PushDelilahComponent::SetUploadModule() {
  uploading_module_ = true;
}
}
