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

/**
 * Manager of all the blocks running on the system
 */


class BlockManager : public engine::NotificationListener {
  BlockManager();                     // Private constructor for singleton implementation
  ~BlockManager();

public:

  // Singleton
  static void init();
  static BlockManager *shared();
  static void destroy();

public:

  // Create blocks
  void create_block(size_t block_id, engine::BufferPointer buffer);

  // Get a particular block
  BlockPointer getBlock(size_t _id);

  // Reset the entire block manager
  void resetBlockManager();

  // Function to review pending read / free / write operations
  void review();

  // Remove blocks not included in this list
  void RemoveBlocksIfNecessary(const std::set<size_t>& all_blocks
                               , const std::set<size_t>& my_blocks
                               , const std::set<size_t>& worker_ids);

  // Notification interface
  virtual void notify(engine::Notification *notification);

  // To be removed....
  void update(BlockInfo &block_info);

  // Get collection of blocks for remote listing
  gpb::Collection *getCollectionOfBlocks(const Visualization& visualization);

  size_t get_scheduled_write_size() {
    return scheduled_write_size;
  }

  size_t get_scheduled_read_size() {
    return scheduled_read_size;
  }

  // Get all block identifiers
  std::set<size_t> GetBlockIds();

private:

  void create_block_from_disk(const std::string& path);
  void recover_blocks_from_disks();

  void schedule_remove_operation(BlockPointer block);
  void schedule_read_operation(BlockPointer block);
  void schedule_write_operation(BlockPointer block);

  au::Dictionary<size_t, Block> blocks_;     // Dictionary of blocks
  std::list<size_t> block_ids_;       // list of block identifiers in order

  size_t scheduled_write_size;        // Amount of bytes scheduled to be writen to disk
  size_t scheduled_read_size;         // Amount of bytes scheduled to be read from disk
  size_t max_memory;                  // Maximum amount of memory to be used by this block manager

  au::Token token_;                   // Mutex protection since operations create blocks in multiple threads
};
}
}

#endif  // ifndef _H_BLOCK_MANAGER
