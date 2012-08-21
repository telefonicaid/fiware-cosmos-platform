
#ifndef _H_WORKER_COMMAND_DELILAH_COMPONENT
#define _H_WORKER_COMMAND_DELILAH_COMPONENT

#include "au/Cronometer.h"          // au::CronometerSystem
#include "au/ErrorManager.h"        // au::ErrorManager

#include <cstring>




#include "samson/common/samson.pb.h"
#include "samson/delilah/DelilahComponent.h"
#include "samson/network/Message.h"             // Message::MessageCode
#include "samson/network/Packet.h"              // samson::Packet

namespace engine {
class Buffer;
}

namespace samson {
class Delilah;


/**
 * Simple component created when a command is send to the controller ( waiting for answeres )
 */

class WorkerResponese {
public:

  size_t worker_id;
  au::ErrorManager error;

  WorkerResponese(size_t _worker_id) {
    worker_id = _worker_id;
  }

  WorkerResponese(size_t _worker_id, std::string error_message) {
    worker_id = _worker_id;
    error.set(error_message);
  }
};

class WorkerCommandDelilahComponent : public DelilahComponent {
  std::string command;

  engine::BufferPointer buffer_;

  std::set<size_t> workers;                         // Ids of the workers involved in this command
  au::map<size_t, WorkerResponese > responses;      // Map with all the responses from workers

  // Collections reported by workers
  au::map<std::string, gpb::Collection > collections;

  std::string main_command;

  size_t worker_id;                 // if != -1 --> worker to sent this command
  bool send_to_all_workers;         // -a
  bool save_in_database;            // -save
  std::string group_field;          // -group
  std::string filter_field;         // -filter
  std::string sort_field;           // -sort
  bool connected_workers;           // -connected
  int limit;                        // -limit

public:

  WorkerCommandDelilahComponent(std::string _command, engine::BufferPointer buffer);
  ~WorkerCommandDelilahComponent();

  void receive(const PacketPointer& packet);

  void run();

  std::string getStatus();


  au::tables::Table *getMainTable();


  static au::tables::Table *getStaticTable(gpb::Collection *collection);

private:

  // Internal function to print content of recevied collection
  void print_content(gpb::Collection *collection);

  // Transform a collection into a table
  au::tables::Table *getTable(gpb::Collection *collection);


  // Check if this command has to be sent to only one worker
  bool sendToAllWorker(const std::string& main_command);
};
}

#endif // ifndef _H_WORKER_COMMAND_DELILAH_COMPONENT