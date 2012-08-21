

#include <sys/stat.h>                  // mkdir

#include "au/S.h"

#include "engine/MemoryManager.h"      // samson::MemoryManager
#include "engine/MemoryRequest.h"
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

  au::ThreadManager::shared()->addThread("PushDelilahComponent", &t, NULL, run_PushDelilahComponent, this);
}

void PushDelilahComponent::run_in_background() {
  while (true) {
    current_status_ = "Looping to push new data...";

    // If data_source if finished, just wait for push_operations to finish
    if (data_source_->isFinish()) {
      // Activate this flag just to show information correctly
      finish_process = true;

      while (true) {
        if (push_ids_.size() == 0) {
          setComponentFinished();
          return;
        }
      }
    }

    // Wait until memory is available
    au::Cronometer cronometer;
    current_status_ = "Waiting until used memory is under 70%";
    while (engine::MemoryManager::shared()->memory_usage() > 0.7) {
      usleep(100000);
    }

    // Create a buffer to be filled
    engine::BufferPointer buffer = engine::Buffer::create("PushDelilahComponent", "push", 64 * 1024 * 1024);

    // Skip KVHeader to write the header at the end
    buffer->skipWrite(sizeof(KVHeader));

    // Full the buffer with the content from the files
    current_status_ = au::S() <<  "Filling a new buffer " << buffer->str();
    if (data_source_->fill(buffer) != 0) {
      setComponentFinishedWithError(au::str("Error filling buffer from '%s'", data_source_->get_name().c_str()));
      return;
    }

    // Set the header
    KVHeader *header = (KVHeader *)buffer->getData();
    header->initForTxt(buffer->getSize() - sizeof(KVHeader));

    // Get the size to update the total process info
    sent_size_ += buffer->getSize();

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
    size_t push_id = notification->environment().get("push_id", (size_t)-1);
    size_t size    = notification->environment().get("size", 0);
    uploaded_size_ += size;

    push_ids_.erase(push_id);
  }
}

std::string PushDelilahComponent::getStatus() {
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
}
