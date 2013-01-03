
#include <string>

#include "samson/common/common.h"

namespace samson {
/**
 * \brief BlockSortInfo is the unit of information for a block necessary to establish priorities between blocks
 *
 * Blocks are sorted in order to decide whitch ones are on-memory for next processing tasks
 */

class BlockSortInfo {
public:

  BlockSortInfo() {
    min_task_id_ = SIZE_T_UNDEFINED;

    so_pos_ = SIZE_T_UNDEFINED;
    so_is_state_ = false;

    extern_so_pos_ = SIZE_T_UNDEFINED;
  }

  void NotifyQueue(const std::string& queue) {
    queues_.insert(queue);     // Accumulate list of queues
  }

  void NotifyQueueInPreviousDataModel(const std::string& queue) {
    old_queues_.insert(queue);     // Accumulate list of queues
  }

  /**
   * \brief Notify this block belongs to a stream operation input.
   * \param state Inform if this queue is a "state" is a reduce operation ( if so, it has priority )
   */

  void NotifyInputForStreamOperation(const std::string&  stream_operation_name
                                     , const std::string& queue_name
                                     , bool state
                                     , size_t pos);



  /**
   * \brief Inform that this block belongs to a stream operation input for other workers
   */
  void NotifyInputForExternStreamOperation(const std::string&  stream_operation_name
                                           , const std::string& queue_name
                                           , size_t pos);

/**
 * \brief Notify This block is in que input queue of a schduled task ( higher priority )
 * It is important to keep the minimum task_id since it will be the higher priority task
 */
  void NotifyInputForTask(size_t task) {
    tasks_.insert(task);
    if (( min_task_id_ == SIZE_T_UNDEFINED ) || ( min_task_id_ > task )) {
      min_task_id_ = task;
    }
  }

  size_t min_task_id() {
    return min_task_id_;
  }

  /**
   * \brief Give a string with information about the priority of this block
   */

  std::string State() const;

  /**
   * \brief Compare the priority of this block with respecte to another one
   * Return if this block has higher priority than the provided ( "other" )
   */

  bool Compare(BlockSortInfo *other);

  /**
   * \brief Check if this block belongs only to the .modules queue ( lower priority )
   */
  bool IsOnlyInModulesQueue();

private:

  size_t min_task_id_;                 // Minimum task
  std::set<size_t> tasks_;             // List of task
  std::set<std::string> queues_;       // List of queues
  std::set<std::string> old_queues_;   // List of queues in the old data-model

  // Higher priority information for a stream operation in this worker
  size_t so_pos_;
  bool so_is_state_;
  std::string so_name_;
  std::string so_queue_;

  // Information about being input for stream operations in other workers
  size_t extern_so_pos_;
  std::string extern_so_name_;
  std::string extern_so_queue_;
};


/**
 *
 * \brief GlobalBlockSortInfo global information to sort all blocks
 *
 * It contains all relevant information for all the blocks in this worker
 * It is used in BlockManager to sort all blocks and remove unnecessary ones.
 *
 * Remeber that sorting blocks is important for putting preferent blocks in memory
 *
 */

class GlobalBlockSortInfo {
public:

  GlobalBlockSortInfo() {
  }

  ~GlobalBlockSortInfo() {
    info_.clearMap();
  }

  BlockSortInfo *Get(size_t block_id) {
    return info_.findInMap(block_id);
  }

  bool IsBlockIsNecessary(size_t block_id) {
    return ( info_.findInMap(block_id) != NULL );
  }

  void NotifyQueueInPreviousDataModel(size_t block_id, const std::string& queue) {
    info_.findOrCreate(block_id)->NotifyQueueInPreviousDataModel(queue);
  }

  void NotifyQueue(size_t block_id, const std::string& queue) {
    info_.findOrCreate(block_id)->NotifyQueue(queue);
  }

  void NotifyInputForTask(size_t block_id, size_t task_id) {
    info_.findOrCreate(block_id)->NotifyInputForTask(task_id);
  }

  void NotifyInputForStreamOperation(size_t block_id, const std::string&  stream_operation_name,
                                     const std::string& queue_name, bool state,
                                     size_t pos) {
    info_.findOrCreate(block_id)->NotifyInputForStreamOperation(stream_operation_name, queue_name, state, pos);
  }

  void NotifyInputForExternStreamOperation(size_t block_id, const std::string&  stream_operation_name,
                                           const std::string& queue_name, size_t pos) {
    info_.findOrCreate(block_id)->NotifyInputForExternStreamOperation(stream_operation_name, queue_name, pos);
  }

  // String state for each block
  std::string GetStateForBlock(size_t block_id) {
    BlockSortInfo *info = info_.findInMap(block_id);

    if (!info) {
      return "To be removed";
    }
    return info->State();
  }

  // Sort two blocks
  bool CompareBlocks(size_t block_id1, size_t block_id2) const {
    BlockSortInfo *info_1 = info_.findInMap(block_id1);
    BlockSortInfo *info_2 = info_.findInMap(block_id2);

    if (!info_2) {
      return true;
    }

    if (!info_1) {
      return false;
    }

    // Other criterias...
    return info_1->Compare(info_2);
  }

private:

  au::map<size_t, BlockSortInfo> info_;     // map of information for all blocks
};
}