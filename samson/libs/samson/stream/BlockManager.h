#ifndef _H_BLOCK_MANAGER
#define _H_BLOCK_MANAGER

#include "Block.h"                      // samson::stream::Block
#include "au/containers/Dictionary.h"
#include "au/containers/SharedPointer.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/Object.h"  // engien::EngineListener
#include "engine/Object.h"              // engine::Object

#include <vector>

namespace samson {
namespace stream {
class BlockList;

/**
 * Manager of all the blocks running on the system
 */


class BlockManager : public engine::Object {
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
  void create_block(size_t block_id, KVHeader *header);

  // Get a particular block
  BlockPointer getBlock(size_t _id);

  // Reset the entire block manager
  void resetBlockManager();

  // Function to review pending read / free / write operations
  void review();

  // Remove blocks not included in this list
  void RemoveBlocksNotIncluded(const std::set<size_t>& blocks);

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

  std::set<size_t> GetPendingBlockIds(const std::set<size_t>& block_ids);
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
};
}
}

#endif  // ifndef _H_BLOCK_MANAGER
