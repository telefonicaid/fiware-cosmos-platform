#ifndef _H_SAMSON_ZOO_DATA_MODEL
#define _H_SAMSON_ZOO_DATA_MODEL

#include <set>
#include <string>

#include "au/CommandLine.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/Uint64Vector.h"

#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/common/Visualitzation.h"
#include "samson/zoo/Connection.h"
#include "samson/zoo/ZooNodeCommiter.h"

#define NODE_WORKER_BASE "/samson/workers/w"

namespace samson {
// ------------------------------------------------------------------
//
// class DataModel
//
// Model for queue-data stream operations ( backed up in zookeeper )
//
// ------------------------------------------------------------------

class DataModel : public ZooNodeCommiter<gpb::Data> {
  public:
    static const std::string kDefaultSamsonDataPath;

    // Constant strings for valid commands
    static const std::string kAdd;
    static const std::string kAddQueueConnection;
    static const std::string kAddStreamOperation;
    static const std::string kBatch;
    static const std::string kBlock;
    static const std::string kClearBatchOPerations;
    static const std::string kClearModules;
    static const std::string kPushQueue;
    static const std::string kRemoveAll;
    static const std::string kRemoveAllData;
    static const std::string kRemoveAllStreamOperations;
    static const std::string kRemoveStreamOperation;
    static const std::string kRm;
    static const std::string kRmQueueConnection;
    static const std::string kSetQueueProperty;
    static const std::string kSetStreamOperationProperty;
    static const std::string kUnsetStreamOperationProperty;

    // Constant strings for item commands
    static const std::string kAddItem;
    static const std::string kRmItem;

    explicit DataModel(zoo::Connection *zoo_connection) :
      ZooNodeCommiter<gpb::Data> (zoo_connection, "/samson/data") {
    }
    virtual ~DataModel() {
    }

    // Groups the initialization of all flags to process command
    static void InitializeCommandFlags(au::CommandLine& cmd);

    // Functions to process each command type
    void ProcessAddCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                           au::ErrorManager */* error */);
    void ProcessAddQueueConnectionCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                          int version, au::ErrorManager */* error */);
    void ProcessAddStreamOperationCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                          int version, au::ErrorManager */* error */);
    void ProcessBatchCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                             au::ErrorManager */* error */);
    void ProcessBlockCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                             au::ErrorManager */* error */);
    void ProcessClearBatchOPerationsCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& cmd, int version,
                                            au::ErrorManager */* error */);
    void ProcessClearModulesCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                                    au::ErrorManager */* error */);
    void ProcessPushQueueCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                                 au::ErrorManager */* error */);
    void ProcessRemoveAllCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                                 au::ErrorManager */* error */);
    void ProcessRemoveAllDataCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                                     au::ErrorManager */* error */);
    void ProcessRemoveAllStreamOperationsCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                                 int version, au::ErrorManager */* error */);
    void ProcessRemoveStreamOperationCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                             int version, au::ErrorManager */* error */);
    void ProcessRmCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */, int version,
                          au::ErrorManager */* error */);
    void ProcessRmQueueConnectionCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                         int version, au::ErrorManager */* error */);
    void ProcessSetQueuePropertyCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                        int version, au::ErrorManager */* error */);
    void ProcessSetStreamOperationPropertyCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                                  int version, au::ErrorManager */* error */);
    void
    ProcessUnsetStreamOperationPropertyCommand(au::SharedPointer<gpb::Data> data, const au::CommandLine& /* cmd */,
                                               int version, au::ErrorManager */* error */);

    // ZooNodeCommiter<gpb::Data>
    virtual void PerformCommit(au::SharedPointer<gpb::Data>, std::string command, int version, au::ErrorManager *error);

    // Check if this command can be process by this element
    static bool isValidCommand(const std::string& main_command);

    // Get collection to be displayed on delilah
    au::SharedPointer<gpb::Collection> GetCollectionForQueues(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForQueueRanges(const Visualization& visualization,
                                                                   const std::string& queue_name);
    au::SharedPointer<gpb::Collection> GetCollectionForQueuesWithBlocks(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForStreamOperations(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForBatchOperations(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForQueueConnections(const Visualization& visualization);

    // Get list of all block_ids in the current data
    std::set<size_t> get_block_ids();

    // Get list of all block_ids I would have
    std::set<size_t> get_my_block_ids(const KVRanges& hg_ranges);

    // method trying to discover if all operations have finished
    bool CheckForAllOperationsFinished();

  private:
    void ReviewBatchOperations(au::SharedPointer<gpb::Data>, int version, au::ErrorManager *error);
    // method trying to discover if a batch operation is really finished
    bool CheckIfBatchOPerationIsFinished(const gpb::BatchOperation* const batch_operation,
                                         au::SharedPointer<gpb::Data> data) const;
};
}

#endif  // ifndef _H_SAMSON_ZOO_DATA_MODEL
