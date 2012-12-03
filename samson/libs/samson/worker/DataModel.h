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
#ifndef _H_SAMSON_ZOO_DATA_MODEL
#define _H_SAMSON_ZOO_DATA_MODEL

#include <set>
#include <string>

#include "au/CommandLine.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/Uint64Vector.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/gpb_operations.h"

#include "zoo/Connection.h"
#include "zoo/ZooNodeCommiter.h"

#define NODE_WORKER_BASE "/samson/workers/w"

// ------------------------------------------------------------------
//
// class DataModel
//
// Complete model for data and operations for samson
//
// It currently lives inside ZK for simplicity
//
// It codifies all information about queues, operations, batch operations etc...
// ------------------------------------------------------------------

namespace samson {
class WorkerBlockData;

class DataModel : public au::StringDataModel<gpb::DataModel> {
public:

  explicit DataModel(au::zoo::Connection *zoo_connection) :
    au::StringDataModel<gpb::DataModel> (zoo_connection, kDefaultSamsonDataPath) {
  }

  virtual ~DataModel() {
  }

  virtual void Init(au::SharedPointer<gpb::DataModel> c);
  virtual void PerformCommit(au::SharedPointer<gpb::DataModel>, std::string command, int version, au::ErrorManager&);

  // Check if this command can be process by this element
  static bool isValidCommand(const std::string& main_command);

  // Get collection to be displayed on delilah console
  au::SharedPointer<gpb::Collection> GetCollectionForQueues(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForQueueRanges(const Visualization& v, const std::string& queue_name);
  au::SharedPointer<gpb::Collection> GetCollectionForQueuesWithBlocks(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForStreamOperations(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForBatchOperations(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForQueueConnections(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForReplication(const Visualization& visualization);

  // Get list of all block_ids in the current data ( previous, candidate and current )
  std::set<size_t> GetAllBlockIds();

  // Get list of all block_ids I should have
  std::set<size_t> GetMyBlockIdsForPreviousAndCandidateDataModel(const std::vector<KVRange>& ranges);
  std::set<size_t> GetMyBlockIdsForPreviousDataModel(const std::vector<KVRange>& ranges);
  std::set<size_t> GetMyBlockIdsForCandidateDataModel(const std::vector<KVRange>& ranges);
  std::set<size_t> GetMyStateBlockIdsForCurrentDataModel(const std::vector<KVRange>& ranges);

  size_t GetLastCommitIdForCurrentDataModel() const;
  size_t GetLastCommitIdForCandidateDataModel() const;
  size_t GetLastCommitIdForPreviousDataModel() const;

  // Method to discover if all operations have finished ( see wait command in delilah )
  bool CheckForAllStreamOperationsFinished();
  bool CheckForAllBatchOperationsFinished(size_t delilah_id = SIZE_T_UNDEFINED);

  // Frezze data model if necessary
  void FreezeCandidateDataModel();

  // Get list of the last commits
  au::SharedPointer<gpb::Collection> GetLastCommitsCollection(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetLastCommitsDebugCollection(const Visualization& visualization);

private:

  void ProcessCommand(gpb::Data *data, const std::string command, au::ErrorManager& error) {
    au::SharedPointer<au::CommandLine> cmd = GetCommandLine();
    cmd->Parse(command);
    ProcessCommand(data, cmd, error);
  }

  void ProcessCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);

  void ProcessAddCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessAddQueueConnectionCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                        au::ErrorManager& error);
  void ProcessAddStreamOperationCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                        au::ErrorManager& error);
  void ProcessBatchCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessBlockCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessClearBatchOPerationsCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                          au::ErrorManager& error);
  void ProcessClearModulesCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessPushQueueCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessRemoveAllCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessRemoveAllDataCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessRemoveAllStreamOperationsCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                               au::ErrorManager& error);
  void ProcessRemoveStreamOperationCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                           au::ErrorManager& error);
  void ProcessRmCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessRmQueueConnectionCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessSetQueuePropertyCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd, au::ErrorManager& error);
  void ProcessSetStreamOperationPropertyCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                                au::ErrorManager& error);
  void ProcessUnsetStreamOperationPropertyCommand(gpb::Data *data, au::SharedPointer<au::CommandLine> cmd,
                                                  au::ErrorManager& error);
  void ProcessFreezeDataModel(au::SharedPointer<gpb::DataModel> data_model, au::ErrorManager& error);
  void ProcessCancelFreezeDataModel(au::SharedPointer<gpb::DataModel> data_model, au::ErrorManager& error);
  void ProcessRecoverDataModel(au::SharedPointer<gpb::DataModel> data_model, au::ErrorManager& error);
  void ProcessConsolidateDataModel(au::SharedPointer<gpb::DataModel> data_model, au::ErrorManager& error);

  // Review batch operations
  void ReviewBatchOperations(gpb::Data *data, au::ErrorManager& error);

  // Check if it is necesssary to run this while recovering
  bool IsRecoveryCommand(const std::string& command);
  // Default path in ZK for this information
  static const std::string kDefaultSamsonDataPath;

  // Groups the initialization of all flags to process command
  static au::SharedPointer<au::CommandLine> GetCommandLine();

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
  static const std::string kSetReplicationFactor;
  static const std::string kSetStreamOperationProperty;
  static const std::string kUnsetStreamOperationProperty;
  static const std::string kFreezeDataModel;
  static const std::string kCancelFreezeDataModel;
  static const std::string kRecoverDataModel;
  static const std::string kConsolidateDataModel;

  static const std::string commands[];
  static const std::string recovery_commands[];

  // Constant strings for item commands
  static const std::string kAddItem;
  static const std::string kRmItem;
};
}

#endif  // ifndef _H_SAMSON_ZOO_DATA_MODEL
