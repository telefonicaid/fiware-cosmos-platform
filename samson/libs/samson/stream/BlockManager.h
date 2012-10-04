#ifndef _H_BLOCK_MANAGER
#define _H_BLOCK_MANAGER

#include "Block.h"                      // samson::stream::Block
#include "au/containers/Dictionary.h"
#include "au/containers/SharedPointer.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/NotificationListener.h"  // engien::EngineListener
#include "engine/NotificationListener.h"              // engine::NotificationListener

#include <vector>

namespace samson {
namespace stream {
class BlockList;

/*
 
 BlockManager
 
 Manager of all the blocks running on the system
 Main responsible to keep blocks of data in memory for operations
 
 */


class BlockManager : public engine::NotificationListener {

public:

  // Singleton
  static void init();
  static BlockManager *shared();
  static void destroy();

  // Create blocks
  void CreateBlock(size_t block_id, engine::BufferPointer buffer);

  // Get a particular block
  BlockPointer GetBlock(size_t _id);

  // Reset the entire block manager
  void ResetBlockManager();

  // Function to review pending read / free / write operations
  void Review();

  // Remove blocks not included in this list
  void RemoveBlocksIfNecessary(const std::set<size_t>& all_blocks
                               , const std::set<size_t>& my_blocks
                               , const std::set<size_t>& worker_ids);

  // Notification interface
  virtual void notify(engine::Notification *notification);

  // Get collection of blocks for remote listing
  gpb::Collection *GetCollectionOfBlocks(const Visualization& visualization);

  size_t scheduled_write_size() {
    return scheduled_write_size_;
  }

  size_t scheduled_read_size() {
    return scheduled_read_size_;
  }

  // Get all block identifiers
  std::set<size_t> GetBlockIds();

private:

  BlockManager(); // Private constructor & destructir for singleton implementation
  ~BlockManager();
  
  void Sort(); // Sort blocks
  void CreateBlockFromDisk(const std::string& path);
  void RecoverBlocksFromDisks();

  void ScheduleRemoveOperation(BlockPointer block);
  void ScheduleReadOperation(BlockPointer block);
  void ScheduleWriteOperation(BlockPointer block);

  au::Dictionary<size_t, Block> blocks_;     // Dictionary of blocks
  std::list<size_t> block_ids_;              // list of block identifiers in order

  size_t scheduled_write_size_;               // Amount of bytes scheduled to be writen to disk
  size_t scheduled_read_size_;                // Amount of bytes scheduled to be read from disk
  size_t max_memory_;                         // Maximum amount of memory to be used by this block manager

  au::Token token_;                          // Mutex protection since operations create blocks in multiple threads
};
}
}

#endif  // ifndef _H_BLOCK_MANAGER
