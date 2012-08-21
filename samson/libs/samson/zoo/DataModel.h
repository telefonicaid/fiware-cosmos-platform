
#ifndef _H_SAMSON_ZOO_DATA_MODEL
#define _H_SAMSON_ZOO_DATA_MODEL

#include "au/containers/Uint64Vector.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/Object.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/zoo/CommitCommand.h"
#include "samson/zoo/Connection.h"
#include "samson/zoo/ZooNodeCommiter.h"

#include "samson/common/MessagesOperations.h"
#include "samson/common/Visualitzation.h"

#define NODE_WORKER_BASE "/samson/workers/w"

namespace samson {
// ------------------------------------------------------------------
//
// class DataModel
//
// Model for queue-data stream operations ( backed up in zookeeper )
//
// ------------------------------------------------------------------

class DataModel : public ZooNodeCommiter<gpb::Data>{
public:

  DataModel(zoo::Connection *zoo_connection);
  virtual ~DataModel();

  // ZooNodeCommiter<gpb::Data>
  virtual void PerformCommit(au::SharedPointer<gpb::Data>, std::string command, int version,  au::ErrorManager *error);

  // Check if this command can be process by this element
  static bool isValidCommand(const std::string& main_command);

  // Get collection to be displayed on delilah
  gpb::Collection *getCollectionForQueues(const Visualization& visualization);
  gpb::Collection *getCollectionForQueuesWithBlocks(const Visualization& visualization);
  gpb::Collection *getCollectionForStreamOperations(const Visualization& visualization);
  gpb::Collection *getCollectionForBatchOperations(const Visualization& visualization);
  gpb::Collection *getCollectionForQueueConnections(const Visualization& visualization);


  // Get list of all block_ids in the current data
  std::set<size_t> get_block_ids();

  // Get list of all block_ids I whould have
  std::set<size_t> get_my_block_ids(const KVRanges& hg_ranges);

private:

  void ReviewBatchOperations(au::SharedPointer<gpb::Data>, int version,  au::ErrorManager *error);
};
}

#endif // ifndef _H_SAMSON_ZOO_DATA_MODEL
