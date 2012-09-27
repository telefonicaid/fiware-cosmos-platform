#ifndef _H_BLOCK
#define _H_BLOCK

/* ****************************************************************************
 *
 * FILE                      Block.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Basic unit of data for stream processing
 * A block is a unit of data possibly in memory with replication of disk
 *
 */
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/containers/SharedPointer.h"
#include "au/containers/set.h"

#include "engine/MemoryManager.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVFile.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/NotificationListener.h"            // engien::EngineListener

#include "engine/ProcessItem.h"

#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct

#include "BlockInfo.h"                              // struct BlockInfo
#include "BlockLookupList.h"

#include <set>

namespace samson {
namespace stream {
class BlockList;

class Block {
public:

  typedef enum {
    on_memory,                                          // It is only in memory
    writing,                                            // A write order has been given
    ready,                                              // On disk and memory
    on_disk,                                            // Just on disk ( memory has been freed )
    reading                                             // Reading from disk to recover at memory
  } BlockState;

  Block(size_t block_id, engine::BufferPointer buffer);  // When block is in memory
  Block(size_t block_id, KVHeader *_header);  // When block is readed from disk
  ~Block();

  // Get Accessorrs
  BlockState state();
  au::SharedPointer<KVFile> getKVFile(au::ErrorManager& error);
  std::string file_name();
  int priority();
  size_t creation_time();
  KVHeader getHeader();
  size_t getSize();
  size_t getSizeOnMemory();
  size_t getSizeOnDisk();
  KVFormat getKVFormat();
  size_t getTime();
  size_t block_id();
  KVRange getKVRange();                // Working with KVRanges
  KVInfo getKVInfo();
  size_t worker_id();
  bool temporal();
  void set_no_temporal();
  size_t min_task_id()
  {
    return min_task_id_;
  }

  // Set accesors
  void set_priority(int _priority);

  // Handy function to print
  std::string str_state();

  // Lookup for a particular queue ( lookupList is created if necessary )
  std::string lookup(const char *key, std::string outputFormat);

  // Handy function to check if this block can be removed from manager
  bool canBeRemoved();

  bool is_content_in_memory();              // Check if content is on memory
  bool is_content_in_disk();                // Check if content is on disk
  bool is_content_locked_in_memory();       // Check if block is locked in memory

  // Add to a list
  void add_block_list(BlockList *list);
  void remove_block_list(BlockList *list);

  // lock content of this block in memory
  void lock_content_in_memory(BlockList *list);
  void unlock_content_in_memory(BlockList *list);


  // Get information about this block
  void update(BlockInfo &block_info);

  // Free content from memory ( lookuplist is also removed )
  void freeBlock();

  // Debug string
  std::string str();

  // Collection-like monitorization information
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization , size_t accumulated_size );

  // Get internal buffer with data
  engine::BufferPointer buffer();

  static bool compare(Block *b1, Block *b2);

private:

  // Update sort information
  void update_sort_information();

  // Identifiers of this block
  size_t block_id_;

  // State disk-memory
  BlockState state_;

  // Buffer of data ( if content of this block is on memory )
  engine::BufferPointer buffer_;

  // Copy of the header ( always in memory )
  KVHeader header;

  // Lock up table to quick access to key-values from REST interface
  au::Token token_lookupList;
  BlockLookupList *lookupList;

  // Creation time of this block
  au::Cronometer cronometer;

  // Containers of this block
  au::set<BlockList> block_lists_;             // List simply containing this block
  au::set<BlockList> block_lists_lock_;        // Lists locking this block in memory

  // Information used to compare two blcoks
  size_t min_task_id_;
  int max_priority_;

  // Information about how key-values are organized in hash-groups
  au::SharedPointer<KVFile> file_;
  au::Token token_file_;

  // Tempora flag ( means that this block has not been pushed into the model )
  bool temporal_;

  friend class BlockManager;
  friend class BlockLookupList;
};

typedef au::SharedPointer<Block>   BlockPointer;
}
}

#endif  // ifndef _H_BLOCK
